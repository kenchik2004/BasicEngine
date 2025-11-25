#include "EnemyDieState.h"
#include "Game/Objects/NeonFade/Enemy.h"	
#include "Game/Scenes/NeonFade/SceneGame.h"
namespace NeonFade
{
	u32 instance_count = 0;
	EnemyDieState::EnemyDieState(Enemy* owner_)
		:IState(static_cast<GameObject*>(owner_))
	{
		instance_count++;
		owner_enemy = owner_;
		animator = owner_enemy->animator.lock().get();
		model = owner_enemy->model.lock().get();

	}
	EnemyDieState::~EnemyDieState()
	{
		instance_count--;
	}
	void EnemyDieState::OnEnter(IStateMachine* machine)
	{
		if (animator->GetCurrentAnimName() != "enemy_down")
			animator->Play("enemy_die", false, 0.0f, 0.2f, true);
		auto rb = owner_enemy->rb.lock().get();
		auto col = owner_enemy->col.lock().get();
		col->RemoveThisComponent();
		rb->RemoveThisComponent();
		for (u8 i = 0; i < 2; i++) {
			model->SetMaterial(Enemy::death_material, i);
		}
		exit_timer = 0;
	}
	void EnemyDieState::OnExit(IStateMachine* machine)
	{
	}
	void EnemyDieState::Update(IStateMachine* machine, float dt)
	{
		exit_timer += dt;
		if (Enemy::death_material && Enemy::death_material->GetGbufferPixelShader()) {
			auto death_shader = Enemy::death_material->GetGbufferPixelShader();
			float death_value = physx::PxClamp(exit_timer / EXIT_TIME, 0.0f, 1.0f);
			float death_speed = 3.0f;
			death_shader->SetValue("SampleCB.sampleparam1", &death_value);
			death_shader->SetValue("SampleCB.sampleparam3", &death_speed);
		}
		if (exit_timer > EXIT_TIME) {
			SafeSharedPtr<Enemy> owner_enemy_ = std::static_pointer_cast<Enemy>(owner_enemy->shared_from_this());

			//死んだら敵の数を減らす
			static_cast<SceneGame*>(owner_enemy->GetScene().get())->SubtractEnemyCount();
			SceneManager::Object::Destory(owner_enemy_);
			return;
		}
	}
}