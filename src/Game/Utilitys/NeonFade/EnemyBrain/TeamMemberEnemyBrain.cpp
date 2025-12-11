#include "TeamMemberEnemyBrain.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/LeaderEnemyBrain.h"

namespace NeonFade {

	TeamMemberEnemyBrain::TeamMemberEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_)
		:AbstractEnemyBrain(state_machine_, player_)
	{

	}

	void TeamMemberEnemyBrain::Think()
	{
		const float dt = Time::UnscaledDeltaTime();
		if (i_frame_timer > 0.0f)
			i_frame_timer -= dt;

		bool knock_back_bool = knock_back;
		bool damage_bool = is_damaged;
		state_machine->is_damaged = damage_bool;
		state_machine->knock_back = knock_back_bool;
		if (hp != 0) {
			if (!damage_bool && !knock_back_bool)
			{

				auto& brain = leader->GetTeamData();
				const Vector3& trns_pos = state_machine->enemy->transform->position;

				Vector3 desired = Vector3(0, 0, 0);

				// 1) チーム全体の移動方向（重み高）
				desired += brain.team_mov_vec.getNormalized();

				// 2) コヒージョン：隊の中心へ軽く寄る（重み中）
				Vector3 to_center = brain.team_position - trns_pos;
				to_center.y = 0;   // 水平方向のみ
				float dist = to_center.magnitude();
				if (dist > 5.0f) {
					desired += to_center.getNormalized() * 0.5f;
				}
				desired = { 0,0,0 };
				// 3) 自分のフォーメーション位置へ移動（重み強）
				Quaternion rot = Quaternion(DEG2RAD(number_in_team * (360 / brain.member_num)), Vector3(0, 1, 0));
				Vector3 slot_pos = brain.team_position + rot.rotate(Vector3(0, 0, 5));

				Vector3 to_slot = slot_pos - trns_pos;
				to_slot.y = 0;   // 水平方向のみ
				float slot_dist = to_slot.magnitude();

				float slowRadius = 3.0f;   // 減速開始距離
				float stopRadius = 1.0f;   // 実際に止める距離

				Vector3 slotForce = Vector3(0, 0, 0);

				if (slot_dist > stopRadius)
				{
					Vector3 dir = to_slot / slot_dist;

					// 減速係数 (近づくほど0に近づく)
					float factor = slot_dist < slowRadius ? (slot_dist / slowRadius) : 1.0f;

					slotForce = dir * factor;   // ←これがArrive
				}
				else
				{
					// スロット付近では完全停止
					slotForce = { 0,0.1f,0 };
				}

				desired += slotForce;

				// 最終方向の正規化
				Vector3 move = desired.getNormalized();

				// 出力速度（お好みで）
				input_vec = move * 5.0f;


			}
		}
		if (state_machine->enemy->transform->position.y < -50.0f)
		{
			Die();
		}

		state_machine->move_vec = input_vec;
		state_machine->is_attacking = false;
		state_machine->is_dead = hp == 0;
		knock_back = false;
		is_damaged = false;

	}

	void TeamMemberEnemyBrain::AddLeader(LeaderEnemyBrain* new_leader)
	{
		if (!new_leader)
			return;
		number_in_team = new_leader->AddMember(this);
		leader = new_leader;
	}

	void TeamMemberEnemyBrain::Damage(u32 damage, bool ignore_i_frame)
	{
		if (!ignore_i_frame && i_frame_timer > 0.0f)
			return;
		if (damage >= hp)
		{
			Die();
			return;
		}
		hp -= damage;
		i_frame_timer = I_FRAME;
		is_damaged = true;
	}

	void TeamMemberEnemyBrain::KnockBack(Vector3 knock_back_vec)
	{
		input_vec = knock_back_vec;
		knock_back = true;
		state_machine->move_vec = input_vec;
	}
	void TeamMemberEnemyBrain::DebugDraw()
	{
		DrawHPDebug();
		if (leader)
			DxLib::DrawLine3D(cast(state_machine->enemy->transform->position + Vector3(0, 4, 0)), cast(leader->GetTeamData().team_position + Vector3(0, 4, 0)), Color::ORANGE);
	}
	void TeamMemberEnemyBrain::Die() {
		hp = 0;
		leader->SubstractMember(this);
	}

}