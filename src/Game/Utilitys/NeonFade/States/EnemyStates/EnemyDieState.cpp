#include "EnemyDieState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/GameObjectWithLifeTime.h"
#include "Game/Scenes/NeonFade/SceneGame.h"

namespace NeonFade {

	static const std::array<std::string, 5> down_animation_names = { "enemy_down_forward","enemy_down","enemy_damage_crowling","enemy_die" ,""};

	EnemyDieState::EnemyDieState(Enemy* owner_enemy_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		owner_enemy = owner_enemy_;
		model = owner_enemy->model.lock().get();
		animator = owner_enemy->animator.lock().get();
		collider = owner_enemy->col.lock().get();
		rb = owner_enemy->rb.lock().get();
		death_material = Enemy::death_material;
		std::function<bool()> default_exit = [this]() {
			return elapsed_time >= DIE_DURATION;
			};
		RegisterChangeRequest("idle", default_exit, 255);
	}
	void EnemyDieState::OnEnter(IStateMachine* machine)
	{
		elapsed_time = 0.0f;
		if (collider) {
			collider->RemoveThisComponent();
			collider = nullptr;
			rb->is_kinematic = true;
		}
		for (u32 i = 0; i < model->GetMaterialsNum(); i++)
		{
			model->SetMaterial(death_material, i);
		}
		//立ったまま死なないよう、ダウンアニメーションを再生する
		auto cur_anim_name = animator->GetCurrentAnimName();
		if (std::find(down_animation_names.begin(), down_animation_names.end(), cur_anim_name) == down_animation_names.end())
		{
			animator->Play("enemy_die");
		}

		//死亡時に故障したようなエフェクトを召喚
		{
			auto fx_obj = SceneManager::Object::Create<GameObjectWithLifeTime>(owner_enemy->GetScene(), 5.0f);
			fx_obj->transform->position = owner_enemy->transform->position;
			fx_obj->transform->scale = { 0.5f,0.5f,0.5f };
			fx_obj->AddComponent<EffectPlayer>(u8"data/FX/electro_shock.efkefc")->Play();
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