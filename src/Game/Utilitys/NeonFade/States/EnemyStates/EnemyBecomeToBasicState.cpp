#include "EnemyBecomeToBasicState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/GameObjectWithLifeTime.h"
#include "Game/Components/EnemyController.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/BasicEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/LeaderEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/TeamMemberEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/EnemyTeam.h"

namespace NeonFade {


	EnemyBecomeToBasicState::EnemyBecomeToBasicState(Enemy* owner_enemy_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		owner_enemy = owner_enemy_;
		auto player_wp = owner_enemy->enem_controller->GetPlayer();
		PlayerWP player_wp_cast = SafeStaticCast<Player>(player_wp.lock());
		player = player_wp_cast;
		model = owner_enemy->model.lock().get();

		/// デフォルトの遷移要求を登録する
		auto def_exit_func = [this]() {
			return true;
			};
		RegisterChangeRequest("idle", def_exit_func, 0);
	}

	bool EnemyBecomeToBasicState::CanTransitTo(const std::string& state_name)
	{
		return false;
	}

	void EnemyBecomeToBasicState::OnExit(IStateMachine* machine)
	{
		auto new_brain = make_safe_unique<BasicEnemyBrain>(static_cast<EnemyStateMachine*>(machine), player);
		owner_enemy->enem_controller->SetBrain(std::move(new_brain));
		if (model)
			model->SetModel("enemy_model");
	}


	EnemyBecomeToLeaderState::EnemyBecomeToLeaderState(Enemy* owner_enemy_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		owner_enemy = owner_enemy_;
		auto player_wp = owner_enemy->enem_controller->GetPlayer();
		PlayerWP player_wp_cast = SafeStaticCast<Player>(player_wp.lock());
		player = player_wp_cast;
		model = owner_enemy->model.lock().get();
		animator = owner_enemy->animator.lock().get();

		/// デフォルトの遷移要求を登録する
		auto def_exit_func = [this]() {
			return timer > BECOME_TO_LEADER_TIME;
			};
		RegisterChangeRequest("idle", def_exit_func, 0);
	}
	void EnemyBecomeToLeaderState::OnExit(IStateMachine* machine)
	{
		auto team_member_brain = dynamic_cast<TeamMemberEnemyBrain*>(owner_enemy->enem_controller->GetBrain());
		EnemyTeam* team = team_member_brain ? team_member_brain->GetTeam() : nullptr;
		auto new_brain = make_safe_unique<LeaderEnemyBrain>(static_cast<EnemyStateMachine*>(machine), player, team);
		owner_enemy->enem_controller->SetBrain(std::move(new_brain));
	}

	void EnemyBecomeToLeaderState::OnEnter(IStateMachine* machine)
	{
		timer = 0.0f;
		auto effect_obj = SceneManager::Object::Create<GameObjectWithLifeTime>("effect", 2.0f);
		effect_obj->transform->position = owner_enemy->transform->position;
		effect_obj->transform->scale = {0.6f, 0.6f, 0.6f};
		auto effect_comp = effect_obj->AddComponent<EffectPlayer>(u8"data/FX/Power_Up.efkefc");
		effect_comp->Play();
	}

	void EnemyBecomeToLeaderState::Update(IStateMachine* machine, float dt)
	{
		timer += dt;
		if (timer > BECOME_TO_LEADER_TIME * 0.5f)
		{
			model->SetModel("enemy_leader_model");
			animator->Play("idle");

		}
	}

	bool EnemyBecomeToLeaderState::CanTransitTo(const std::string& state_name)
	{
		return false;
	}


}