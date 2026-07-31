//-----------------------------------------------------------------------------
//! @file   EnemyKnockFrontState.cpp
//! @brief  EnemyKnockFrontStateの実装。前方にノックバックした状態を管理する
//! @author 岩野 健太郎(E_Nock)
//-----------------------------------------------------------------------------


#include "EnemyKnockFrontState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/GameObjectWithLifeTime.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/AbstractEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/BasicEnemyBrain.h"
#include "Game/Components/EnemyController.h"


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

		//ノックダウンした際に弱っていたら、這いずり状態への遷移条件も登録しておく
		{
			std::function<bool()> crawl_request = [this]()
				{
					auto brain = owner_enemy->enem_controller->GetBrain();
					bool is_weakened = brain->IsWeakened();
					return elapsed_time >= KNOCK_FRONT_DURATION && is_weakened;
				};
			RegisterChangeRequest("crowling", crawl_request, 1); // 這いずり状態への遷移は、死亡状態への遷移の次に優先的に行う
		}

		// ノックバック状態の継続時間が経過した後に自動で立ち上がるように、状態遷移の条件を登録する
		{
			// ダウン状態からの復帰条件を登録
			std::function<bool()> default_exit = [this]()
				{
					return elapsed_time >= KNOCK_FRONT_DURATION;
				};

			// ダウン状態からの復帰は、ダウン時間が経過した後に自動で行う
			RegisterChangeRequest("stand_up_front", default_exit, 2);
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

		// チェインノックバックの判定時間内でなければ処理を行わない
		if (elapsed_time > CHAIN_KNOCK_BACK_START_TIME && elapsed_time < (CHAIN_KNOCK_BACK_START_TIME + CHAIN_KNOCK_BACK_DURATION))
			// チェインノックバックの判定用コライダーを作成
			if (!chain_knock_collider) {
				Vector3 null_pos = { 0,0,0 };
				Quaternion  null_rot = { 0,0,0,1 };
				float  radius = 3.0f; // コライダーの半径
				bool  is_trigger = true; // トリガーとして設定する
				Collider::Layer layer = Collider::Layer::Wepon; // コライダーのレイヤーを武器に設定する
				u8 mask = static_cast<u8>(Collider::Layer::Enemy); // 敵同士のみで衝突判定を行うためのマスク


				chain_knock_collider = owner_enemy->AddComponent<SphereCollider>(null_pos, null_rot, radius, is_trigger, layer, mask);


			}
		if (elapsed_time > (CHAIN_KNOCK_BACK_START_TIME + CHAIN_KNOCK_BACK_DURATION))
			// チェインノックバックの判定用コライダーを削除
			if (chain_knock_collider) {
				chain_knock_collider->RemoveThisComponent();
				chain_knock_collider.reset();
			}
	}

	//! @brief EnemyKnockFrontStateから出るときの処理
	//! @param machine この状態を管理する状態マシンへのポインタ
	void EnemyKnockFrontState::OnExit(IStateMachine* machine)
	{}

	void EnemyKnockFrontState::OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info)
	{
		// チェインノックバックの判定時間内でなければ処理を行わない
		if (elapsed_time < CHAIN_KNOCK_BACK_START_TIME || elapsed_time >(CHAIN_KNOCK_BACK_START_TIME + CHAIN_KNOCK_BACK_DURATION))
			return;

		// 衝突したオブジェクトが敵でなければ処理を行わない
		auto layer = hit_info.hit_collision->GetLayer();
		if (layer != Collider::Layer::Enemy || hit_info.hit_collision->owner.lock().get() == owner_enemy)
			return;

		auto other_enemy = SafeStaticCast<Enemy>(hit_info.hit_collision->owner.lock());

		auto* other_machine = other_enemy->enem_controller->GetStateMachine();
		if (other_machine->GetCurrentStateName() == "knock_back" || other_machine->GetCurrentStateName() == "knock_front")
			return; // すでにノックバック状態の敵には影響を与えない

		Vector3  knock_back_dir = rb->velocity; // 自分のノックバック方向を取得する
		knock_back_dir += (other_enemy->transform->position - owner_enemy->transform->position).getNormalized() * 3.0f; // 衝突した敵の方向も加える
		knock_back_dir.y = 0; //上下方向の力は無視する
		knock_back_dir = knock_back_dir.getNormalized() * CHAIN_KNOCK_BACK_FORCE; // チェインノックバックの力を加える

		knock_back_dir.y = CHAIN_KNOCK_BACK_UP_FORCE; // 上方向の力も加える
		other_enemy->Down(knock_back_dir); // 衝突した敵をダウンさせる
		other_enemy->Damage(CHAIN_KNOCK_BACK_DAMAGE); // 衝突した敵に小ダメージを与える

		{
			auto kokusen = SceneManager::Object::Create<GameObjectWithLifeTime>(u8"kokusen_effect", 1.0f);
			kokusen->transform->position = other_enemy->transform->position + Vector3(0, 4.0f, 0);
			auto  effect_comp = kokusen->AddComponent<EffectPlayer>("data/FX/KOKUSEN.efkefc");
			effect_comp->Play();
		}

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
		if (t >= 1.0f) {
			auto brain = owner_enemy->enem_controller->GetBrain();
			if (brain) {
				brain->SetIsCrawling(true); // 這いずり状態に設定する
			}
		}
	}
}