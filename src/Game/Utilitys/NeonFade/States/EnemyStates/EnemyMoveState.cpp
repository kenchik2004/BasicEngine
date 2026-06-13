//---------------------------------------------------------------------------
//! @file   EnemyMoveState.cpp
//! @brief  EnemyMoveStateの実装。敵の移動状態の処理を行う
//---------------------------------------------------------------------------
#include "EnemyMoveState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"

namespace NeonFade
{
	//! @brief 移動状態を構築し、必要なコンポーネントへの参照を初期化する。
	//! @param enemy_ この状態を所有する敵オブジェクト。
	EnemyMoveState::EnemyMoveState(Enemy* enemy_)
		:IState(static_cast<GameObject*>(enemy_))
	{
		// 状態所有者の敵本体を保持する。
		owner_enemy = enemy_;
		// 速度設定に使用する剛体を取得する。
		rb = enemy_->rb.lock().get();
		// 歩行モーション再生に使用するアニメータを取得する。
		animator = enemy_->animator.lock().get();
	}
	//! @brief 移動状態へ入った瞬間の初期化処理。
	//! @param machine ステートマシン本体。
	void EnemyMoveState::OnEnter(IStateMachine* machine)
	{
		// 歩行アニメーションをランダムな開始位置からループ再生し、敵間の動き均一化を防ぐ。
		animator->Play("enemy_walk", true, Random::Float01(), 0.1f);
	}
	//! @brief 移動方向への向き更新と速度設定を行う更新処理。
	//! @param machine ステートマシン本体。
	//! @param dt 前フレームからの経過時間。
	void EnemyMoveState::Update(IStateMachine* machine, float dt)
	{
		//進行方向に向ける
		// ステートマシンから移動ベクトルを取得する。
		auto enem_machine = static_cast<EnemyStateMachine*>(machine);
		Vector3 move_vec = enem_machine->move_vec;
		// 垂直成分を除去して水平面上での向き変更のみ行う。
		move_vec.y = 0;
		// 現在の前方と移動方向の内積を求め、ほぼ平行でない場合は向きを更新する。
		float dot = owner_enemy->transform->AxisZ().dot(move_vec);
		if (fabsf(dot) > FLT_EPSILON)
			owner_enemy->transform->SetAxisZ(move_vec.getNormalized());
		//移動
		// 重力の影響を保持するために現在の垂直速度を退避する。
		float vel_y = rb->velocity.y;
		// 水平速度を移動ベクトルで上書きして敵をターゲット方向へ移動させる。
		rb->velocity = move_vec;
		// 退避した垂直速度を復元して重力落下を維持する。
		rb->velocity.y = vel_y;

	}
	//! @brief 移動状態を抜ける際の終了処理（現在は空実装）。
	//! @param machine ステートマシン本体。
	void EnemyMoveState::OnExit(IStateMachine* machine)
	{
	}
}