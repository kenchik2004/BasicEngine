//---------------------------------------------------------------------------
//! @file   EnemyLeaderStepBackState.cpp
//! @brief  EnemyLeaderStepBackStateの実装。リーダー敵の後退状態の処理を行う
//---------------------------------------------------------------------------
#include "EnemyLeaderStepBackState.h"
#include "Game/Objects/NeonFade/Enemy.h"

namespace NeonFade {
	//! @brief リーダー後退状態を構築し、指定時間後にアイドルへ戻る遷移条件を登録する。
	//! @param owner_ この状態を所有する敵オブジェクト。
	//! @param player_ 向きを合わせる対象となるプレイヤーオブジェクトの弱参照。
	EnemyLeaderStepBackState::EnemyLeaderStepBackState(Enemy* owner_, GameObjectWP player_)
		:IState(static_cast<GameObject*>(owner_))
	{
		// 後退タイマが上限を超えたらアイドルへ遷移させる条件式。
		std::function<bool()> step_back_to_idle = [state = this]() {
			return state->step_back_timer >= state->STEP_BACK_TIME;
			};
		// 遷移先「idle」と時間終了条件を優先度 1 で登録する。
		RegisterChangeRequest("idle", step_back_to_idle, 1);
		// 状態所有者の敵本体を保持する。
		enemy = owner_;
		// ジャンプ力付与に使用する剛体を取得する。
		rb = enemy->rb.lock().get();
		// 後退モーション再生に使用するアニメータを取得する。
		animator = enemy->animator.lock().get();
		// ターゲットとなるプレイヤーの弱参照を保持する。
		player = player_;
	}
	//! @brief リーダー後退状態へ入った瞬間の初期化処理。
	//! @param machine ステートマシン本体。
	void EnemyLeaderStepBackState::OnEnter(IStateMachine* machine)
	{
		// 後退タイマを初期化して後退時間の計測を開始する。
		step_back_timer = 0.0f;
		// 後退モーションを再生して後退動作を表現する。
		animator->Play("enemy_stepback", true, 0.0f, 0.05f, false);
		if (player) {
			// プレイヤー方向への水平ベクトルを算出する。
			Vector3 to_player = (player.lock()->transform->position - enemy->transform->position).getNormalized();
			// 垂直成分を除去して水平面上の向き変更のみ行う。
			to_player.y = 0.0f;
			// 十分な方向ベクトルがある場合のみ向きを正規化してプレイヤーへ向ける。
			if (to_player.magnitudeSquared() > 1e-6f) {
				to_player.normalize();
				enemy->transform->SetAxisZ(to_player);
			}

		}
		// ジャンプ済みフラグを未実行状態にリセットする。
		jumped = false;
	}
	//! @brief リーダー後退状態を抜ける際の終了処理（現在は空実装）。
	//! @param machine ステートマシン本体。
	void EnemyLeaderStepBackState::OnExit(IStateMachine* machine)
	{
	}
	//! @brief 後退ジャンプのタイミング制御を行う更新処理。
	//! @param machine ステートマシン本体。
	//! @param dt 前フレームからの経過時間。
	void EnemyLeaderStepBackState::Update(IStateMachine* machine, float dt)
	{
		// 後退経過時間を加算する。
		step_back_timer += dt;
		if (step_back_timer >= 0.5f && !jumped) {
			//後退
			// 自身の前方逆方向（後方）に上方向を加えた力ベクトルを生成する。
			Vector3 back_dir = -enemy->transform->AxisZ();
			back_dir.y = 1.0f;
			// 後方斜め上への力を一度だけ付与して後退ジャンプを実現する。
			rb->AddForce(back_dir * 20.0f, ForceMode::VelocityCange);
			// 二重ジャンプを防ぐためにフラグを立てる。
			jumped = true;
		}
	}
}