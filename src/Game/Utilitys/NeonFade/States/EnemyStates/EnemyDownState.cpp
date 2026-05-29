//---------------------------------------------------------------------------
//! @file   EnemyDownState.cpp
//! @brief  EnemyDownStateの実装。敵のダウン状態の処理を行う
//---------------------------------------------------------------------------
#include "precompile.h"
#include "EnemyDownState.h"

#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"
#include "Game/Objects/NeonFade/Enemy.h"




//! @brief ダウン状態を構築し、ヒットストップとコライダ回転のアニメーションコールバックおよびアイドル遷移条件を登録する。
//! @param owner_ この状態を所有する敵オブジェクト。
NeonFade::EnemyDownState::EnemyDownState(Enemy* owner_)
	:IState(static_cast<GameObject*>(owner_))
{

	// 状態所有者の敵本体を保持する。
	enemy = owner_;
	// ダウンモーション再生とコールバック登録に使用するアニメータを取得する。
	animator = enemy->animator.lock().get();
	// ノックバック速度の書き込みに使用する剛体を取得する。
	rb = enemy->rb.lock().get();

	// ヒット効果音が未ロードの場合のみ取得する。
	if (!hit_se)
		hit_se = AudioManager::CloneByName(u8"hit_se");

	{
		// 後方倒れアニメーション（enemy_down）用のコールバックブロック。
		// 指定フレームでアニメーション速度を極低速化してヒットストップ効果を表現するコールバック。
		std::function hit_stop = [this]() {
			animator->anim_speed = 0.0001f;
			// スケール非依存の経過時間でヒットストップタイマを加算する。
			hit_stop_timer += Time::UnscaledDeltaTime();
			};
		// 倒れ込み後にコライダを横倒し姿勢へ回転させ、ノックアウト効果音を再生するコールバック。
		std::function collision_rotate = [this]() {
			auto col = enemy->GetComponent<CapsuleCollider>();
			// コライダが存在しない場合は処理をスキップする。
			if (!col)
				return;
			// カプセルコライダを横向きに回転させて倒れた姿勢と合わせる。
			col->rotation = Quaternion(DEG2RAD(90), { 0,1,0 });
			// コライダ位置を低い位置へオフセットする。
			col->position = { 0,1.0f,0 };
			// 倒れ後は武器・地形・乗り物とのみ衝突判定を持つよう制限する。
			col->SetHitGroup(Collider::Layer::Terrain | Collider::Layer::Vehicle | Collider::Layer::Wepon);
			// ノックアウト効果音が有効であれば再生する。
			if (knockout_se)
				knockout_se->PlayOneShot();
			};
		// 後方倒れアニメーション10フレーム目でヒットストップ処理を実行する。
		animator->SetAnimationCallBack("enemy_down", hit_stop, 10, "hit_stop");
		// 後方倒れアニメーション30フレーム目でコライダ回転処理を実行する。
		animator->SetAnimationCallBack("enemy_down", collision_rotate, 30, "collision_rotate");
	}
	{
		// 前方倒れアニメーション（enemy_down_forward）用のコールバックブロック。
		// 指定フレームでアニメーション速度を極低速化してヒットストップ効果を表現するコールバック。
		std::function hit_stop = [this]() {
			animator->anim_speed = 0.0001f;
			hit_stop_timer += Time::UnscaledDeltaTime();
			};
		// 倒れ込み後にコライダを横倒し姿勢へ回転させ、ノックアウト効果音を再生するコールバック。
		std::function collision_rotate = [this]() {
			auto col = enemy->GetComponent<CapsuleCollider>();
			if (!col)
				return;
			col->rotation = Quaternion(DEG2RAD(90), { 0,1,0 });
			col->position = { 0,1.0f,0 };
			col->SetHitGroup(Collider::Layer::Terrain | Collider::Layer::Vehicle | Collider::Layer::Wepon);
			if (knockout_se)
				knockout_se->PlayOneShot();
			};
		// 前方倒れアニメーション10フレーム目でヒットストップ処理を実行する。
		animator->SetAnimationCallBack("enemy_down_forward", hit_stop, 10, "hit_stop");
		// 前方倒れアニメーション30フレーム目でコライダ回転処理を実行する。
		animator->SetAnimationCallBack("enemy_down_forward", collision_rotate, 30, "collision_rotate");
	}
	// ダウン時間が閾値を超えたらアイドルへ遷移させる条件式。
	std::function default_change = [this]() {
		return exit_timer >= EXIT_TIME;
		};
	// 遷移先「idle」と時間判定条件を優先度 1 で登録する。
	RegisterChangeRequest("idle", default_change, 1);
	// ノックアウト効果音が未ロードの場合のみ取得する。
	if (!knockout_se)
		knockout_se = AudioManager::CloneByName(u8"knockout_se");
}

