#include "EnemyStandUpBackState.h"
#include "Game/Objects/NeonFade/Enemy.h"

namespace NeonFade {
	EnemyStandUpBackState::EnemyStandUpBackState(Enemy* owner_enemy_)
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
	void EnemyStandUpBackState::OnEnter(IStateMachine* machine)
	{
		elapsed_time = 0.0f;
		animator->Play("enemy_standup_back");
	}
	void EnemyStandUpBackState::Update(IStateMachine* machine, float dt)
	{
		elapsed_time += dt;
		RotateCollider();
		ScaleCollider();
		TranslateCollider();
	}
	void EnemyStandUpBackState::OnExit(IStateMachine* machine)
	{
		static const Quaternion target_collider_rotation = Quaternion(DEG2RAD(90.0f), { 0, 0, -1 });
		col->rotation = target_collider_rotation; // コライダーの回転を最終的な状態にリセットする
		static constexpr float target_collider_height = 5.7f; // コライダーの最終的な高さ	
		col->height = target_collider_height; // コライダーの高さを最終的な状態にリセットする
		static const Vector3 target_collider_position = Vector3(0, 0, 0); // コライダーの最終的な位置
		col->position = target_collider_position; // コライダーの位置を最終的な状態にリセットする
		rb->freeze_position = { false, false, false }; // コライダーの回転とスケーリングが終わったら位置の固定を解除する

	}
	bool EnemyStandUpBackState::CanTransitTo(const std::string& state_name)
	{
		if (state_name == "knock_back" || state_name == "knock_front")
			return true;
		return false;
	}
	void EnemyStandUpBackState::RotateCollider()
	{
		static constexpr float ROTATION_DURATION = 0.8f; // コライダーの回転にかける時間（秒）
		static constexpr float ROTATION_START = 0.7f;		//<! 回転を開始するタイミング（秒）
		static const Quaternion initial_collider_rotation = Quaternion(DEG2RAD(90.0f), { 0, 0, -1 }) * Quaternion(DEG2RAD(-90.0f), { 0, 1, 0 });				//<! コライダーの初期回転の定数クォータニオン
		Quaternion target_rotation = Quaternion(DEG2RAD(90.0f), { 0, 0, -1 }); // コライダーを90度回転させるクォータニオン

		float t = std::clamp((elapsed_time - ROTATION_START) / ROTATION_DURATION, 0.0f, 1.0f);		// 経過時間に応じて回転を補間するためのパラメーター
		col->rotation = Slerp(initial_collider_rotation, target_rotation, t); // コライダーの回転を補間して更新する
		if (t > 0.0f && t < 1.0f)
			rb->freeze_position = { true, false, true }; // コライダーの回転中は位置を固定する
	}
	void EnemyStandUpBackState::ScaleCollider()
	{
		static constexpr float SCALE_DURATION = 0.8f; // コライダーのスケーリングにかける時間（秒）
		static constexpr float SCALE_START = 1.5f;		//<! スケーリングを開始するタイミング（秒）
		static constexpr float initial_collider_height = 1.0f; // コライダーの初期の高さ
		static constexpr float target_collider_height = 5.7f; // コライダーの最終的な高さ
		float t = std::clamp((elapsed_time - SCALE_START) / SCALE_DURATION, 0.0f, 1.0f);		// 経過時間に応じてスケーリングを補間するためのパラメーター
		col->height = std::lerp(initial_collider_height, target_collider_height, t); // コライダーの高さを補間して更新する
	}
	void EnemyStandUpBackState::TranslateCollider()
	{
		static constexpr float TRANSLATE_DURATION = 0.8f; // コライダーの移動にかける時間（秒）
		static constexpr float TRANSLATE_START = 1.3f;		//<! 移動を開始するタイミング（秒）
		static const Vector3 initial_collider_position = Vector3(-1.5f, 0.0f, 0.0f); // コライダーの初期位置
		static const Vector3 target_collider_position = Vector3(0, 0, 0); // コライダーの最終的な位置
		float t = std::clamp((elapsed_time - TRANSLATE_START) / TRANSLATE_DURATION, 0.0f, 1.0f);		// 経過時間に応じて移動を補間するためのパラメーター
		col->position = Lerp(initial_collider_position, target_collider_position, t); // コライダーの位置を補間して更新する

	}
}



