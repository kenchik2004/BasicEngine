#include "EnemyKnockBackState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/AbstractEnemyBrain.h"


namespace NeonFade {
	EnemyKnockBackState::EnemyKnockBackState(Enemy* owner_enemy_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		owner_enemy = owner_enemy_;
		animator = owner_enemy->animator.lock().get();
		rb = owner_enemy->rb.lock().get();
		col = owner_enemy->col.lock().get();

		std::function <bool()> die_exit = [this]() {
			return elapsed_time >= KNOCK_BACK_DURATION && owner_enemy->IsDead();
			};
		RegisterChangeRequest("die", die_exit, 0);

		std::function<bool()> default_exit = [this]() {
			return elapsed_time >= KNOCK_BACK_DURATION;
			};
		RegisterChangeRequest("stand_up_back", default_exit, 1);
	}
	void EnemyKnockBackState::OnEnter(IStateMachine* machine)
	{
		elapsed_time = 0.0f;
		animator->Play("enemy_down");
		auto enem_state_machine = dynamic_cast<EnemyStateMachine*>(machine);
		if (!enem_state_machine || !enem_state_machine->GetBrain())
			return;

		Vector3 knock_back_dir = enem_state_machine->GetBrain()->GetKnockBackVec();
		rb->velocity = knock_back_dir; // ノックバックベクトルを速度に直接設定する
		knock_back_dir.y = 0; // 水平方向のノックバックに限定する
		owner_enemy->transform->SetAxisZ(-knock_back_dir.getNormalized()); // ノックバックの方向に背を向けるように回転させる

	}
	void EnemyKnockBackState::OnExit(IStateMachine* machine)
	{}
	void EnemyKnockBackState::Update(IStateMachine* machine, float dt)
	{
		elapsed_time += dt;
		RotateCollider();
	}
	bool EnemyKnockBackState::CanTransitTo(const std::string& state_name)
	{
		if (state_name == "knock_back" || state_name == "knock_front")
			return true;
		return false;
	}
	void EnemyKnockBackState::RotateCollider()
	{
		static constexpr float ROTATION_DURATION = 0.3f; // コライダーの回転にかける時間（秒）
		static constexpr float ROTATION_START = 0.5f;		//<! ヒットストップを開始するタイミング（秒）
		static const Quaternion initial_collider_rotation = Quaternion(DEG2RAD(90.0f), { 0, 0, -1 });				//<! コライダーの初期回転の定数クォータニオン
		Quaternion target_rotation = initial_collider_rotation * Quaternion(DEG2RAD(-90.0f), { 0, 1, 0 }); // コライダーを-90度回転させるクォータニオン

		float t = std::clamp((elapsed_time - ROTATION_START) / ROTATION_DURATION, 0.0f, 1.0f);		// 経過時間に応じて回転を補間するためのパラメーター
		col->rotation = Slerp(initial_collider_rotation, target_rotation, t); // コライダーの回転を補間して更新する
	}

}