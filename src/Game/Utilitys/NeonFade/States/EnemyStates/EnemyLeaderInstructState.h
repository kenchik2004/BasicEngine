#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade
{

	class Enemy;
	class LeaderEnemyBrain;

	//! @brief 敵のリーダーが仲間に指示を出す状態を管理するクラス
	//! 基本的に、敵のリーダーのみに存在する。自チームのメンバーに対して攻撃指示を行う
	class EnemyLeaderInstructState :
		public IState
	{
	public:
		// @brief EnemyLeaderInstructStateのコンストラクタ
		// @param owner_enemy_ この状態を管理するEnemyオブジェクトへのポインタ
		EnemyLeaderInstructState(Enemy* owner_enemy_, LeaderEnemyBrain* brain);

		// @brief IStateの純粋仮想関数のオーバーライド
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;

		// @brief この状態から遷移可能な状態を制限するための関数
		bool CanTransitTo(const std::string& state_name) override;

	private:
		Enemy* owner_enemy = nullptr;	//<! オーナーのEnemyオブジェクトへのポインタ
		LeaderEnemyBrain* brain = nullptr;	//<! リーダーのAI脳へのポインタ
		GameObject* player = nullptr;		//<! プレイヤーオブジェクトへのポインタ

		Animator* animator;


		float elapsed_time = 0.0f;		//<! ステート開始後の経過時間を追跡するための変数
		static constexpr float INSTRUCT_DURATION = 2.0f;	//<! 指示状態の持続時間（秒）
		static constexpr float INSTRUCT_START = 1.0f;	//<! メンバーに対して指示出すタイミング（秒）

		static constexpr float ROTATION_SPEED = 3.0f;	//<! 指示中の回転の補間速度

		static constexpr u32 INSTRUCT_SIEGE_CHANCE = 40;	//<! 指示中に取り囲みを行う確率（％）

		void InstructMembers();	//<! メンバーに対して指示を出す関数
	};
}
