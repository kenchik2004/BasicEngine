//---------------------------------------------------------------------------
//! @file   IStateMachine.h
//! @brief  汎用ステートマシンの基底インタフェースの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h" 
class GameObject;

namespace NeonFade
{
	class IStateMachine
	{
	public:
		virtual ~IStateMachine() = default;
		IStateMachine(GameObject* owner_);


		virtual void ChangeState(std::string_view next);
		const std::string GetCurrentStateName() const { return current_state ? current_state->GetName() : "none"; }
		IState* GetCurrentState() const { return current_state; }
		IState* GetState(std::string_view name) const;
		virtual void Update(float dt);
		void AddState(std::string_view name, SafeUniquePtr<IState> state);
		virtual void OnCollisionEnter(const HitInfo& hit_info);
		virtual void OnCollisionStay(const HitInfo& hit_info);
		virtual void OnCollisionExit(const HitInfo& hit_info);
		virtual void OnTriggerEnter(const HitInfo& hit_info);
		virtual void OnTriggerStay(const HitInfo& hit_info);
		virtual void OnTriggerExit(const HitInfo& hit_info);
		virtual void DebugDraw();
	protected:
		IState* current_state = nullptr;
		GameObject* owner = nullptr;
		std::unordered_map<std::string, SafeUniquePtr<IState>> states;
	};
}

