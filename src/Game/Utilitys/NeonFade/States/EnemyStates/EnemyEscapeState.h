#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade
{
	class Enemy;
	class EnemyEscapeState :
		public IState
	{
	public:
		EnemyEscapeState(Enemy* owner_enemy_);
		void OnEnter(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnExit(IStateMachine* machine) override;
		bool CanTransitTo(const std::string& state_name) override;

	private:
		static constexpr float ESCAPE_DURATION = 10.0f;	//<! 逃走状態の継続時間（秒）
		static constexpr float ESCAPE_SPEED = 5.0f;		//<! 逃走速度（m/s）
		static constexpr float ESCAPE_ROTATION_SPEED = 10.0f;	//<! 逃走時の回転速度（度/秒）
		static constexpr float RETEAMING_DISTANCE_THRESHOLD = 100.0f;	//<! 再編成を開始する距離の閾値（m）

		float distance_to_player_sqr = 0.0f;	//<! プレイヤーまでの距離の二乗値

		Enemy* owner_enemy = nullptr;						//<! オーナーのEnemyオブジェクトへのポインタ
		RigidBody* rb = nullptr;							//<! オーナーのRigidBodyコンポーネントへのポインタ
		Animator* animator = nullptr;						//<! オーナーのAnimatorコンポーネントへのポインタ
	};
}

