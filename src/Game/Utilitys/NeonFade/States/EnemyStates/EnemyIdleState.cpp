//---------------------------------------------------------------------------
//! @file   EnemyIdleState.cpp
//! @brief  EnemyIdleStateの実装。敵の待機状態の処理を行う
//---------------------------------------------------------------------------
#include "precompile.h"
#include "EnemyIdleState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"

//! @brief 待機状態を構築し、必要なコンポーネント参照を取得する。
//! @param owner_ この状態を所有する敵オブジェクト。
NeonFade::EnemyIdleState::EnemyIdleState(Enemy* owner_) :
	IState(static_cast<GameObject*>(owner_))
{
	// 状態所有者の敵本体を保持する。
	enemy = owner_;
	// 待機中の速度制御に使用する剛体を取得する。
	rb = owner_->rb.lock().get();
	// 待機モーション再生に使用するアニメータを取得する。
	animator = owner_->animator.lock().get();

}

//! @brief 待機状態へ入った瞬間の初期化処理。
//! @param machine ステートマシン本体。
void NeonFade::EnemyIdleState::OnEnter(IStateMachine* machine)
{
	// 待機モーションをループ再生して停止演出を開始する。
	animator->Play("enemy_idle",true);
}

//! @brief 待機状態を抜ける際の終了処理（現在は空実装）。
//! @param machine ステートマシン本体。
void NeonFade::EnemyIdleState::OnExit(IStateMachine* machine)
{
}

//! @brief 毎フレーム実行する待機状態の更新処理（現在は空実装）。
//! @param machine ステートマシン本体。
//! @param dt 前フレームからの経過時間。
void NeonFade::EnemyIdleState::Update(IStateMachine* machine, float dt)
{
}
