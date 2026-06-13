//---------------------------------------------------------------------------
//! @file   EnemyLeaderAttackState.cpp
//! @brief  EnemyLeaderAttackStateの実装。リーダー敵の攻撃状態の処理を行う
//---------------------------------------------------------------------------
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyLeaderAttackState.h"
#include "Game/Objects/NeonFade/Enemy.h"

namespace NeonFade {
	//! @brief リーダー攻撃状態を構築し、指定時間後にアイドルへ戻る遷移条件を登録する。
	//! @param owner_ この状態を所有する敵オブジェクト。
	//! @param player_ ターゲットとなるプレイヤーオブジェクトの弱参照。
	EnemyLeaderAttackState::EnemyLeaderAttackState(Enemy* owner_,GameObjectWP player_)
		:IState(static_cast<GameObject*>(owner_))
	{
		// 攻撃タイマが上限を超えたらアイドルへ遷移させる条件式。
		std::function<bool()> attack_to_idle = [state = this]() {
			return state->attack_timer >= state->ATTACK_TIME;
			};
		// 遷移先「idle」と時間終了条件を優先度 1 で登録する。
		RegisterChangeRequest("idle", attack_to_idle, 1);
		// 状態所有者の敵本体を保持する。
		enemy = owner_;
		// ジャンプ力付与に使用する剛体を取得する。
		rb = enemy->rb.lock().get();
		// 扇動モーション再生に使用するアニメータを取得する。
		animator = enemy->animator.lock().get();
		// ターゲットとなるプレイヤーの弱参照を保持する。
		player = player_;
	}
	//! @brief リーダー攻撃状態へ入った瞬間の初期化処理。
	//! @param machine ステートマシン本体。
	void EnemyLeaderAttackState::OnEnter(IStateMachine* machine)
	{
		// 攻撃タイマを初期化して扇動時間の計測を開始する。
		attack_timer = 0.0f;
		// 扇動モーションをループなしで再生して指示動作を表現する。
		animator->Play("enemy_instruct", true, 0.0f, 0.2f, false);
	}
	//! @brief リーダー攻撃状態を抜ける際の終了処理（現在は空実装）。
	//! @param machine ステートマシン本体。
	void EnemyLeaderAttackState::OnExit(IStateMachine* machine)
	{
	}
	//! @brief プレイヤーへの向き更新と定期ジャンプによる扇動表現を行う更新処理。
	//! @param machine ステートマシン本体。
	//! @param dt 前フレームからの経過時間。
	void EnemyLeaderAttackState::Update(IStateMachine* machine, float dt)
	{
		// 攻撃経過時間を加算する。
		attack_timer += dt;
		if (attack_timer >= ATTACK_TIME) {
			//攻撃終了
			// 攻撃終了判定後にタイマをリセットする（遷移条件は RegisterChangeRequest で処理）。
			attack_timer = 0.0f;
		}
		//プレイヤーの方を向き、手を振って部下を扇動する(プレイヤーを煽るイメージ)
		// プレイヤーの現在位置を取得する。
		Vector3 player_pos = player.lock()->transform->position;
		// 自身からプレイヤーへのベクトルを算出する。
		Vector3 to_player = player_pos - enemy->transform->position;
		// 垂直成分を除去して水平面上での向き変更のみ行う。
		to_player.y = 0;
		// 十分な方向ベクトルがある場合のみ正面方向をプレイヤーへ向ける。
		if (to_player.magnitudeSquared() > 0.001f)
			enemy->transform->SetAxisZ(to_player);


		//0.5秒ごとにジャンプして扇動してる感を出す
		// 0.5 秒周期でジャンプ力を加え、煽りリアクションを表現する。
		if (fmodf(attack_timer, 0.5f) <= dt) {
			rb->AddForce(Vector3(0, 10, 0), ForceMode::VelocityCange);
		}
	}
}