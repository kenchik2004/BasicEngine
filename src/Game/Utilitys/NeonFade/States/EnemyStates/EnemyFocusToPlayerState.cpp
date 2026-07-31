#include "EnemyFocusToPlayerState.h"

#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Components/EnemyController.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/EnemyRVOSystem.h"


#include "Game/Objects/NeonFade/GameObjectWithLifeTime.h"

namespace NeonFade {
	EnemyFocusToPlayerState::EnemyFocusToPlayerState(Enemy* owner_enemy_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		owner_enemy = owner_enemy_;
		player = owner_enemy->enem_controller->GetPlayer().lock().get();
		owner_transform = owner_enemy->transform.get();
		animator = owner_enemy->animator.lock().get();
		// デフォルトの遷移要求を登録する
		std::function<bool()> default_exit = [this]() {
			//一定時間経過後に自動的に待機状態に遷移する
			return focus_timer >= MAX_FOCUS_DURATION;
			};
		RegisterChangeRequest("idle", default_exit, 0);
	}
	void EnemyFocusToPlayerState::OnEnter(IStateMachine* machine)
	{
		//どのステートから遷移するか分からないが、とりあえずアイドルモーションを再生する
		animator->PlayIfNoSame("enemy_idle", true);

		//経過時間のリセット
		focus_timer = 0.0f;
	}
	void EnemyFocusToPlayerState::Update(IStateMachine* machine, float dt)
	{
		//ゆっくりとプレイヤーの方を向くだけ

		// 経過時間を更新
		focus_timer += dt;

		// プレイヤーの方向を向く処理
		Vector3 to_player = player->transform->position - owner_transform->position;
		to_player.normalize();

		// 内積を計算して、判定用に保持する
		cosine_to_player = owner_transform->AxisZ().dot(to_player);

		//回転を適用する
		EnemyRVOSystem::ApplyRotation(to_player, owner_transform, ROTATION_SPEED);
	}
	void EnemyFocusToPlayerState::OnExit(IStateMachine* machine)
	{
		if constexpr (false) {

			auto lifetime_obj = SceneManager::Object::Create<GameObjectWithLifeTime>(u8"electro_effect", 2.0f);
			lifetime_obj->transform->position = owner_transform->position + Vector3(0, 4.0f, 0);
			lifetime_obj->transform->rotation = owner_transform->rotation;
			auto effect_comp = lifetime_obj->AddComponent<EffectPlayer>("data/FX/ElectricSpark.efkefc");
			effect_comp->Play();
		}
	}
	bool EnemyFocusToPlayerState::CanTransitTo(const std::string& state_name)
	{
		if (state_name == "damage" || state_name == "knock_back" || state_name == "knock_front" || state_name == "die")
			return true;

		// プレイヤーの方向を向いたかどうかを判定する
		//軽量化のため、内積を使って角度を計算する
		static const float THRESHOLD_COSINE = std::cos(DEG2RAD(FOCUS_THRESHOLD_ANGLE));
		if (cosine_to_player >= THRESHOLD_COSINE)
		{
			return true;
		}
		return false;
	}
}
