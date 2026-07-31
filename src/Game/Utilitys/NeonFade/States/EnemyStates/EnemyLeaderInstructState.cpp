#include "EnemyLeaderInstructState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/GameObjectWithLifeTime.h"
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

		//誰がリーダーなのか分かりづらいので、リーダーの頭上に注意マークのエフェクトを表示する
		{
			auto caution_effect_obj = SceneManager::Object::Create<GameObjectWithLifeTime>(u8"caution_effect", 2.0f);
			auto caution_effect = caution_effect_obj->AddComponent<EffectPlayer>("data/FX/CautionMark.efkefc");
			//リーダーの頭上にエフェクトを表示する
			Vector3 effect_offset = { 0.0f, 8.0f, 0.0f };
			caution_effect_obj->transform->position = owner_enemy->transform->position + effect_offset;
			caution_effect->Play();
		}
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
			// チームが解散されている場合は、指示を出さない
			if (!team)
				return;

			//確率で、取り囲むか攻撃するかを決定する
			bool instruct_siege = Random::Int(1, 100) <= INSTRUCT_SIEGE_CHANCE; // 指示する行動をランダムに決定する

			// チームのメンバーに指示を出す
			//メンバーを取得
			const auto& members = team->GetMembers();

			//全メンバーに対し指示を出す
			for (const auto& member : members) {
				//nullチェックと死亡チェック
				if (member && !member->IsDead()) {

					// メンバーに指示を出す
					//取り囲むor攻撃する
					if (instruct_siege) {
						//取り囲む指示を出す
						member->SiegePlayer();
					}
					else {
						//攻撃する指示を出す
						member->GoToAttack();
					}
				}
			}


		}
	}
}
