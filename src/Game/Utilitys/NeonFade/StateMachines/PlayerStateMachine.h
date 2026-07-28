// PlayerStateMachine.h
//---------------------------------------------------------------------------
//! @file   PlayerStateMachine.h
//! @brief  プレイヤー用ステートマシンクラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/StateMachines/IStateMachine.h"

namespace NeonFade
{
	class Player;
	/// @brief プレイヤーの各種状態遷移を管理するステートマシンクラス
	class PlayerStateMachine :
		public IStateMachine
	{
	public:
		/// @brief プレイヤーステートマシンのコンストラクタ
		/// @param owner_ 所有者となるプレイヤーオブジェクトのポインタ
		PlayerStateMachine(Player* owner_);
		/// @brief デストラクタ
		virtual ~PlayerStateMachine();
		/// @brief デバッグ用の描画処理を行う
		void DebugDraw() override;
		/// @brief 毎フレームの更新処理を行う
		/// @param dt 経過時間
		void Update(float dt) override;

	public:
		Player* player = nullptr; /// @brief 所有者であるプレイヤーオブジェクトへのポインタ
		bool is_jumping = false; /// @brief ジャンプ中かどうかを示すフラグ
		bool is_falling = false; /// @brief 落下中かどうかを示すフラグ
		bool is_landed = false; /// @brief 着地しているかどうかを示すフラグ
		bool is_attacking = false; /// @brief 攻撃中かどうかを示すフラグ
		u32 attack_index_prev = 0; /// @brief 前フレームの攻撃インデックス
		std::array<SafeUniquePtr<IState>, 4> attack_states_vec; /// @brief 攻撃用ステートを保持する配列
		bool is_dodging = false; /// @brief 回避行動中かどうかを示すフラグ
		bool is_damaged = false; /// @brief ダメージを受けているかどうかを示すフラグ
		Vector3 move_input = { 0,0,0 }; /// @brief プレイヤーの移動入力ベクトル
		bool can_climb = false; /// @brief 壁を登れる状態かどうかを示すフラグ
		Vector3 climb_start_pos; /// @brief 壁登りの開始座標
		Vector3 climb_end_pos; /// @brief 壁登りの終了座標

	};
}