//---------------------------------------------------------------------------
//! @file   LeaderEnemyBrain.cpp
//! @brief  LeaderEnemyBrainの実装。リーダー敵のAI思考ロジックを実装する
//---------------------------------------------------------------------------
#include "LeaderEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/TeamMemberEnemyBrain.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"
#include "Game/Components/EnemyController.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/EnemyTeam.h"

#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyDropKickAttackEntryState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyLeaderInstructState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyRandomWalkState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyBecomeToBasicState.h"

namespace NeonFade
{
	LeaderEnemyBrain::LeaderEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_, EnemyTeam* team)
		:AbstractEnemyBrain(state_machine_, player_)
	{
		if (team && !my_team) {
			team->SetLeader(this);
		}
		hp = MAX_HP;

		if (!state_machine || !state_machine->enemy)
			return;

		//基礎的なステートは親クラスで登録するので、ここでは追加のステートのみを登録する

		//ドロップキック攻撃ステート
		auto atk_1_state = make_safe_unique<EnemyDropKickAttackEntryState>(state_machine->enemy);
		state_machine->AddState("drop_kick", std::move(atk_1_state));

		//攻撃指示ステート
		auto instruct_state = make_safe_unique<EnemyLeaderInstructState>(state_machine->enemy, this);
		state_machine->AddState("instruct", std::move(instruct_state));

		//ランダムウォークステート
		auto random_walk_state = make_safe_unique<EnemyRandomWalkState>(state_machine->enemy);
		state_machine->AddState("random_walk", std::move(random_walk_state));

		//リーダーがチームを失った場合に通常の敵AIに戻るステート
		auto become_basic_state = make_safe_unique<EnemyBecomeToBasicState>(state_machine->enemy);
		state_machine->AddState("become_basic", std::move(become_basic_state));

		// 初期状態はidleに設定する
		state_machine->ChangeState("idle");


		//リーダーは専用モデルを使用する
		{
			auto model = state_machine->enemy->model;
			if (model->model_name != "enemy_leader_model")
				model->SetModel("enemy_leader_model");
		}




	}
	LeaderEnemyBrain::~LeaderEnemyBrain()
	{

		//自身をチームから削除する
		if (my_team)
			my_team->SetLeader(nullptr);
	}
	std::string LeaderEnemyBrain::Think()
	{
		i_frame_timer -= Time::DeltaTime();
		instruct_cooldown_timer -= Time::DeltaTime();
		std::string result = __super::Think();

		// もし親クラスのThinkで遷移先が決まっていれば、その遷移先を返す
		if (result != "") {
			return result;
		}

		{
			Vector3 player_pos = player->transform->position;
			Vector3 my_pos = body->transform->position;
			Vector3 to_player = player_pos - my_pos;
			if (to_player.magnitudeSquared() < 50.0f * 50.0f)
			{
				result = "atk_1";
			}
			else if (to_player.magnitudeSquared() < 100.0f * 100.0f)
			{
				if (instruct_cooldown_timer <= 0.0f)
				{
					result = "instruct";
					instruct_cooldown_timer = INSTRUCT_COOLDOWN;
				}
			}
			else
			{
				result = "random_walk";
			}
#if 1
			if (Input::GetKeyDown(KeyCode::K))
			{
				result = "instruct";
			}
#endif
			if (!my_team)
				result = "become_basic";
		}


		ResetFrameParameters();
		return result;

	}

	void LeaderEnemyBrain::Die()
	{
		__super::Die();
		// チームを解散するか、新しいリーダーを選出する
		ReleaseTeamOrSelectNewLeader();
	}

	void LeaderEnemyBrain::ResetTeamRef()
	{
		my_team = nullptr;
	}

	void LeaderEnemyBrain::SetTeamRef(EnemyTeam* team)
	{
		my_team = team;
	}

	void LeaderEnemyBrain::ReleaseTeamOrSelectNewLeader()
	{
		//既に解散済みなら何もしない
		if (!my_team)
			return;


		//壊滅状態(メンバーが一定数以下)の場合は、チームを解散する
		if (my_team->GetMemberNum() <= TEAM_RELEASE_MEMBER_NUM) {
			my_team->ClearTeam();
		}
		//チームがまだ成立しうる(=メンバーがまだ残っている)場合は、ランダムで新しいリーダーを選出する
		else {
			//ランダムなインデックスを生成して、メンバーの中から新しいリーダーを選出する
			auto members = my_team->GetMembers();
			u32 rand_idx = Random::Int(0, static_cast<int>(members.size() - 1));

			//次は君だ
			members[rand_idx]->BecomeLeader();
			my_team->SetLeader(nullptr); //チームから自分を削除する
		}

	}

}