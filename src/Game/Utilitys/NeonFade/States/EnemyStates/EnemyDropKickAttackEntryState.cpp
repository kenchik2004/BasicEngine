#include "EnemyDropKickAttackEntryState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Components/EnemyController.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/EnemyRVOSystem.h"


namespace NeonFade
{
	EnemyDropKickAttackEntryState::EnemyDropKickAttackEntryState(Enemy* owner_enemy_)
		: IState(static_cast<GameObject*>(owner_enemy_))
	{

		//所有者となるEnemyオブジェクトへのポインタを保存する
		owner_enemy = owner_enemy_;

		//参照するコンポーネントを取得する
		animator = owner_enemy->animator.lock().get();
		rb = owner_enemy->rb.lock().get();


		// "dropkick_main"ステートが存在するか確認し、存在しない場合は新規作成して登録する
		{
			auto atk_main_state = owner_enemy->enem_controller.lock()->GetStateMachine()->GetState("dropkick_main");
			if (!atk_main_state) {
				// "dropkick_main"ステートが存在しない場合は新規作成して登録する
				auto new_atk_main_state = make_safe_unique<EnemyDropKickAttackMainState>(owner_enemy);
				new_atk_main_state->SetName("dropkick_main");
				owner_enemy->enem_controller.lock()->GetStateMachine()->AddState("dropkick_main", std::move(new_atk_main_state));
			}
		}

		// 遷移条件を登録する
		{
			//一定時間以上走ってもプレイヤーに届かない場合は、攻撃を中止してidle状態に遷移する
			auto failed_func = [this]() {
				return attack_timer >= ENTRY_DURATION;
				};

			//プレイヤーが攻撃範囲内に入った場合、攻撃状態に遷移する
			auto to_atk_main = [this]() {
				//軽量な距離比較用に2乗して使用する
				static constexpr float ATTACK_RANGE_SQUARED = ATTACK_TRANSIT_RANGE * ATTACK_TRANSIT_RANGE;

				return to_target.magnitudeSquared() < ATTACK_RANGE_SQUARED && attack_timer >= MINIMUM_ENTRY_DURATION;
				};

			//遷移条件としてコールバックの登録を行う
			RegisterChangeRequest("dropkick_main", to_atk_main, 0);
			RegisterChangeRequest("idle", failed_func, 0);
		}

	}
	void EnemyDropKickAttackEntryState::OnEnter(IStateMachine* machine)
	{
		// 攻撃開始時に走るアニメーションを再生する
		if (animator)
			animator->Play("enemy_fast_run", true);

		// 攻撃状態の経過時間をリセットする
		attack_timer = 0.0f;


		// プレイヤーへの方向ベクトルを計算する
		Vector3 to_player = owner_enemy->enem_controller->GetPlayer()->transform->position - owner_enemy->transform->position;
		to_player.y = 0; // Y軸方向の速度は無視する
		to_target = to_player;

	}
	void EnemyDropKickAttackEntryState::OnExit(IStateMachine* machine)
	{}
	void EnemyDropKickAttackEntryState::Update(IStateMachine* machine, float dt)
	{

		// アニメーターとリジッドボディが有効でない場合は処理を中断する
		if (!animator || !rb)
			return;

		// 攻撃状態の経過時間を更新する
		attack_timer += dt;

		// プレイヤーへの方向ベクトルを更新する
		Vector3 to_player = owner_enemy->enem_controller->GetPlayer()->transform->position - owner_enemy->transform->position;
		to_player.y = 0; // Y軸方向の速度は無視する
		to_target = to_player;

		// RVOシステムを使用して、敵同士が衝突しないように移動方向を補正する
		EnemyRVOSystem::CalculateCohesion(to_player, owner_enemy->transform.get(), owner_enemy);

		// 最終的な移動方向を正規化し、速度と回転を適用する
		EnemyRVOSystem::ApplyMovementAndRotation(to_player, owner_enemy->transform.get(), rb, ROTATION_SPEED, RUN_SPEED);
	}

	bool EnemyDropKickAttackEntryState::CanTransitTo(const std::string& state_name)
	{

		//ダメージ、ノックバック、死亡状態への遷移は無条件で許可する
		if (state_name == "damage" || state_name == "knock_back" || state_name == "knock_front" || state_name == "die")
			return true;

		//それ以外の状態への遷移は許可しない
		return false;
	}




	EnemyDropKickAttackMainState::EnemyDropKickAttackMainState(Enemy* owner_enemy_)
		: IState(static_cast<GameObject*>(owner_enemy_))
	{

		//所有者となるEnemyオブジェクトへのポインタを保存する
		owner_enemy = owner_enemy_;

		//参照するコンポーネントを取得する
		animator = owner_enemy->animator.lock().get();
		rb = owner_enemy->rb.lock().get();
		col = owner_enemy->col.lock().get();


		// 遷移条件を登録する
		{
			//一定時間経過後にidle状態に遷移する
			std::function<bool()> change_func = [this]() {
				return attack_timer >= ATTACK_DURATION;
				};

			//遷移条件としてコールバックの登録を行う
			RegisterChangeRequest("idle", change_func, 0);
		}


	}

