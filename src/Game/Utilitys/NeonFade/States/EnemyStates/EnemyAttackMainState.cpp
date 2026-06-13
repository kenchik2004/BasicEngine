//---------------------------------------------------------------------------
//! @file   EnemyAttackMainState.cpp
//! @brief  EnemyAttackMainStateの実装。敵の攻撃メイン状態の処理を行う
//---------------------------------------------------------------------------
#include "EnemyAttackMainState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"

namespace NeonFade
{
	//! @brief 攻撃メイン状態を構築し、攻撃判定の生成と破棄をアニメーションイベントへ登録する。
	//! @param owner_ この状態を所有する敵オブジェクト。
	EnemyAttackMainState::EnemyAttackMainState(Enemy* owner_)
		:IState(static_cast<GameObject*>(owner_))
	{
		// 状態所有者の敵本体を保持する。
		owner_enemy = owner_;
		// 攻撃モーション再生とコールバック登録に使用するアニメータを取得する。
		animator = owner_enemy->animator.lock().get();
		// 攻撃フレームでヒットボックスを生成するコールバックを定義する。
		std::function<void()> hit_box_attach = [this]() {
			// 既存ヒットボックスが残っている場合は重複判定を防ぐため先に破棄する。
			if (hit_box) {
				hit_box->RemoveThisComponent();
				hit_box.reset();
			}
			// 敵へ球体コライダを追加し、攻撃判定として構成する。
			auto hit_box_ = owner_enemy->AddComponent<SphereCollider>();
			// 接触通知のみ扱うためトリガー判定を有効化する。
			hit_box_->is_trigger = true;
			// この判定を武器レイヤーとして扱う。
			hit_box_->SetLayer(Collider::Layer::Wepon);
			// プレイヤーレイヤーとのみ接触イベントを受け取る。
			hit_box_->SetHitGroup(Collider::Layer::Player);
			// 攻撃の有効半径を設定する。
			hit_box_->radius = 5.0f;
			// 敵前方かつやや上方へ判定中心を配置する。
			hit_box_->position = Vector3(0, 3.5f, 3.5f);
			// 後続処理で参照できるようメンバへ保持する。
			hit_box = hit_box_;
			};
		// 攻撃終了フレームでヒットボックスを破棄するコールバックを定義する。
		std::function<void()> hit_box_remove = [this]() {
			// 生成済みヒットボックスが存在する場合のみ安全に削除する。
			if (hit_box) {
				hit_box->RemoveThisComponent();
				hit_box.reset();
			}
			};
		// 攻撃アニメーション15フレーム目で攻撃判定生成を実行する。
		animator->SetAnimationCallBack("enemy_attack_main", hit_box_attach, 15, "hit_box_attach");
		// 攻撃アニメーション30フレーム目で攻撃判定破棄を実行する。
		animator->SetAnimationCallBack("enemy_attack_main", hit_box_remove, 30, "hit_box_remove");

	}
	//! @brief 攻撃メイン状態へ入った瞬間の初期化処理。
	//! @param machine ステートマシン本体。
	void EnemyAttackMainState::OnEnter(IStateMachine* machine)
	{
		// 攻撃状態の継続時間を計測するタイマを初期化する。
		attack_timer = 0;
		// 攻撃メインモーションを再生して攻撃シーケンスを開始する。
		animator->Play("enemy_attack_main");


	}
	//! @brief 攻撃メイン状態を抜ける際の終了処理。
	//! @param machine ステートマシン本体。
	void EnemyAttackMainState::OnExit(IStateMachine* machine)
	{
		// 状態終了時に残存ヒットボックスを確実に除去する。
		if (hit_box) {
			hit_box->RemoveThisComponent();
			hit_box.reset();
		}
	}
	//! @brief 攻撃メイン状態中の経過時間を更新する処理。
	//! @param machine ステートマシン本体。
	//! @param dt 前フレームからの経過時間。
	void EnemyAttackMainState::Update(IStateMachine* machine, float dt)
	{
		// 攻撃開始からの経過時間を加算し、遷移条件判定に利用する。
		attack_timer += dt;

	}
	//! @brief 攻撃判定が他オブジェクトへ接触した際にダメージを与える処理。
	//! @param machine ステートマシン本体。
	//! @param hit_info 接触したコライダ情報。
	void EnemyAttackMainState::OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info)
	{
		// 接触相手の所有オブジェクトが存在する場合のみ処理を進める。
		if (auto player = hit_info.hit_collision->owner.lock()) {
			// 接触相手をプレイヤー型として扱えるか安全に判定する。
			auto player_neonfade = SafeStaticCast<NeonFade::Player>(player);
			// プレイヤーであれば固定ダメージを適用する。
			if (player_neonfade) {
				player_neonfade->Damage(10);
			}
		}
	}
}