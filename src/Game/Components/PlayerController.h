// PlayerController.h
//---------------------------------------------------------------------------
//<! @file   PlayerController.h
//<! @brief  プレイヤーコントローラーコンポーネント（プレイヤー入力処理）
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/StateMachines/PlayerStateMachine.h"

namespace NeonFade
{
	USING_PTR(PlayerStateMachine);
	//---------------------------------------------------------------------
	//<! @class PlayerController
	//<! @brief プレイヤーの入力処理を担うコンポーネント
	//---------------------------------------------------------------------
	class PlayerController :
		public Component
	{
	public:
		USING_SUPER(PlayerController);

		void OnSleep() override;
		void OnWakeUp() override;

		//<! @brief 初期化処理
		//<! @return 初期化の成否
		int Init() override;

		//<! @brief 更新処理
		void Update() override;

		//<! @brief 終了処理
		void Exit() override;

		//<! @brief デバッグ描画（遅延）
		void LateDebugDraw() override;

		//<! @brief ダメージを受ける
		//<! @param damage_ ダメージ量
		void Damage(int damage_);

		//<! @brief 入力を無視するか設定する
		//<! @param ignore_ 無視するかどうか
		void SetIgnoreInput(bool ignore_) { ignore_input = ignore_; }

		//<! @brief 衝突開始イベント
		//<! @param hit_info 衝突情報
		void OnCollisionEnter(const HitInfo& hit_info);
		//<! @brief 衝突継続イベント
		//<! @param hit_info 衝突情報
		void OnCollisionStay(const HitInfo& hit_info);
		//<! @brief 衝突終了イベント
		//<! @param hit_info 衝突情報
		void OnCollisionExit(const HitInfo& hit_info);

		//<! @brief トリガー開始イベント
		//<! @param hit_info 衝突情報
		void OnTriggerEnter(const HitInfo& hit_info);
		//<! @brief トリガー継続イベント
		//<! @param hit_info 衝突情報
		void OnTriggerStay(const HitInfo& hit_info);
		//<! @brief トリガー終了イベント
		//<! @param hit_info 衝突情報
		void OnTriggerExit(const HitInfo& hit_info);

		//<! @brief ステートマシンを取得する
		//<! @return ステートマシンの生ポインタ
		PlayerStateMachine* GetStateMachine() const { return state_machine.get(); }

	private:
		//<! @brief 移動入力の処理
		void ProcessMoveInput();
		//<! @brief 回避入力の処理
		void ProcessDodgeInput();
		//<! @brief 攻撃入力の処理
		void ProcessAttackInput();
		//<! @brief 登攀判定用レイキャストの処理
		void ProcessClimbRaycast();
		//<! @brief 落下および登攀状態の更新
		void UpdateFallingAndClimbingStates();
		//<! @brief ジャンプ入力の処理
		void ProcessJumpInput();
		//<! @brief ステートマシンへの入力伝達処理
		void TransmitInputToStateMachine();

		PlayerStateMachineUP state_machine; //<! プレイヤーのステートマシン
		SafeWeakPtr<Player> owner_player = nullptr; //<! 所有者となるプレイヤー
		bool is_jumping = false; //<! ジャンプ中フラグ
		bool is_landed = false; //<! 着地フラグ
		bool is_falling = false; //<! 落下中フラグ
		bool is_attacking = false; //<! 攻撃中フラグ
		bool is_dodging = false; //<! 回避中フラグ

		bool is_damaged = false; //<! ダメージ受け中フラグ
		float fall_detect_time = 0.0f; //<! 落下検出タイマー
		constexpr static float FALL_DETECT_THRESHOLD = 0.4f; //<! 落下検出閾値
		bool can_climb = false; //<! 登攀可能フラグ
		Vector3 climb_hit_point = { 0, 0, 0 }; //<! 登攀ヒット位置
		Vector3 climb_hit_normal = { 0, 0, 0 }; //<! 登攀ヒット法線
		Vector3 climb_ray_start = { 0, 0, 0 }; //<! 登攀レイ開始位置
		bool is_climbing = false; //<! 登攀中フラグ
		Vector3 move_input = { 0, 0, 0 }; //<! 移動入力ベクトル

		bool ignore_input = false; //<! 入力を無視するか

		static constexpr PadButton JUMP_BUTTON = PadButton::Button1; //<! ジャンプボタン
		static constexpr PadButton NORMAL_ATTACK_BUTTON = PadButton::Button2; //<! 通常攻撃ボタン
		static constexpr PadButton KNOCK_UP_ATTACK_BUTTON = PadButton::Button3; //<! 打ち上げ攻撃ボタン
		static constexpr PadButton SHOOT_ATTACK_BUTTON = PadButton::Button4; //<! 蹴り飛ばし攻撃ボタン
		static constexpr PadButton DODGE_BUTTON = PadButton::LTrigger; //<! 回避ボタン
	};
}