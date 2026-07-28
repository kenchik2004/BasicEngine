#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade
{

	class Enemy;

	//! @brief プレイヤーの方向を向くだけのステートクラス
	//! @brief 敵がプレイヤーの方向を向く行動を行う際の状態を表す
	class EnemyFocusToPlayerState :
		public IState
	{
	public:

		//! @brief コンストラクタ
		//! @param owner_enemy_ 所有者となるEnemyオブジェクトへのポインタ
		EnemyFocusToPlayerState(Enemy* owner_enemy_);

		// 親クラスの純粋仮想関数のオーバーライド
		void OnEnter(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnExit(IStateMachine* machine) override;
		
		//! @brief 遷移可能かどうかを判定
		bool CanTransitTo(const std::string& state_name) override;

	private:
		Enemy* owner_enemy = nullptr;							// 所有者となるEnemyオブジェクトへのポインタ
		Transform* owner_transform = nullptr;					// 所有者のTransformへのポインタ
		GameObject* player = nullptr;						// プレイヤーへのポインタ
		Animator* animator = nullptr;					// 所有者のAnimatorへのポインタ

		static constexpr float ROTATION_SPEED = 10.0f; // プレイヤーの方向を向く際の回転速度

		static constexpr float MAX_FOCUS_DURATION = 3.0f; // プレイヤーの方向を向く状態の最大持続時間
		float focus_timer = 0.0f; // プレイヤーの方向を向く状態の経過時間を保持する変数

		static constexpr float FOCUS_THRESHOLD_ANGLE = 5.0f; // プレイヤーの方向を向いたとみなす角度の閾値（度数法）
		float cosine_to_player = 0.0f; // プレイヤーの方向を向く際の内積値(cosθ)を保持する変数

	};
}
