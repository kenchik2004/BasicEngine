#include "EnemyDieState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Scenes/NeonFade/SceneGame.h"

namespace NeonFade {
	EnemyDieState::EnemyDieState(Enemy* owner_enemy_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		owner_enemy = owner_enemy_;
		model = owner_enemy->model.lock().get();
		death_material = Enemy::death_material;
		std::function<bool()> default_exit = [this]() {
			return elapsed_time >= DIE_DURATION;
			};
		RegisterChangeRequest("idle", default_exit, 255);
	}
	void EnemyDieState::OnEnter(IStateMachine* machine)
	{
		elapsed_time = 0.0f;

		for (u32 i = 0; i < model->GetMaterialsNum(); i++)
		{
			model->SetMaterial(death_material, i);
		}
	}
	void EnemyDieState::OnExit(IStateMachine* machine)
	{
		//死んだら敵の数を減らす
		static_cast<SceneGame*>(owner_enemy->GetScene().get())->SubtractEnemyCount();

		SceneManager::Object::Destroy(owner_enemy->shared_from_this());
	}
	void EnemyDieState::Update(IStateMachine* machine, float dt)
	{
		elapsed_time += dt;
		if (Enemy::death_material && Enemy::death_material->GetGbufferPixelShader()) {
			auto death_shader = Enemy::death_material->GetGbufferPixelShader();
			// 経過時間を [0, 1] に正規化して死亡エフェクトの進行率を算出する。
			float death_value = physx::PxClamp(elapsed_time / DIE_DURATION, 0.0f, 1.0f);
			// 死亡エフェクトの速度パラメータを固定値で設定する。
			float death_speed = 3.0f;
			// シェーダの進行率パラメータへ書き込む。
			death_shader->SetValue("SampleCB.sampleparam1", &death_value);
			// シェーダの速度パラメータへ書き込む。
			death_shader->SetValue("SampleCB.sampleparam3", &death_speed);
		}
	}
	bool EnemyDieState::CanTransitTo(const std::string& state_name)
	{
		//死亡状態からはどの状態にも遷移できない
		return false;
	}
}