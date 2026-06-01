//---------------------------------------------------------------------------
//! @file   EnemyAttackReadyState.cpp
//! @brief  EnemyAttackReadyStateの実装。敵の攻撃準備状態の処理を行う
//---------------------------------------------------------------------------
#include "EnemyAttackReadyState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/Player.h"

namespace NeonFade {
	//! @brief 攻撃準備状態を構築し、攻撃チャージへの遷移条件を登録する。
	//! @param enemy_ この状態を所有する敵オブジェクト。
	EnemyAttackReadyState::EnemyAttackReadyState(Enemy* enemy_)
		:IState(static_cast<GameObject*>(enemy_))
	{
		// 状態所有者の敵本体を保持する。
		owner_enemy = enemy_;
		// 歩行モーション再生に使用するアニメータを取得する。
		animator = enemy_->animator.lock().get();
		// 移動方向制御と速度設定に使用する剛体を取得する。
		rb = enemy_->rb.lock().get();
		// 追跡対象のプレイヤーをシーン上から取得して弱参照で保持する。
		target = SceneManager::Object::Get<Player>();
		// next フラグが真になった時点で攻撃チャージへ遷移する条件式。
		std::function<bool()> request = [this]() {
			return next;
			};
		// 遷移先「attack_charge」と遷移条件を優先度 1 で状態機械へ登録する。
		RegisterChangeRequest("attack_charge", request, 1);
	}
	//! @brief 攻撃準備状態へ入った瞬間の初期化処理。
	//! @param machine 状態機械本体。
	void EnemyAttackReadyState::OnEnter(IStateMachine* machine)
	{
		// 前回の攻撃遷移フラグをリセットする。
		next = false;
		// 基準値に乱数を加算して毎回異なる待機時間を設定する。
		ready_timer = max_ready_time + Random::Float01();
		// 準備状態ではアニメーション速度を 2 倍にして素早い足踏みを表現する。
		animator->anim_speed = 2.0f;
		// 攻撃を開始する距離を乱数で決定し、行動パターンに変化を与える。
		random_attack_distance = 3.0f + Random::Float01() * 5.0f;
		// 歩行モーションをランダムな開始位相でループ再生する。
		animator->Play("enemy_walk", true, Random::Float01());
	}
	//! @brief 目標へ接近し、条件が整い次第攻撃チャージへ遷移を要求する更新処理。
	//! @param machine 状態機械本体。
	//! @param dt 前フレームからの経過時間。
	void EnemyAttackReadyState::Update(IStateMachine* machine, float dt)
	{
		// 残り待機時間を経過時間だけ減算する。
		ready_timer -= dt;
		Vector3 mov_vec;

		// 敵専用状態機械へダウンキャストして状態情報を利用できる形にする。
		auto enem_machine = static_cast<EnemyStateMachine*>(machine);
		// 自身からプレイヤーへのベクトルを求める。
		mov_vec = target->transform->position - owner_enemy->transform->position;
		// 地面法線方向の成分を除去し、水平面上の接近ベクトルのみを扱う。
		mov_vec = ProjectOnPlane(mov_vec, { 0,1,0 });

		// まだ接近距離に達していないか待機時間が残っている場合はプレイヤーへ追跡を続ける。
		if (mov_vec.magnitudeSquared() >= random_attack_distance * random_attack_distance || ready_timer > 0.0f) {

			// ベクトルを正規化して一定速度で前進させるための単位ベクトルにする。
			mov_vec.normalize();
			// 前進方向に敵の正面を向ける。
			owner_enemy->transform->SetAxisZ(mov_vec);
			// 移動速度を 10 に設定する。
			mov_vec *= 10;
			// 垂直速度は物理計算の結果を維持する。
			mov_vec.y = rb->velocity.y;
			rb->velocity = mov_vec;
			return;
		}
		// 接近完了かつ待機時間経過後に攻撃チャージへの遷移フラグを立てる。
		next = true;
	}
	//! @brief 攻撃準備状態を抜ける際の終了処理。
	//! @param machine 状態機械本体。
	void EnemyAttackReadyState::OnExit(IStateMachine* machine)
	{
		// 速度倍率を通常値に戻し、次状態のアニメーション再生に影響を与えないようにする。
		animator->anim_speed = 1.0f;
	}
}