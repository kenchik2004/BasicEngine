// BasicEnemyBrain.h
//---------------------------------------------------------------------------
//! @file   BasicEnemyBrain.h
//! @brief  基本的な敵AI思考クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/EnemyBrain/AbstractEnemyBrain.h"
namespace NeonFade {

	//! @brief 基本となる敵AIのステートを登録する関数
	//! 待機、ダメージ、ノックバック、死亡などの、どの敵にも共通する基本的なステートを登録する
	//! @param state_machine 登録対象のステートマシン

	class EnemyStateMachine;
	/// @brief 単体で行動する基本的な敵の思考を管理するクラス
	class BasicEnemyBrain :
		public AbstractEnemyBrain
	{
	public:
		/// @brief 基本的な敵AIのコンストラクタ
		/// @param state_machine_ 状態遷移を管理するステートマシン
		/// @param player_ プレイヤーの情報を持つ弱参照ポインタ
		BasicEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_);
		/// @brief 思考処理を実行する
		/// @return 次に遷移すべきステートの文字列
		std::string Think() override;

		/// @brief デバッグ描画
		/// @note 現在はデバッグ描画は行わないため、空実装のままにしておく
		void DebugDraw() override {}

	private:

		/// @brief プレイヤーを発見したかどうかを判定する
		/// @return 発見していればtrueを返す
		bool IsFoundPlayer();
		/// @brief 弱っている他の仲間を援護すべきか判定する
		/// @return 援護対象がいればtrueを返す
		bool IsHaveToCoverOtherEnemy();

	};
}