//-----------------------------------------------------------------------------
//! @file   EnemyKnockFrontState.cpp
//! @brief  EnemyKnockFrontStateの実装。前方にノックバックした状態を管理する
//! @author 岩野 健太郎(E_Nock)
//-----------------------------------------------------------------------------


#include "EnemyKnockFrontState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/AbstractEnemyBrain.h"


namespace NeonFade
{

	//! @brief EnemyKnockFrontStateのコンストラクタ
	//! @param owner_enemy_ この状態を管理するEnemyオブジェクトへのポインタ
	EnemyKnockFrontState::EnemyKnockFrontState(Enemy* owner_enemy_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		//各種必要なコンポーネントへの参照を取得
		{
			// オーナーエネミーと必要なコンポーネントへの参照を取得
			owner_enemy = owner_enemy_;
			animator = owner_enemy->animator.lock().get();
			rb = owner_enemy->rb.lock().get();
			col = owner_enemy->col.lock().get();
		}

		// ダウン後に死亡する可能性があるため、死亡状態への遷移条件も登録しておく
		{
			std::function<bool()> die_request = [this]()
				{
					return elapsed_time >= KNOCK_FRONT_DURATION && owner_enemy->IsDead();
				};
			RegisterChangeRequest("die", die_request, 0); // 死亡状態への遷移は、死亡判定が真になったときに優先的に行う
		}
		// ノックバック状態の継続時間が経過した後に自動で立ち上がるように、状態遷移の条件を登録する
		{
			// ダウン状態からの復帰条件を登録
			std::function<bool()> default_exit = [this]()
				{
					return elapsed_time >= KNOCK_FRONT_DURATION;
				};

			// ダウン状態からの復帰は、ダウン時間が経過した後に自動で行う
			RegisterChangeRequest("stand_up_front", default_exit, 1);
		}
	}

	//! @brief EnemyKnockFrontStateに入るときの処理
	//! @param machine この状態を管理する状態マシンへのポインタ
	void EnemyKnockFrontState::OnEnter(IStateMachine* machine)
	{

		// 各種フラグとタイマーの初期化
		{
			elapsed_time = 0.0f;
			hit_stop_started = false;
			hit_stop_ended = false;
		}

		//アニメーション再生
		animator->Play("enemy_down_forward");

		auto enem_state_machine = dynamic_cast<EnemyStateMachine*>(machine);
		if (!enem_state_machine || !enem_state_machine->GetBrain())
			return;

		Vector3 knock_back_dir = enem_state_machine->GetBrain()->GetKnockBackVec();
		rb->velocity = knock_back_dir; // ノックバックベクトルを速度に直接設定する
		knock_back_dir.y = 0; // 水平方向のノックバックに限定する
		owner_enemy->transform->SetAxisZ(knock_back_dir.getNormalized()); // ノックバックの方向に背を向けるように回転させる
	}

	//! @brief EnemyKnockFrontStateの更新処理
	//! @param machine この状態を管理する状態マシンへのポインタ
	void EnemyKnockFrontState::Update(IStateMachine* machine, float dt)
	{
		elapsed_time += dt;

		// ヒットストップの開始と終了のタイミングで処理を行う
		if (!hit_stop_started && elapsed_time >= HIT_STOP_START) {
			// ヒットストップ開始
			hit_stop_started = true;

			// 現在の速度を保存してから停止する
			knock_front_velocity = rb->velocity;
			// 速度を0にして位置を固定する
			rb->velocity = { 0, 0, 0 };
			rb->freeze_position = { true, true, true };

			// アニメーションも一時停止する
			animator->Pause();

		}

		if (!hit_stop_ended && elapsed_time >= HIT_STOP_START + HIT_STOP_DURATION) {
			// ヒットストップ終了
			hit_stop_ended = true;

			// 保存しておいた速度を復元して位置の固定も解除する
			rb->velocity = knock_front_velocity;
			rb->freeze_position = { false, false, false };

			// アニメーションの一時停止も解除する
			animator->Resume();

		}

		// ノックバック中にコライダーの向きを変える
		RotateCollider();
	}

	//! @brief EnemyKnockFrontStateから出るときの処理
	//! @param machine この状態を管理する状態マシンへのポインタ
	void EnemyKnockFrontState::OnExit(IStateMachine* machine)
	{
	}

	//! @brief EnemyKnockFrontStateから遷移可能な状態を制限する関数
	//! @param state_name 遷移先の状態名
	bool EnemyKnockFrontState::CanTransitTo(const std::string& state_name)
	{
		// この状態から遷移できる状態を制限する
		//ノック中にノックバックや前方ノックに遷移することは許可する
		if (state_name == "knock_back" || state_name == "knock_front")
			return true;

		// それ以外の状態への遷移は許可しない
		return false;
	}
	void EnemyKnockFrontState::RotateCollider()
	{
		static constexpr float ROTATION_DURATION = 0.3f; // コライダーの回転にかける時間（秒）
		static constexpr float ROTATION_START = 0.9f;		//<! 回転を開始するタイミング（秒）
		static const Quaternion initial_collider_rotation = Quaternion(DEG2RAD(90.0f), { 0, 0, -1 });				//<! コライダーの初期回転の定数クォータニオン
		Quaternion target_rotation = initial_collider_rotation * Quaternion(DEG2RAD(90.0f), { 0, 1, 0 }); // コライダーを90度回転させるクォータニオン

		float t = std::clamp((elapsed_time - ROTATION_START) / ROTATION_DURATION, 0.0f, 1.0f);		// 経過時間に応じて回転を補間するためのパラメーター
		col->rotation = Slerp(initial_collider_rotation, target_rotation, t); // コライダーの回転を補間して更新する
	}
}