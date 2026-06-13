//---------------------------------------------------------------------------
//! @file   EnemyWelterState.cpp
//! @brief  EnemyWelterStateの実装。敵のよろめき状態の処理を行う
//---------------------------------------------------------------------------
#include "EnemyWelterState.h"
#include "Game/Objects/NeonFade/Enemy.h"

namespace NeonFade
{
	//! @brief よろめき状態を構築し、指定時間後にアイドルへ戻る遷移条件を登録する。
	//! @param owner_ この状態を所有する敵オブジェクト。
	EnemyWelterState::EnemyWelterState(Enemy* owner_)
		:IState(static_cast<GameObject*>(owner_))
	{
		// 状態所有者の敵本体を保持する。
		enemy = owner_;
		// 前進速度設定に使用する剛体を取得する。
		rb = enemy->rb.lock().get();
		// よろめきモーション再生に使用するアニメータを取得する。
		animator = enemy->animator.lock().get();
		// よろめきタイマが上限を超えたらアイドルへ遷移させる条件式。
		std::function default_change = [this]() {
			return timer >= WELTER_TIME;
			};
		// 遷移先「idle」と時間終了条件を優先度 1 で登録する。
		RegisterChangeRequest("idle", default_change, 1);

	}
	//! @brief よろめき状態へ入った瞬間の初期化処理。
	//! @param machine ステートマシン本体。
	void EnemyWelterState::OnEnter(IStateMachine* machine)
	{
		// よろめきタイマを初期化して経過時間の計測を開始する。
		timer = 0.0f;
		// 同一アニメーションが既に再生中でない場合のみ逃走モーションを再生する。
		enemy->animator.lock()->PlayIfNoSame("enemy_escape", true, 0.0f, 0.1f, false);
	}
	//! @brief よろめき状態を抜ける際の終了処理（現在は空実装）。
	//! @param machine ステートマシン本体。
	void EnemyWelterState::OnExit(IStateMachine* machine)
	{
	}
	//! @brief ランダム回転と前進速度の設定を行うよろめき更新処理。
	//! @param machine ステートマシン本体。
	//! @param dt 前フレームからの経過時間。
	void EnemyWelterState::Update(IStateMachine* machine, float dt)
	{
		// よろめき経過時間を加算する。
		timer += dt;
		// トランスフォームを取得して回転操作を行う。
		Transform* trns = enemy->transform.get();
		// Y 軸周りにフレーム時間スケールのランダムな角速度で回転させ、よろめき感を演出する。
		trns->AddRotation(Vector3(0, 720.0f * dt * Random::Range(-1.0f, 1.0f), 0));

		// 現在の前方ベクトルを取得する。
		Vector3 forward = enemy->transform->AxisZ();
		// 前方方向へ一定速度で移動させてよろめきながら走る挙動を実現する。
		rb->velocity = forward * 15.0f;

	}
}