	void EnemyDropKickAttackMainState::OnEnter(IStateMachine* machine)
	{


		if (animator)
			animator->Play("enemy_dropkick", false, 0.6f, 0.1f);
		attack_timer = 0.0f;

		//物理、コライダーコンポーネントが有効でない場合は処理を中断する
		if (!rb || !col)
			return;

		// プレイヤーへの方向ベクトルを計算し、プレイヤーに向かって跳び蹴りを行うための速度を設定する
		Vector3 to_player = owner_enemy->enem_controller->GetPlayer()->transform->position - owner_enemy->transform->position;

		// RVOシステムを使用して、敵同士が衝突しないように移動方向を補正する
		EnemyRVOSystem::CalculateCohesion(to_player, owner_enemy->transform.get(), owner_enemy);
		// 最終的な移動方向を正規化し、速度を適用する
		EnemyRVOSystem::ApplyMovement(to_player, rb, ATTACK_SPEED);
		//ApplyMovementは水平速度のみを設定するため、Y軸方向の速度を追加で設定する
		rb->velocity.y = ATTACK_SPEED_Y;

		// 後でコリジョンの高さを元に戻せるように、攻撃開始時のコリジョンの高さを保存しておく
		initial_collision_height = col->height;
	}

	void EnemyDropKickAttackMainState::OnExit(IStateMachine* machine)
	{
		// 攻撃終了時にコリジョンの高さを元に戻す
		if (col)
			col->height = initial_collision_height;

		if (attack_col)
			attack_col->RemoveThisComponent();

	}

	void EnemyDropKickAttackMainState::Update(IStateMachine* machine, float dt)
	{

		// アニメーターとリジッドボディが有効でない場合は処理を中断する
		if (!animator || !rb)
			return;

		// 攻撃状態の経過時間を更新する
		attack_timer += dt;

		// 攻撃アニメーションが終了した場合、idleアニメーションを再生しておく
		if (!animator->IsPlaying())
		{
			animator->PlayIfNoSame("enemy_idle", true);
		}

		if (attack_timer > INVINCIBILITY_START_TIME && attack_timer < INVINCIBILITY_END_TIME)
		{
			// 無敵時間中は、攻撃判定用のコライダーを有効化する
			if (!attack_col)
			{
				// 攻撃判定用のコライダーを作成して追加する
				auto new_attack_col = owner_enemy->AddComponent<SphereCollider>(Vector3(0.0f, 0.0f, 3.0f), Quaternion(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, true, Collider::Layer::Wepon, Collider::Layer::Player);
				attack_col = new_attack_col;
			}
		}
		else
		{
			// 無敵時間外では、攻撃判定用のコライダーを削除する
			if (attack_col)
			{
				attack_col->RemoveThisComponent();
				attack_col.reset();
			}
		}
		if (attack_timer > INVINCIBILITY_END_TIME) {
			//無敵時間外(蹴り終わって立ち上がる時)には、敵同士が衝突しないように移動方向を補正する
			Vector3 cohesion_velocity = { 0.0f,0.0f,0.0f };
			EnemyRVOSystem::CalculateCohesion(cohesion_velocity, owner_enemy->transform.get(), owner_enemy);
			EnemyRVOSystem::ApplyMovement(cohesion_velocity, rb,1.0f);
		}

		// 攻撃中は必要に応じてコリジョンの高さを変更する
		ChangeCollisionHeight();
	}

	void EnemyDropKickAttackMainState::OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info)
	{
		if (hit_info.collision == attack_col.lock()) {
			auto player = SafeStaticCast<Player>(hit_info.hit_collision->owner.lock());
			player->Damage(1); // プレイヤーにダメージを与える
		}
	}

	bool EnemyDropKickAttackMainState::CanTransitTo(const std::string& state_name)
	{
		// 無敵時間中でなければ、ダメージやノックバック、死亡状態への遷移を許可する
		if (attack_timer < INVINCIBILITY_START_TIME || attack_timer > INVINCIBILITY_END_TIME)
		{
			if (state_name == "damage" || state_name == "knock_back" || state_name == "knock_front" || state_name == "die")
				return true;
		}

		// それ以外の状態への遷移は許可しない
		return false;
	}

	void EnemyDropKickAttackMainState::ChangeCollisionHeight()
	{
		// コリジョンの高さを変更するタイミングで、線形補間を使用して滑らかに変化させる

		//攻撃初めのコリジョンの高さを変更するタイミング
		if (attack_timer >= COLLISION_EDIT_START_TIME && attack_timer <= COLLISION_EDIT_END_TIME) {
			// 線形補間を使用してコリジョンの高さを変更する
			EditCollisionHeight(COLLISION_EDIT_START_TIME, COLLISION_EDIT_END_TIME, attack_timer, initial_collision_height, EDITED_COLLISION_HEIGHT);
			return;
		}

		//攻撃終わりのコリジョンの高さを元に戻すタイミング
		if (attack_timer >= COLLISION_RESTORE_START_TIME && attack_timer <= COLLISION_RESTORE_END_TIME) {
			// 線形補間を使用してコリジョンの高さを元に戻す
			EditCollisionHeight(COLLISION_RESTORE_START_TIME, COLLISION_RESTORE_END_TIME, attack_timer, EDITED_COLLISION_HEIGHT, initial_collision_height);
		}
	}

	void EnemyDropKickAttackMainState::EditCollisionHeight(const float start_time, const float end_time, const float current_time, const float initial_height, const float target_height)
	{
		// コリジョンの高さを線形補間で変更する
		if (!col)
			return;

		// 線形補間のパラメータtを計算する
		float t = (current_time - start_time) / (end_time - start_time);
		// tを0.0fから1.0fの範囲に制限する
		t = std::clamp(t, 0.0f, 1.0f);

		// 線形補間を使用してコリジョンの高さを変更する
		col->height = std::lerp(initial_height, target_height, t);
	}
}