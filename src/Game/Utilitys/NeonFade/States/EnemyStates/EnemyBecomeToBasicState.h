#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade {

	class Enemy;
	USING_PTR(Player);

	//! @brief 敵の脳を単独行動用のものに切り替えるステート
	class EnemyBecomeToBasicState :
		public IState
	{
	public:
		//! @brief コンストラクタ
		//! @param owner_enemy_ このステートを所有する敵のポインタ
		EnemyBecomeToBasicState(Enemy* owner_enemy_);

		// IStateの純粋仮想関数のオーバーライド
		void OnExit(IStateMachine* machine) override;

		//更新や開始処理は特に必要ないため、空実装とする
		void OnEnter(IStateMachine* machine) override {}
		void Update(IStateMachine* machine, float dt) override {}

		bool CanTransitTo(const std::string& state_name) override;
	private:
		Enemy* owner_enemy;
		PlayerWP player;

		ModelRenderer* model; //!< モデルレンダラー

	};

	//! @brief 敵の脳をリーダー用のものに切り替えるステート
	class EnemyBecomeToLeaderState :
		public IState
	{
	public:
		//! @brief コンストラクタ
		//! @param owner_enemy_ このステートを所有する敵のポインタ
		EnemyBecomeToLeaderState(Enemy* owner_enemy_);

		// IStateの純粋仮想関数のオーバーライド
		void OnExit(IStateMachine* machine) override;

		//更新や開始処理は特に必要ないため、空実装とする
		void OnEnter(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;

		bool CanTransitTo(const std::string& state_name) override;
	private:
		Enemy* owner_enemy;
		PlayerWP player;

		ModelRenderer* model; //!< モデルレンダラー
		Animator* animator; //!< アニメーター
		float timer = 0.0f;
		static constexpr float BECOME_TO_LEADER_TIME = 1.0f; // 脳を切り替えるまでの時間

	};
}

