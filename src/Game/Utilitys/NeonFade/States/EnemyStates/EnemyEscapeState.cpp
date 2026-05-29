//---------------------------------------------------------------------------
//! @file   EnemyEscapeState.cpp
//! @brief  EnemyEscapeStateの実装。敵の逃走状態の処理を行う
//---------------------------------------------------------------------------
#include "EnemyEscapeState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/Player.h"

namespace NeonFade {
	//! @brief 逃走状態を構築し、逃走終了のアイドル遷移条件を登録する。
	//! @param owner_ この状態を所有する敵オブジェクト。
	//! @param leder_ 追従先リーダーオブジェクトの弱参照。
	EnemyEscapeState::EnemyEscapeState(Enemy* owner_, SafeWeakPtr<Object> leder_) :
		IState(static_cast<GameObject*>(owner_))
	{
		// 状態所有者の敵本体を保持する。
		enemy = owner_;
		// 逃走タイマを最大値で初期化する。
		escape_timer = ESCAPE_TIME;
		// 移動速度制御に使用する剛体を取得する。
		rb = enemy->rb.lock().get();
		// 逃走モーション再生に使用するアニメータを取得する。
		animator = enemy->animator.lock().get();
		// 追跡・回避の基準となるプレイヤーオブジェクトを取得する。
		player = SceneManager::Object::Get<Player>().get();
		// リーダーへの弱参照を保持する。
		leader = leder_;
		// exit_flag が真になった時点でアイドルへ遷移させる条件式。
		std::function escape_end = [this]() {
			return exit_flag;
			};
		// 遷移先「idle」と逃走終了条件を優先度 0 で登録する。
		RegisterChangeRequest("idle", escape_end, 0);

	}

	//! @brief 逃走状態へ入った瞬間の初期化処理。
	//! @param machine 状態機械本体。
	void EnemyEscapeState::OnEnter(IStateMachine* machine)
	{
		// 逃走タイマを最大値でリセットして逃走を再開始する。
		escape_timer = ESCAPE_TIME;
		// 逃走モーションをランダムな開始位相でループ再生する。
		animator->Play("enemy_escape", true, Random::Float01(), 0.2f, false);
		// 前回の終了フラグをリセットする。
		exit_flag = false;
	}

	//! @brief 逃走状態を抜ける際の終了処理（現在は空実装）。
	//! @param machine 状態機械本体。
	void EnemyEscapeState::OnExit(IStateMachine* machine)
	{
	}

	//! @brief リーダーまたはプレイヤーを基準に逃走移動を行い、タイムアップで終了する更新処理。
	//! @param machine 状態機械本体。
	//! @param dt 前フレームからの経過時間。
	void EnemyEscapeState::Update(IStateMachine* machine, float dt)
	{
		// 残り逃走時間を経過時間分だけ減算する。
		escape_timer -= dt;
		// 現在の正面方向を逃走ベクトルの初期値として使用する。
		Vector3 mov = enemy->transform->AxisZ();
		if (leader)
		{
			// リーダーが存在する場合はリーダー方向へ向かう成分を混合する。
			Vector3 to_leader = leader->transform->position - enemy->transform->position;
			// 垂直成分を除去して水平面上の移動のみ扱う。
			mov.y = 0;
			// リーダーへ向かう方向へ 20% の重みで補間する。
			mov = Lerp(mov, to_leader.getNormalized(), 0.2f);
			// リーダーに近すぎる場合は逆方向に押し返して密集を防ぐ。
			if (to_leader.magnitude() < 5.0f)
				mov = Lerp(mov, -to_leader.getNormalized(), 0.3f);
		}
		else {
			// リーダーが存在しない場合はプレイヤーから遠ざかる方向へ逃げる。
			Vector3 to_player = player->transform->position - enemy->transform->position;
			// 垂直成分を除去して水平面上の移動のみ扱う。
			mov.y = 0;
			// プレイヤーの逆方向へ 10% の重みで補間して方向を更新する。
			mov = Lerp(mov, -to_player.getNormalized(), 0.1f);

		}
		// 速度ベクトルを正規化して一定速度で移動できるようにする。
		mov.normalize();
		// 有効な移動方向がある場合のみ敵の正面方向を更新する。
		if (!mov.isZero())
			enemy->transform->SetAxisZ(mov);
		// 移動速度を 10 に設定する。
		mov *= 10;
		// 垂直速度は物理計算の結果を維持する。
		mov.y = rb->velocity.y;
		rb->velocity = mov;
		if (escape_timer <= 0.0f)
		{
			exit_flag = true;	// タイムアップで逃走終了
		}

	}

}