#include "EnemyKnockBackState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/GameObjectWithLifeTime.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/AbstractEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/BasicEnemyBrain.h"
#include "Game/Components/EnemyController.h"


namespace NeonFade {
	EnemyKnockBackState::EnemyKnockBackState(Enemy* owner_enemy_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		owner_enemy = owner_enemy_;
		animator = owner_enemy->animator.lock().get();
		rb = owner_enemy->rb.lock().get();
		col = owner_enemy->col.lock().get();

		std::function <bool()> die_exit = [this]() {
			return elapsed_time >= KNOCK_BACK_DURATION && owner_enemy->IsDead();
			};
		RegisterChangeRequest("die", die_exit, 0);




		std::function<bool()> default_exit = [this]() {
			return elapsed_time >= KNOCK_BACK_DURATION;
			};
		RegisterChangeRequest("stand_up_back", default_exit, 1);
	}
	void EnemyKnockBackState::OnEnter(IStateMachine* machine)
	{
		elapsed_time = 0.0f;
		animator->Play("enemy_down");
		auto enem_state_machine = dynamic_cast<EnemyStateMachine*>(machine);
		if (!enem_state_machine || !enem_state_machine->GetBrain())
			return;

		Vector3 knock_back_dir = enem_state_machine->GetBrain()->GetKnockBackVec();
		rb->velocity = knock_back_dir; // ノックバックベクトルを速度に直接設定する
		knock_back_dir.y = 0; // 水平方向のノックバックに限定する
		owner_enemy->transform->SetAxisZ(-knock_back_dir.getNormalized()); // ノックバックの方向に背を向けるように回転させる

	}
	void EnemyKnockBackState::OnExit(IStateMachine* machine)
	{
		if (chain_knock_collider) {
			chain_knock_collider->RemoveThisComponent();
			chain_knock_collider.reset();
		}
	}
	void EnemyKnockBackState::Update(IStateMachine* machine, float dt)
	{
		elapsed_time += dt;
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
	bool EnemyKnockBackState::CanTransitTo(const std::string& state_name)
	{
		if (state_name == "knock_back" || state_name == "knock_front")
			return true;
		return false;
	}
	void EnemyKnockBackState::OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info)
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
	void EnemyKnockBackState::RotateCollider()
	{
		static constexpr float ROTATION_DURATION = 0.3f; // コライダーの回転にかける時間（秒）
		static constexpr float ROTATION_START = 0.5f;		//<! ヒットストップを開始するタイミング（秒）
		static const Quaternion initial_collider_rotation = Quaternion(DEG2RAD(90.0f), { 0, 0, -1 });				//<! コライダーの初期回転の定数クォータニオン
		Quaternion target_rotation = initial_collider_rotation * Quaternion(DEG2RAD(-90.0f), { 0, 1, 0 }); // コライダーを-90度回転させるクォータニオン

		float t = std::clamp((elapsed_time - ROTATION_START) / ROTATION_DURATION, 0.0f, 1.0f);		// 経過時間に応じて回転を補間するためのパラメーター
		col->rotation = Slerp(initial_collider_rotation, target_rotation, t); // コライダーの回転を補間して更新する
	}

}