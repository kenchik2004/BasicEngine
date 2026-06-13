//---------------------------------------------------------------------------
//! @file   PlayerMoveState.h
//! @brief  プレイヤーの移動状態クラスの定義
//---------------------------------------------------------------------------
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
		//! @brief 移動状態を初期化するコンストラクタ。
		//! @param owner_ この状態を所有するプレイヤーオブジェクト。
		PlayerMoveState(Player* owner_);
		//! @brief 状態遷移直後に走行またはダッシュアニメーションを再生する初期化処理。
		//! @param machine ステートマシン本体。
		void OnEnter(IStateMachine* machine) override;
		//! @brief 状態遷移直前に落下タイマをリセットする終了処理。
		//! @param machine ステートマシン本体。
		void OnExit(IStateMachine* machine) override;
		//! @brief 毎フレーム実行する入力処理・向き更新・速度設定処理。
		//! @param machine ステートマシン本体。
		//! @param dt 前フレームからの経過時間。
		void Update(IStateMachine* machine, float dt) override;
		//! @brief コリジョン侵入イベント（移動中は処理なし）。
		void OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info) override {}
		//! @brief 地面接触継続イベントで落下タイマをリセットする。
		//! @param machine ステートマシン本体。
		//! @param hit_info 衝突情報。
		void OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info) override;
		//! @brief コリジョン離脱イベントで落下タイマをリセットする。
		//! @param machine ステートマシン本体。
		//! @param hit_info 衝突情報。
		void OnCollisionExit(IStateMachine* machine, const HitInfo& hit_info) override;
		//! @brief デバッグ用の移動方向ライン描画処理。
		void DebugDraw() override;
	private:
		//! @brief 所有者となるプレイヤーオブジェクト。
		Player* player = nullptr;
		//! @brief 速度設定に使用する剛体。
		RigidBody* rb = nullptr;
		//! @brief 移動モーションを制御するアニメータ。
		Animator* animator = nullptr;
		//! @brief デバッグ描画用の正規化済み入力ベクトル。
		Vector3 input_dbg;
		float fall_timer = 0.0f;	// 落下判定タイマー
		constexpr static float max_fall_time = 0.5f;	//これよりも長く地面に接触していなかったら落下状態へ移行
		//! @brief ダッシュ中かどうかを表すフラグ。
		bool sprint = false;
	};

}
