#include "EnemyCoverState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Components/EnemyController.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyCoverApproachState.h"

#include "Game/Utilitys/NeonFade/EnemyBrain/EnemyRVOSystem.h"

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
				static const float TARGET_DISTANCE_THRESHOLD_SQR = TARGET_DISTANCE_THRESHOLD * TARGET_DISTANCE_THRESHOLD;
				//カバー対象が離脱完了(一定距離逃げる時間稼ぎ)した場合はカバーを続行せずにidleに移る
				if (to_me_dist_sqr > TARGET_DISTANCE_THRESHOLD_SQR)
					return false;
			}

			cover_to_me.normalize();
			cover_to_player.normalize();


			//カバー対象からプレイヤー、カバー対象から自分へのベクトルを計算し、両者のなす角が規定以内であればカバーを続行する

			//両者のなす角のcos値を計算する
			static const float COVER_CONTINUE_COS_THRESHOLD = cosf(DEG2RAD(COVER_CONTINUE_ANGLE_THRESHOLD));
			float cosine_angle = cover_to_me.dot(cover_to_player);
			bool is_angle_within_threshold = cosine_angle >= COVER_CONTINUE_COS_THRESHOLD;

			//なす角が規定以内で、かつカバー状態の持続時間が規定を越えてしまった場合はidleに移らずにカバーを続行する
			//
			return elapsed_time >= COVER_DURATION && is_angle_within_threshold;
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
		EnemyRVOSystem::CalculateCohesion(mov_dir, owner_enemy->transform.get(), owner_enemy);
		EnemyRVOSystem::ApplyMovement(mov_dir, rb, COHESION_WEIGHT);
	}
	bool EnemyCoverState::CanTransitTo(const std::string& state_name)
	{
		if (state_name == "knock_back" || state_name == "knock_front" || state_name == "damage")
			return true;
		return false;
	}


}