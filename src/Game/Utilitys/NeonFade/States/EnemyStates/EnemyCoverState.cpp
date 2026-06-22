#include "EnemyCoverState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Components/EnemyController.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyCoverApproachState.h"

namespace NeonFade {
	EnemyCoverState::EnemyCoverState(Enemy* owner_enemy_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		owner_enemy = owner_enemy_;
		animator = owner_enemy->animator.lock().get();
		rb = owner_enemy->rb.lock().get();
		std::function<bool()> default_exit = [this]() {
			return elapsed_time >= COVER_DURATION;
			};
		RegisterChangeRequest("idle", default_exit, 1);

		std::function<bool()> continue_cover = [this]() {
			//カバー対象がいなくなっていたらカバーを続行せずにidleに移る
			if (!covering_enemy)
				return false;

			//カバー対象を中心とした、プレイヤーと自分の位置関係を計算する
			const Vector3& player_pos = owner_enemy->enem_controller->GetPlayer()->transform->position;
			Vector3 cover_to_player = player_pos - covering_enemy->transform->position;
			const Vector3& my_pos = owner_enemy->transform->position;
			Vector3 cover_to_me = my_pos - covering_enemy->transform->position;

			cover_to_player.y = 0; // 水平方向のベクトルにする
			cover_to_me.y = 0; // 水平方向のベクトルにする

			//正規化する前に、距離で可否を判断する
			//この時点で引っかかれば正規化のコスト削減ができるのでお得
			{
				float to_player_dist_sqr = cover_to_player.magnitudeSquared();
				float to_me_dist_sqr = cover_to_me.magnitudeSquared();
				// 自分がカバー対象よりもプレイヤーから遠い(プレイヤーの奥まで行ってしまった)場合はカバーを続行せずにidleに移る
				if (to_me_dist_sqr > to_player_dist_sqr)
					return false;
			}

			cover_to_me.normalize();
			cover_to_player.normalize();


			//カバー対象からプレイヤー、カバー対象から自分へのベクトルを計算し、両者のなす角が規定以内であればカバーを続行する

			//両者のなす角のcos値を計算する
			static const float COVER_CONTINUE_COS_THRESHOLD = cosf(DEG2RAD(COVER_CONTINUE_ANGLE_THRESHOLD));
			float cosine_angle = cover_to_me.dot(cover_to_player);

			//なす角が規定以内で、かつカバー状態の持続時間が規定を越えてしまった場合はidleに移らずにカバーを続行する
			return elapsed_time >= COVER_DURATION && cosine_angle >= COVER_CONTINUE_COS_THRESHOLD;
			};
		RegisterChangeRequest("cover", continue_cover, 0); // カバー状態を続行する条件を優先度高めで登録
	}
	void EnemyCoverState::OnEnter(IStateMachine* machine)
	{
		elapsed_time = 0.0f;
		//カバーに入るときのアニメーションを再生する
		//アニメーションの長さより先にカバー状態が終わる可能性があるため、ループ再生する
		//自身から遷移してきた場合は、同じアニメーションを再生し直さないようにする
		animator->PlayIfNoSame("enemy_cover", true);

		auto approach_state = machine->GetState("cover_approach");

		// カバーに入る際、カバーに近づく状態で設定されていたカバー対象を引き継ぐ
		//基本的にこのステートは近付き状態もしくは自身からしか遷移できないので、
		if (approach_state) {
			auto approach_casted = static_cast<EnemyCoverApproachState*>(approach_state);
			covering_enemy = approach_casted->GetCoverTarget();
		}
	}
	void EnemyCoverState::OnExit(IStateMachine* machine)
	{}
	void EnemyCoverState::Update(IStateMachine* machine, float dt)
	{
		//生ポで参照しているので、不正アクセスを防ぐために、カバー対象が弱っている敵のリストからいなくなった場合は、カバー対象をリセットする
		const std::vector<Enemy*>& weakened_enemies = Enemy::GetWeakenedEnemies();
		if (std::find(weakened_enemies.begin(), weakened_enemies.end(), covering_enemy) == weakened_enemies.end())
			covering_enemy = nullptr;	// カバー対象が弱っている敵のリストからいなくなった場合は、カバー対象をリセットする

		Vector3 player_pos = owner_enemy->enem_controller->GetPlayer()->transform->position;
		const Vector3& my_pos = owner_enemy->transform->position;
		Vector3 to_player = player_pos - my_pos;
		to_player.y = 0; // 水平方向のベクトルにする
		if (to_player.magnitudeSquared() > 0.001f) { // プレイヤーとほぼ同じ位置にいる場合は回転しない
			to_player.normalize();
			to_player = Slerp(owner_enemy->transform->AxisZ(), to_player, ROTATION_SPEED); // 現在の向きとプレイヤー方向の間をスムーズに補間する)
			owner_enemy->transform->SetAxisZ(to_player); //プレイヤーから仲間を守るので、敵はプレイヤーの方を向くように回転させる
		}
		elapsed_time += dt;

		// 仲間との群集行動の計算
		Vector3 mov_dir = { 0, 0, 0 };
		CalculateCohesion(mov_dir);
		ApplyMovement(mov_dir);
	}
	bool EnemyCoverState::CanTransitTo(const std::string& state_name)
	{
		if (state_name == "knock_back" || state_name == "knock_front")
			return true;
		return false;
	}

