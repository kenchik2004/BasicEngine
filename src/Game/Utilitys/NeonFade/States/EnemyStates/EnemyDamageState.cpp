//---------------------------------------------------------------------------
//! @file   EnemyDamageState.cpp
//! @brief  EnemyDamageStateの実装。敵のダメージ状態の処理を行う
//---------------------------------------------------------------------------
#include "precompile.h"
#include "EnemyDamageState.h"

#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"
#include "Game/Objects/NeonFade/Enemy.h"

//! @brief ダメージ状態を構築し、ヒットストップ処理とアイドル遷移条件を登録する。
//! @param owner_ この状態を所有する敵オブジェクト。
NeonFade::EnemyDamageState::EnemyDamageState(Enemy* owner_) :
	IState(static_cast<GameObject*>(owner_))
{

	// 状態所有者の敵本体を保持する。
	enemy = owner_;
	// ダメージモーション再生とコールバック登録に使用するアニメータを取得する。
	animator = enemy->animator.lock().get();
	// 将来の速度操作に備えて剛体コンポーネントを取得する。
	rb = enemy->rb.lock().get();
	// 指定フレームでアニメーション速度を極低速化してヒットストップ効果を表現するコールバック。
	std::function hit_stop = [this]() {
		animator->anim_speed = 0.001f;
		// スケールに依存しない経過時間でヒットストップタイマを加算する。
		hit_stop_timer += Time::UnscaledDeltaTime();
		};
	// ダメージアニメーション3フレーム目でヒットストップ処理を実行する。
	animator->SetAnimationCallBack("enemy_damage", hit_stop, 3, "hit_stop");
	// アニメーション再生が終了したらアイドルへ遷移させる条件式。
	std::function default_change = [this]() {
		return !animator->IsPlaying();
		};
	// 遷移先「idle」と終了判定をデフォルト優先度で登録する。
	RegisterChangeRequest("idle", default_change, 1);

	// ヒット効果音クリップが未ロードの場合のみ取得する。
	if (!hit_se)
		hit_se = AudioManager::CloneByName(u8"hit_se");
}

//! @brief ダメージ状態へ入った瞬間の初期化処理。
//! @param machine 状態機械本体。
void NeonFade::EnemyDamageState::OnEnter(IStateMachine* machine)
{
	// ダメージモーションを再生して被ダメージ演出を開始する。
	animator->Play("enemy_damage");
	// アニメーション速度を上げてダメージリアクションを素早く見せる。
	animator->anim_speed = 1.6f;
	// ヒットストップタイマを初期化して次回計測に備える。
	hit_stop_timer = 0;
	// ヒット効果音を一回再生する。
	hit_se->PlayOneShot();
	//rb->velocity = { 0, 0, 0 };
}

//! @brief ダメージ状態を抜ける際の終了処理。
//! @param machine 状態機械本体。
void NeonFade::EnemyDamageState::OnExit(IStateMachine* machine)
{
	//animator->Stop();
	// アニメーション速度を通常値へ戻し、次状態の再生に影響しないようにする。
	animator->anim_speed = 1.0f;
}

//! @brief ダメージ演出中のヒットストップ制御を行う更新処理。
//! @param machine 状態機械本体。
//! @param dt 前フレームからの経過時間。
void NeonFade::EnemyDamageState::Update(IStateMachine* machine, float dt)
{
	// ダメージアニメーションが終了した場合はアイドル状態へ即遷移する。
	if (!animator->IsPlaying())
		machine->ChangeState("idle");
	// ヒットストップが開始されている間はスケール非依存の経過時間でタイマを加算する。
	if (hit_stop_timer > 0)
		hit_stop_timer += Time::UnscaledDeltaTime();
	// ヒットストップ時間を超えたら速度を通常に戻しタイマをリセットする。
	if (hit_stop_timer > HITSTOP_TIME) {
		animator->anim_speed = 1.6f;
		hit_stop_timer = 0;
	}

}
