#include "EnemyStandUpFrontState.h"
#include "Game/Objects/NeonFade/Enemy.h"

namespace NeonFade {
	EnemyStandUpFrontState::EnemyStandUpFrontState(Enemy* owner_enemy_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		owner_enemy = owner_enemy_;
		animator = owner_enemy->animator.lock().get();
		rb = owner_enemy->rb.lock().get();
		col = owner_enemy->col.lock().get();
		std::function<bool()> default_exit = [this]()
			{
				return elapsed_time >= STAND_UP_DURATION;
			};
		RegisterChangeRequest("idle", default_exit);
	}
	void EnemyStandUpFrontState::OnEnter(IStateMachine* machine)
	{
		elapsed_time = 0.0f;
		animator->Play("enemy_standup_forward");
	}
	void EnemyStandUpFrontState::Update(IStateMachine* machine, float dt)
	{
		elapsed_time += dt;
		RotateCollider();
		TranslateCollider();
	}
	void EnemyStandUpFrontState::OnExit(IStateMachine* machine)
	{

		rb->freeze_position = { false, false, false }; // コライダーの回転が終わったら位置の固定を解除する
		static const Quaternion target_collider_rotation = Quaternion(DEG2RAD(90.0f), { 0, 0, -1 });
		col->rotation = target_collider_rotation; // コライダーの回転を最終的な状態にリセットする
		static const Vector3 target_collider_position = Vector3(0, 0, 0); // コライダーの最終的な位置
		col->position = target_collider_position; // コライダーの位置を最終的な状態にリセットする


	}
	bool EnemyStandUpFrontState::CanTransitTo(const std::string& state_name)
	{
		if (state_name == "knock_back" || state_name == "knock_front")
			return true;
		return false;
	}
	void EnemyStandUpFrontState::RotateCollider()
	{
		static constexpr float ROTATION_DURATION = 0.8f; // コライダーの回転にかける時間（秒）
		static constexpr float ROTATION_START = 2.7f;		//<! 回転を開始するタイミング（秒）
		//! コライダーの初期回転の定数クォータニオン
		static const Quaternion initial_collider_rotation = Quaternion(DEG2RAD(90.0f), { 0, 0, -1 }) * Quaternion(DEG2RAD(90.0f), { 0, 1, 0 });	
		Quaternion target_rotation = Quaternion(DEG2RAD(90.0f), { 0, 0, -1 }); // コライダーを90度回転させるクォータニオン

		float t = std::clamp((elapsed_time - ROTATION_START) / ROTATION_DURATION, 0.0f, 1.0f);		// 経過時間に応じて回転を補間するためのパラメーター
		col->rotation = Slerp(initial_collider_rotation, target_rotation, t); // コライダーの回転を補間して更新する
		if (t > 0.0f && t < 1.0f)
			rb->freeze_position = { true, false, true }; // コライダーの回転中は位置を固定する
	}
	void EnemyStandUpFrontState::TranslateCollider()
	{
		static constexpr float TRANSLATE_DURATION = 0.8f; // コライダーの移動にかける時間（秒）
		static constexpr float TRANSLATE_START = 1.7f;		//<! 移動を開始するタイミング（秒）
		static const Vector3 initial_collider_position = Vector3(0.0f, 0.0f, -1.0f); // コライダーの初期位置の定数ベクトル
		static const Vector3 target_collider_position = Vector3(0, 0, 0); // コライダーの最終的な位置
		float t = std::clamp((elapsed_time - TRANSLATE_START) / TRANSLATE_DURATION, 0.0f, 1.0f);		// 経過時間に応じて移動を補間するためのパラメーター
		col->position = Lerp(initial_collider_position, target_collider_position, t); // コライダーの位置を補間して更新する
	}
}