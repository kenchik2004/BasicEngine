#include "EnemyRandomWalkState.h"
#include "Game/Objects/NeonFade/Enemy.h"

namespace NeonFade
{
	EnemyRandomWalkState::EnemyRandomWalkState(Enemy* owner_enemy_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		owner_enemy = owner_enemy_;
		animator = owner_enemy->animator.lock().get();
		rb = owner_enemy->rb.lock().get();

		std::function<bool()> default_exit = [this]() {
			// 目的地に近づく or 一定時間経過したら状態遷移を要求する
			static constexpr float MIN_DISTANCE_SQUARED = 5.0f * 5.0f; // 目的地に近いとみなす距離の二乗
			return (owner_enemy->transform->position - next_destination).magnitudeSquared() < MIN_DISTANCE_SQUARED || elapsed_time >= MAX_WALK_TIME;
			};
		RegisterChangeRequest("knock_back", default_exit);
	}
	void EnemyRandomWalkState::OnEnter(IStateMachine* machine)
	{
		animator->Play("enemy_walk", true);
		next_destination = Random::Position({ -100.0f, 0.0f, -100.0f }, { 100.0f, 0.0f, 100.0f }); // ランダムな目的地を生成
		next_destination += owner_enemy->transform->position; // 現在の位置を基準に目的地を設定

		elapsed_time = 0.0f; // 経過時間をリセット

	}
	void EnemyRandomWalkState::Update(IStateMachine* machine, float dt)
	{
		elapsed_time += dt;
		Vector3 forward = owner_enemy->transform->AxisZ();
		Vector3 to_dest = next_destination - owner_enemy->transform->position;
		to_dest = Slerp(forward, to_dest, 0.05f).getNormalized(); // 前方ベクトルと目的地へのベクトルを補間して滑らかに方向転換する

		to_dest *= walk_speed;
		to_dest.y = 0; // Y軸の移動を無効にする（地面に沿って移動するため）

		owner_enemy->transform->SetAxisZ(to_dest); // 目的地への方向を向く
		to_dest.y = rb->velocity.y; // 現在のY軸の速度を保持

		rb->velocity = to_dest;
	}
	void EnemyRandomWalkState::OnExit(IStateMachine* machine)
	{
		rb->velocity = { 0, rb->velocity.y, 0 }; // 状態を抜けるときに速度をリセットする
	}

	bool EnemyRandomWalkState::CanTransitTo(const std::string& state_name)
	{
		//自身への遷移は許可しない
		if (state_name == "random_walk")
			return false;
		return true;
	}

}