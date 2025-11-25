#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

class RigidBody;
class Animator;
namespace NeonFade
{
	class Player;
	class PlayerMoveState :
		public IState
	{
	public:
		PlayerMoveState(Player* owner_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info) override {}
		void OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info) override;
		void OnCollisionExit(IStateMachine* machine, const HitInfo& hit_info) override;
		void DebugDraw() override;
	private:
		Player* player = nullptr;
		RigidBody* rb = nullptr;
		Animator* animator = nullptr;
		Vector3 input_dbg;
		float fall_timer = 0.0f;	// 落下判定タイマー
		constexpr static float max_fall_time = 0.5f;	//これよりも長く地面に接触していなかったら落下状態へ移行
		bool sprint = false;
	};

}