	void EnemyCoverState::CalculateCohesion(Vector3& out_mov_dir)
	{

		Vector3 cohesion = Vector3(0, 0, 0);
		// 近くの敵に少し引き寄せられるようにする
		const std::vector<Enemy*>& all_enemies = Enemy::GetAllEnemies();
		const u32 num_enemies = static_cast<u32>(all_enemies.size());
		Vector3 owner_pos = owner_enemy->transform->position;
		for (u32 i = 0; i < num_enemies; ++i) {
			Enemy* enemy = all_enemies[i];
			if (enemy != owner_enemy) {
				Vector3 to_enemy = enemy->transform->position - owner_pos;
				float distance_squared = to_enemy.magnitudeSquared();
				distance_squared = max(distance_squared, 0.0001f); // ゼロ除算を防ぐために最小値を設定

				static constexpr float APPROACH_DISTANCE_MAX = 50.0f; // 影響を与える最大距離
				static constexpr float APPROACH_DISTANCE_MIN = 15.0f; // 影響を与える最小距離
				static constexpr float LEAVE_DISTANCE_MIN = 10.0f; // あまりにも近い場合に離れる距離
				static constexpr float APMAX_SQRD = APPROACH_DISTANCE_MAX * APPROACH_DISTANCE_MAX; // 影響を与える最大距離の二乗
				static constexpr float APMIN_SQRD = APPROACH_DISTANCE_MIN * APPROACH_DISTANCE_MIN; // 影響を与える最小距離の二乗
				static constexpr float LEAVE_SQRD = LEAVE_DISTANCE_MIN * LEAVE_DISTANCE_MIN; // あまりにも近い場合に離れる距離の二乗
				static constexpr float APPROACH_FACTOR = 0.02f; // 近くの敵に引き寄せられる係数
				static constexpr float LEAVE_FACTOR = 2.5f; // あまりにも近い場合に離れる係数

				if (distance_squared < APMAX_SQRD && distance_squared > APMIN_SQRD) { // 近くの敵に対してのみ影響を与える
					float sqrt_dist = sqrtf(distance_squared);
					float x = (sqrt_dist - APPROACH_DISTANCE_MIN) / (APPROACH_DISTANCE_MAX - APPROACH_DISTANCE_MIN); // 影響の強さを距離に応じて変化させる
					float smoothstep = x * x * (3 - 2 * x); // 影響の強さを距離に応じて変化させる

					cohesion += to_enemy.getNormalized() * smoothstep * APPROACH_FACTOR; // 引き寄せる方向に力を加える
				}
				if (distance_squared < LEAVE_SQRD) { // あまりにも近い場合は少し離れるようにする
					float sqrt_dist = sqrtf(distance_squared);
					float x = (LEAVE_DISTANCE_MIN - sqrt_dist) / LEAVE_DISTANCE_MIN; // 影響の強さを距離に応じて変化させる
					float smoothstep = x * x * (3 - 2 * x); // 影響の強さを距離に応じて変化させる

					cohesion -= to_enemy.getNormalized() * smoothstep * LEAVE_FACTOR; // 離れる方向に力を加える
				}
			}
		}

		float dot = out_mov_dir.dot(cohesion);
		float direction_factor = (dot >= 0) ? 1.0f : 0.5f; // 引き寄せる方向と同じならそのまま、逆なら半分の力にする
		out_mov_dir += cohesion * direction_factor; // 引き寄せる力を加える
	}
	void EnemyCoverState::ApplyMovement(Vector3& mov_dir)
	{
		mov_dir.y = 0; // Y軸の回転を無効にする
		mov_dir.normalize();
		mov_dir *= COHESION_WEIGHT; // 凝集力の重みを掛ける
		mov_dir.y = rb->velocity.y; // 現在のY軸の速度を保持
		rb->velocity = mov_dir;
	}
}