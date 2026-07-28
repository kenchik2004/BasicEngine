//---------------------------------------------------------------------------
//! @file   PlayerSmashChargeState.h
//! @brief  プレイヤーのスマッシュチャージ状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
class LightManager;
class PointLight;
namespace NeonFade {
	class Player;
	class PlayerSmashChargeState :
		public IState
	{
	public:
		PlayerSmashChargeState(Player* player_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info) override;
		void DebugDraw() override;
	private:
		SafeSharedPtr<Texture> charge_emi_tex = nullptr;
		SafeSharedPtr<Texture> def_emi_tex = nullptr;
		Player* owner_player;
		RigidBody* rb;
		Animator* animator;
		SafeSharedPtr<Collider> enem_finder_col = nullptr;
		SafeWeakPtr<Object> lock_on_target = nullptr;
		float charge_timer = 0.0f;
		SafeWeakPtr<GameObject> charge_effect = nullptr;
		LightManager* light_manager = nullptr;
		std::vector<SafeSharedPtr<PointLight>> smash_lights;
		static constexpr float MAX_CHARGE_TIME = 1.0f;
		SafeSharedPtr<AudioClip> charge_se;
		static constexpr int LIGHT_BIND_INDEX = 37;
		bool camera_cinema_mode = false;
		Vector3 target_vec = { 0,0,1 };

		//スマッシュ攻撃のチャージ中は、攻撃方向とカメラの向きがあまりに乖離していると不自然なので、ある程度カメラの向きに近づけるようにする。
		//また、この値を越えている場合はスマッシュをスーパーチャクチみたいな攻撃に変える。
		static constexpr float ANGLE_THRESHOLD = 45.0f;
		static inline const float COS_ANGLE_THRESHOLD = cosf(DEG2RAD(ANGLE_THRESHOLD));

		static constexpr float ENEMY_FIND_RANGE = 50.0f;
	};
}

