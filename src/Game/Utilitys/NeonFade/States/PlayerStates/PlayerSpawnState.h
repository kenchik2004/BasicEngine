#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade
{

	class Player;
	class PlayerSpawnState :
		public IState
	{
	public:
		PlayerSpawnState(Player* owner_player_);
		void OnEnter(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnExit(IStateMachine* machine) override;

	private:
		static constexpr float SPAWN_DURATION = 2.5f;	//<! スポーン状態の継続時間（秒）
		float elapsed_time = 0.0f;						//<! ステート開始後の経過時間を追跡するための変数
		Player* owner_player = nullptr;					//<! オーナーのPlayerオブジェクトへのポインタ
		Animator* animator = nullptr;						//<! オーナーのAnimatorコンポーネントへのポインタ
		GameObjectWP trail_effect = nullptr;						//<! スポーン時のトレイルエフェクトオブジェクトへの弱参照


	};

}