//! @brief ダウン状態へ入った瞬間の初期化処理。ノックバック方向を算出し対応する倒れアニメーションを再生する。
//! @param machine 状態機械本体。
void NeonFade::EnemyDownState::OnEnter(IStateMachine* machine)
{
	// ヒットストップタイマとダウン経過タイマを初期化する。
	hit_stop_timer = 0;
	exit_timer = 0;
	// ヒット効果音を一回再生してダメージ受け付け開始を演出する。
	if (hit_se)
		hit_se->PlayOneShot();
	// 敵専用状態機械へダウンキャストして入力移動ベクトルを取得する。
	auto enem_machine = static_cast<EnemyStateMachine*>(machine);
	// ノックバック方向を移動ベクトルから取得し、垂直成分を除去する。
	knock_back_vec = enem_machine->move_vec;
	knock_back_vec.y = 0;
	// 敵が現在向いている正面方向を取得する。
	Vector3 current_z = enemy->transform->AxisZ();
	// ノックバック方向と現在の正面が一致している（前方に倒れる）かを判定する。
	bool down_forward = false;
	if (!knock_back_vec.isZero() && current_z.dot(knock_back_vec) > 0.5f)
		down_forward = true;

	// ノックバック方向がゼロの場合は現在の正面の逆方向を使用する。
	if (knock_back_vec.isZero())
		knock_back_vec = -current_z;
	Vector3 set_forward = knock_back_vec;
	set_forward.normalize();
	if (down_forward) {								// だいたい前方に倒れる場合	
		// 倒れる方向と進行方向が一致しているため正面向きに倒れさせる。
		enemy->transform->SetAxisZ(set_forward);
		animator->Play("enemy_down_forward", false, 0.05f, 0.2f, true);
	}
	else {											//倒れる方向と進行方向があまりにも違う場合
		// 倒れる方向と進行方向が逆向きのため逆方向に向けて後方倒れアニメーションを再生する。
		enemy->transform->SetAxisZ(-set_forward);
		animator->Play("enemy_down", false, 0.05f, 0.2f, true);
	}
}

//! @brief ダウン状態を抜ける際にコライダを通常の立ち姿勢に戻す終了処理。
//! @param machine 状態機械本体。
void NeonFade::EnemyDownState::OnExit(IStateMachine* machine)
{
	// アニメーション速度を通常値へ戻す。
	animator->anim_speed = 1.0f;
	auto col = enemy->GetComponent<CapsuleCollider>();
	// コライダを直立姿勢（90度傾き）に戻す。
	col->rotation = Quaternion(DEG2RAD(90), { 0,0,-1 });
	// コライダのオフセットをリセットする。
	col->position = { 0,0,0 };
	// 起き上がり後は全衝突レイヤーとの判定を復元する。
	col->SetHitGroup(Collider::Layer::Terrain | Collider::Layer::Vehicle | Collider::Layer::Wepon | Collider::Layer::Player | Collider::Layer::Enemy);

}

//! @brief ダウン中のヒットストップ解除とノックバック速度の適用を行う更新処理。
//! @param machine 状態機械本体。
//! @param dt 前フレームからの経過時間。
void NeonFade::EnemyDownState::Update(IStateMachine* machine, float dt)
{
	// ダウン経過時間を加算して遷移条件判定に使用する。
	exit_timer += dt;
	// ヒットストップが開始されている間はタイマを加算する。
	if (hit_stop_timer > 0)
		hit_stop_timer += dt;
	// ヒットストップ時間を超えたらアニメーション速度を戻し、ノックバック速度を適用する。
	if (hit_stop_timer > HITSTOP_TIME) {
		auto enem_machine = static_cast<EnemyStateMachine*>(machine);
		// アニメーション速度を通常に戻してモーションを再開する。
		animator->anim_speed = 1.0f;
		// ノックバック方向へ速度を設定して吹き飛び表現を実現する。
		rb->SetVelocity(knock_back_vec);
		// タイマをリセットしてヒットストップを終了する。
		hit_stop_timer = 0;
	}
}
