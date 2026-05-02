//---------------------------------------------------------------------------
//! @file   PlayerController.h
//! @brief  プレイヤーコントローラーコンポーネント（プレイヤー入力処理）
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/StateMachines/PlayerStateMachine.h"
namespace NeonFade
{
	USING_PTR(PlayerStateMachine);
	//---------------------------------------------------------------------
	//! @class PlayerController
	//! @brief プレイヤーの入力処理を担うコンポーネント
	//---------------------------------------------------------------------
	class PlayerController :
		public Component
	{
	public:
		USING_SUPER(PlayerController);
		//! @brief 初期化処理
		int Init() override;
		//! @brief 更新処理
		void Update() override;
		//! @brief 終了処理
		void Exit() override;
		//! @brief デバッグ描画（遅延）
		void LateDebugDraw() override;
		//! @brief 物理演算前処理
		void PrePhysics() override;

		//! @brief ダメージを受ける
		void Damage(int damage);

		//! @brief 衝突開始イベント
		void OnCollisionEnter(const HitInfo& hit_info);
		//! @brief 衝突継続イベント
		void OnCollisionStay(const HitInfo& hit_info);
		//! @brief 衝突終了イベント
		void OnCollisionExit(const HitInfo& hit_info);
		//! @brief トリガー開始イベント
		void OnTriggerEnter(const HitInfo& hit_info);
		//! @brief トリガー継続イベント
		void OnTriggerStay(const HitInfo& hit_info);
		//! @brief トリガー終了イベント
		void OnTriggerExit(const HitInfo& hit_info);
		//! @brief ステートマシンを取得する
		PlayerStateMachine* GetStateMachine() const { return state_machine.get(); }

	private:
		PlayerStateMachineUP state_machine; //!< プレイヤーのステートマシン
		SafeSharedPtr<Player> owner_player = nullptr; //!< オーナープレイヤー
		bool is_jumping = false; //!< ジャンプ中フラグ
		bool is_landed = false; //!< 着地フラグ
		bool is_falling = false; //!< 落下中フラグ
		bool is_attacking = false; //!< 攻撃中フラグ
		bool is_dodging = false; //!< 回避中フラグ

		bool is_damaged = false; //!< ダメージ受け中フラグ
		float fall_detect_time = 0.0f; //!< 落下検出タイマー
		constexpr static float FALL_DETECT_THRESHOLD = 0.4f; //!< 落下検出閾値
		bool can_climb = false; //!< 登攀可能フラグ
		Vector3 climb_hit_point = { 0,0,0 }; //!< 登攀ヒット位置
		Vector3 climb_hit_normal = { 0,0,0 }; //!< 登攀ヒット法線
		Vector3 climb_ray_start = { 0,0,0 }; //!< 登攀レイ開始位置
		bool is_climbing = false; //!< 登攀中フラグ
		Vector3 move_input = { 0,0,0 }; //!< 移動入力ベクトル
	};
}
