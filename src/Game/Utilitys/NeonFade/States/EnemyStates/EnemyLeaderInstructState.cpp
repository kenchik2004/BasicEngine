#include "EnemyLeaderInstructState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Components/EnemyController.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/LeaderEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/EnemyTeam.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/EnemyRVOSystem.h"

namespace NeonFade
{

	EnemyLeaderInstructState::EnemyLeaderInstructState(Enemy* owner_enemy_, LeaderEnemyBrain* brain_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		// オーナーのEnemyオブジェクトへのポインタを保持
		owner_enemy = owner_enemy_;
		// リーダーのAI脳へのポインタを保持
		brain = brain_;

		// プレイヤーオブジェクトへのポインタを取得
		player = owner_enemy->enem_controller->GetPlayer().lock().get();


		animator = owner_enemy->animator.lock().get();
		std::function<bool()> default_exit = [this]() {
			return elapsed_time >= INSTRUCT_DURATION;
			};
		RegisterChangeRequest("idle", default_exit, 1);
	}

	void EnemyLeaderInstructState::OnEnter(IStateMachine* machine)
	{
		elapsed_time = 0.0f;
		animator->PlayIfNoSame("enemy_instruct");
	}
	void EnemyLeaderInstructState::OnExit(IStateMachine* machine)
	{}
	void EnemyLeaderInstructState::Update(IStateMachine* machine, float dt)
	{
		elapsed_time += dt;
		InstructMembers();
		if (!player || !owner_enemy)
			return;
		Vector3 mov_dir = player->transform->position - owner_enemy->transform->position;
		EnemyRVOSystem::ApplyRotation(mov_dir, owner_enemy->transform.get(), ROTATION_SPEED);
	}
	bool EnemyLeaderInstructState::CanTransitTo(const std::string& state_name)
	{
		if (state_name == "damage" || state_name == "knock_back" || state_name == "knock_front")
			return true;
		return false;
	}

	void EnemyLeaderInstructState::InstructMembers()
	{
		if (elapsed_time < INSTRUCT_START || !brain)
			return;
		{
			auto team = brain->GetTeam();
			if (team) {
				const auto& members = team->GetMembers();
				for (const auto& member : members) {
					if (member && !member->IsDead())
						member->GoToAttack();
				}

			}
		}
	}
}
