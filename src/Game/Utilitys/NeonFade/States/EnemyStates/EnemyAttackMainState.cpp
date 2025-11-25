#include "EnemyAttackMainState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"

namespace NeonFade
{
	EnemyAttackMainState::EnemyAttackMainState(Enemy* owner_)
		:IState(static_cast<GameObject*>(owner_))
	{
		owner_enemy = owner_;
		animator = owner_enemy->animator.lock().get();
		std::function<void()> hit_box_attach = [this]() {
			if (hit_box) {
				hit_box->RemoveThisComponent();
				hit_box.reset();
			}
			auto hit_box_ = owner_enemy->AddComponent<SphereCollider>();
			hit_box_->is_trigger = true;
			hit_box_->SetLayer(Collider::Layer::Enemy);
			hit_box_->SetHitGroup(Collider::Layer::Player);
			hit_box_->radius = 5.0f;
			hit_box_->position = Vector3(0, 3.5f, -3.5f);
			hit_box = hit_box_;
			};
		std::function<void()> hit_box_remove = [this]() {
			if (hit_box) {
				hit_box->RemoveThisComponent();
				hit_box.reset();
			}
			};
		animator->SetAnimationCallBack("enemy_attack_main", hit_box_attach, 15, "hit_box_attach");
		animator->SetAnimationCallBack("enemy_attack_main", hit_box_remove, 30, "hit_box_remove");

	}
	void EnemyAttackMainState::OnEnter(IStateMachine* machine)
	{
		attack_timer = 0;
		animator->Play("enemy_attack_main");


	}
	void EnemyAttackMainState::OnExit(IStateMachine* machine)
	{
		if (hit_box) {
			hit_box->RemoveThisComponent();
			hit_box.reset();
		}
	}
	void EnemyAttackMainState::Update(IStateMachine* machine, float dt)
	{
		attack_timer += dt;

	}
	void EnemyAttackMainState::OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info)
	{
		if (auto player = hit_info.hit_collision->owner.lock()) {
			auto player_neonfade = SafeStaticCast<NeonFade::Player>(player);
			if (player_neonfade) {
				player_neonfade->Damage(10);
			}
		}
	}
}