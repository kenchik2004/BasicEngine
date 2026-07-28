#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade {

	class Player;
	class PlayerSuperHeroLandingState :
		public IState
	{
	public:
		PlayerSuperHeroLandingState(Player* owner_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;

		void OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info) override;

	private:
		Player* owner_player = nullptr;
		RigidBody* rb = nullptr;
		Animator* animator = nullptr;
		SphereColliderWP atk_hit_box = nullptr; // 攻撃判定用のSphereColliderへの弱参照
		GameObjectWP crater = nullptr; // 着地時に生成するクレーターオブジェクトへの弱参照
		float landing_timer = 0.0f;
		static constexpr float LANDING_ANIM_SPEED = 0.5f; // 着地アニメーションの再生速度
		float anim_speed_before_landing = 1.0f; // 着地前のアニメーション再生速度を保持する変数
		static constexpr float LANDING_DURATION = 1.5f; // 着地アニメーションの持続時間

		static constexpr float LANDING_FORCE = 100.0f; // 着地時に加える力の大きさ

		static constexpr float COLIDER_RADIUS = 20.0f; // 攻撃判定用のSphereColliderの半径
		static constexpr u32 DAMAGE = 50;

		static inline const Vector3 CAMERA_SHAKE_INTENSITY = { 0.2f, 1.25f, 0.2f }; // カメラの揺れの強さ
		static constexpr float CAMERA_SHAKE_DURATION = 0.5f; // カメラの揺れの持続時間

		static constexpr float KNOCKBACK_UPWARD_FORCE = 3.0f; // ノックバックの上方向の力の大きさ
		static constexpr float KNOCKBACK_FORCE = 20.0f; // ノックバックの力の大きさ

		void SummonCraterObject();
	};

}

