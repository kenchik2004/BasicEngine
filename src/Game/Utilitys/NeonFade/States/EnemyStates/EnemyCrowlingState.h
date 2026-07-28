#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade
{
	class Enemy;
	class EnemyCrowlingState :
		public IState
	{
	public:
		EnemyCrowlingState(Enemy* owner_enemy_);
		void OnEnter(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnExit(IStateMachine* machine) override;
		bool CanTransitTo(const std::string& state_name) override;

	private:
		static constexpr float CROWLING_DURATION = 15.0f;	//<! 這いずり状態の継続時間（秒）
		static constexpr float CROWLING_SPEED = 9.5f;		//<! 這いずり状態の移動速度
		static constexpr float CROWLING_ROTATION_SPEED = 1.0f;	//<! 這いずり状態の回転の補間速度
		static constexpr float CROWLING_CONTINUE_RANGE_THRESHOLD = 50.0f;	//<! 這いずり状態を継続するための距離の閾値
		static constexpr std::array<float, 2> APPLY_MOVEMENT_FRAME_INDICES = { 60.0f,150.0f };		//<! アニメーションコールバック(フレーム)が移動に反映されるフレームインデックス

		Enemy* owner_enemy;	//<! オーナーのEnemyオブジェクトへのポインタ
		Animator* animator;	//<! オーナーのAnimatorコンポーネントへのポインタ
		RigidBody* rb;		//<! オーナーのRigidBodyコンポーネントへのポインタ
		CapsuleCollider* col;		//<! オーナーのColliderコンポーネントへのポインタ

		float elapsed_time = 0.0f;	//<! ステート開始後の経過時間を追跡するための変数
		Vector3 movement_direction = { 0,0,0 };	//<! 移動方向を保存する変数



	};

}