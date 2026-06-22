#include "EnemyCoverApproachState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Components/EnemyController.h"

namespace NeonFade {
	EnemyCoverApproachState::EnemyCoverApproachState(Enemy* owner_enemy_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		owner_enemy = owner_enemy_;
		animator = owner_enemy->animator.lock().get();
		rb = owner_enemy->rb.lock().get();

		// 一定時間経過したら状態遷移を要求する条件を定義するラムダ関数
		std::function<bool()> default_exit = [this]() {
			// カバー位置に近づくのに時間がかかりすぎている、またはカバー対象がいない場合は、状態遷移を要求する
			return elapsed_time >= MAX_APPROACH_TIME || !cover_target;
			};
		RegisterChangeRequest("idle", default_exit, 1);

		// カバー位置に近づいたとみなす条件を定義するラムダ関数
		std::function<bool()> cover_exit = [this]() {
			static constexpr float DEST_THRESHOLD_SQUARED = APPROACH_DISTANCE_THRESHOLD * APPROACH_DISTANCE_THRESHOLD; // 目的地に近いとみなす距離の二乗
			Vector3 to_destination = approach_destination - owner_enemy->transform->position;
			to_destination.y = 0.0f; // y軸方向の距離は無視する
			return to_destination.magnitudeSquared() < DEST_THRESHOLD_SQUARED;
			};
		RegisterChangeRequest("cover", cover_exit, 0);

	}
	void EnemyCoverApproachState::OnEnter(IStateMachine* machine)
	{
		elapsed_time = 0.0f;
		animator->Play("enemy_escape", true);
		// カバー位置に近づくための目的地を計算する
		CalculateApproachDestination();
	}
	void EnemyCoverApproachState::Update(IStateMachine* machine, float dt)
	{
		//生ポで参照しているので、不正アクセスを防ぐために、カバー対象が弱っている敵のリストからいなくなった場合は、カバー対象をリセットする
		const std::vector<Enemy*>& weakened_enemies = Enemy::GetWeakenedEnemies();
		if (std::find(weakened_enemies.begin(), weakened_enemies.end(), cover_target) == weakened_enemies.end())
			cover_target = nullptr;	// カバー対象が弱っている敵のリストからいなくなった場合は、カバー対象をリセットする

		elapsed_time += dt;
		Vector3 mov_dir = Vector3(0, 0, 0);
		CalculateBaseMovement(mov_dir);
		CalculateCohesion(mov_dir);

		ApplyMovementAndRotation(mov_dir);

	}
	void EnemyCoverApproachState::OnExit(IStateMachine* machine)
	{}
	void EnemyCoverApproachState::DebugDraw()
	{
		DxLib::DrawSphere3D(cast(approach_destination), 0.5f, 8, Color::MAGENTA, Color::MAGENTA, false);
	}
	bool EnemyCoverApproachState::CanTransitTo(const std::string& state_name)
	{
		if (state_name == "knock_back" || state_name == "knock_front" || state_name == "damage")
			return true;
		return false;
	}
	void EnemyCoverApproachState::CalculateApproachDestination()
	{
		const Vector3& my_pos = owner_enemy->transform->position;
		Vector3 other_pos = owner_enemy->transform->position;	// 自身の位置を初期値として設定(他の敵がいない場合は自身の位置を目的地とし、離脱条件を満たす)
		float min_distance_to_other = FLT_MAX;
		const std::vector<Enemy*>& weakened_enemies = Enemy::GetWeakenedEnemies();
		if (weakened_enemies.empty())
			return;
		// 他の敵の中で最も近い敵の位置を取得する
		for (auto& other_enemy : weakened_enemies)
		{
			if (other_enemy == owner_enemy)
				continue;
			float distance_to_other = (other_enemy->transform->position - my_pos).magnitudeSquared();
			if (distance_to_other < min_distance_to_other)
			{
				min_distance_to_other = distance_to_other;
				other_pos = other_enemy->transform->position;
				cover_target = other_enemy;	// 最も近い敵をカバー対象として設定する
			}
		}
		// 他の敵の位置とプレイヤーの位置を考慮して、カバー位置に近づくための目的地を計算する
		Vector3 player_pos = owner_enemy->enem_controller->GetPlayer()->transform->position;

		// 他の敵の位置とプレイヤーの位置の中間点に近づくように目的地を設定する(APPROACH_POSITION_RATIOの割合で補間)
		approach_destination = Lerp(player_pos, other_pos, APPROACH_POSITION_RATIO);
		approach_destination.y = 0.0f; // y軸方向の高さを固定
		// 目的地をランダムにずらすことで、敵が同じ位置に集まるのを防ぐ
		approach_destination += Random::Position({ -APPROACH_DESTINATION_RADIUS, 0.0f, -APPROACH_DESTINATION_RADIUS }, { APPROACH_DESTINATION_RADIUS, 0.0f, APPROACH_DESTINATION_RADIUS });
	}
	void EnemyCoverApproachState::CalculateBaseMovement(Vector3& out_mov_dir)
	{
		// カバー位置に近づくための移動方向を計算する
		Vector3 to_destination = approach_destination - owner_enemy->transform->position;
		to_destination.normalize();
		out_mov_dir += to_destination * APPROACH_SPEED;
	}
	void EnemyCoverApproachState::CalculateCohesion(Vector3& out_mov_dir)
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
	void EnemyCoverApproachState::ApplyMovementAndRotation(Vector3& mov_dir)
	{
		mov_dir.y = 0; // Y軸の回転を無効にする
		mov_dir.normalize();
		Vector3 forward = owner_enemy->transform->AxisZ();
		mov_dir = Slerp(forward, mov_dir, ROTATION_SPEED);
		owner_enemy->transform->SetAxisZ(mov_dir); // 目的地への方向を向く
		mov_dir *= APPROACH_SPEED;

		mov_dir.y = rb->velocity.y; // 現在のY軸の速度を保持
		rb->velocity = mov_dir;
	}
}