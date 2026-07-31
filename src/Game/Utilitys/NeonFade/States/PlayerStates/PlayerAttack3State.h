//---------------------------------------------------------------------------
//! @file   PlayerAttack3State.h
//! @brief  プレイヤーの第3攻撃状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade {
	class Player;
	class PlayerAttack3State :
		public IState
	{
	public:
		PlayerAttack3State(Player* player_);
		void OnEnter(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnExit(IStateMachine* machine) override;
		void OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info)override;
		static constexpr float EXIT_TIME = 0.6f;
		float exit_timer = 0.0f;
		float hit_stop_timer = 0.0f;
		static constexpr float HIT_STOP_TIME = 0.1f;
		static constexpr float CAMERA_SHAKE_TIME = 0.2f;
		static inline const Vector3 CAMERA_SHAKE_INTENSITY = { 0.3f, 0.3f, 0.3f };

		static constexpr float KNOCK_BACK_FORCE = 40.0f;
		static constexpr float KNOCK_BACK_UP_FORCE = 10.0f;
		static constexpr u32 DAMAGE = 15;

		int stop_counter = 0;
		static constexpr int MAX_STOP_COUNT = 3;
	private:
		float hit_box_created_time = 0.0f;
		Player* owner_player = nullptr;
		Animator* animator = nullptr;
		RigidBody* rb = nullptr;
		SafeSharedPtr<Collider> hit_box = nullptr;
	};
}

