//---------------------------------------------------------------------------
//! @file   EnemyAttackChargeState.cpp
//! @brief  EnemyAttackChargeStateの実装。敵の攻撃チャージ状態の処理を行う
//---------------------------------------------------------------------------
#include "EnemyAttackChargeState.h"
#include "Game/Objects/NeonFade/Enemy.h"

namespace NeonFade {
	//! @brief 敵の攻撃チャージ状態を構築し、攻撃開始への遷移条件を登録する。
	//! @param owner_ この状態を所有する敵オブジェクト。
	EnemyAttackChargeState::EnemyAttackChargeState(Enemy* owner_)
		:IState(static_cast<GameObject*>(owner_))
	{
		// 状態所有者の敵本体を保持する。
		owner_enemy = owner_;
		// アニメーション制御コンポーネントの参照を取得する。
		animator = owner_enemy->animator.lock().get();
		// 物理移動制御用の剛体コンポーネントを取得する。
		rb = owner_enemy->rb.lock().get();
		// チャージ時間が閾値を超えた時点で攻撃本体へ遷移させる条件式。
		std::function<bool()> charge_to_attack = [this]() {
			return charge_timer >= CHARGE_TIME;
			};
		// 遷移先「attack_main」とチャージ完了条件を状態機械へ登録する。
		RegisterChangeRequest("attack_main", charge_to_attack, 0);
	}
	//! @brief 攻撃チャージ状態に入った瞬間の初期化処理。
	//! @param machine 状態機械本体。
	void EnemyAttackChargeState::OnEnter(IStateMachine* machine)
	{
		// チャージ専用アニメーションを再生して予備動作を開始する。
		animator->Play("enemy_attack_charge");
		// 遷移直後からチャージ時間を再計測するためタイマを初期化する。
		charge_timer = 0.0f;
		// チャージ開始時は水平方向の速度を停止してその場で溜め動作に入る。
		rb->velocity = { 0,0,0 };
	}
	//! @brief 攻撃チャージ状態を抜ける際の終了処理。
	//! @param machine 状態機械本体。
	void EnemyAttackChargeState::OnExit(IStateMachine* machine)
	{
	}
	//! @brief 攻撃チャージ中の向き補正と移動制御を行う更新処理。
	//! @param machine 状態機械本体。
	//! @param dt 前フレームからの経過時間。
	void EnemyAttackChargeState::Update(IStateMachine* machine, float dt)
	{
		// フレーム経過分をチャージタイマへ加算する。
		charge_timer += dt;
		// チャージ前半のみ移動ベクトルに追従させ、後半はその場で溜めを維持する。
		if (charge_timer < 0.6f) {
			// 敵専用状態機械へダウンキャストして入力移動ベクトルを取得する。
			auto enem_machine = static_cast<EnemyStateMachine*>(machine);
			Vector3 mov = enem_machine->move_vec;
			// 地面法線方向の成分を除去し、水平面上の移動成分のみを扱う。
			mov = ProjectOnPlane(mov, { 0,1,0 });
			// 有効な移動ベクトルが存在する場合のみ敵の正面方向を更新する。
			if (mov.isFinite() && !mov.isZero())
				owner_enemy->transform->SetAxisZ(mov);
			// 規定速度以上の入力時は正規化して一定速度で前進させる。
			if (mov.magnitudeSquared() >= 3.0f * 3.0f)
				rb->velocity = mov.getNormalized() * 3.0f;
			// 微小入力時は垂直速度を維持しつつ水平方向のみ停止する。
			else
				rb->velocity = { 0,rb->velocity.y,0 };
		}
	}
}