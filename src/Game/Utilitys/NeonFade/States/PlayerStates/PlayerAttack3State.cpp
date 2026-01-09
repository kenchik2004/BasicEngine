#include "PlayerAttack3State.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Utilitys/NeonFade/StateMachines/PlayerStateMachine.h"
#include "Game/Objects/NeonFade/GameObjectWithLifeTime.h"
#include "Game/Components/PlayerCameraMachine.h"

namespace NeonFade {
	PlayerAttack3State::PlayerAttack3State(Player* player_)
		:IState(static_cast<GameObject*>(player_))
	{
		owner_player = player_;
		rb = player_->rb.lock().get();
		animator = player_->animator.lock().get();
		std::function<void()> create_hit_box = [this]() {
			auto col = owner_player->AddComponent<BoxCollider>
				(Vector3(0, -3, 3.5f), Quaternion(DEG2RAD(90), { 0,1,0 }), Vector3(2, 2, 9),
					true, Collider::Layer::Wepon, Collider::Layer::Enemy);
			hit_box = std::move(col);
			hit_box_created_time = exit_timer;
			};
		//ヒットボックス生成タイミング
		animator->SetAnimationCallBack("leg_sweep", create_hit_box, 27, "create_hit_box");

		std::function<bool()> default_exit =
			[this]() {
			return exit_timer >= EXIT_TIME;
			};
		RegisterChangeRequest("idle", default_exit, 0);
	}
	void PlayerAttack3State::OnEnter(IStateMachine* machine)
	{
		exit_timer = 0.0f;
		animator->Play("leg_sweep", true, 0, 0);
		animator->anim_speed = 1.5f;
		rb->velocity *= 0.3f;
		hit_stop_timer = 0.0f;
	}
	void PlayerAttack3State::Update(IStateMachine* machine, float dt)
	{
		exit_timer += dt;

		if (hit_stop_timer > 0.0f) {
			hit_stop_timer -= dt;
			if (hit_stop_timer <= 0.0f)
				animator->anim_speed = 1.5f;
		}
		if (hit_box) {
			hit_box->rotation = Slerp(Quaternion(DEG2RAD(90), { 0,1,0 }), Quaternion(DEG2RAD(-90), { 0,1,0 }), (exit_timer - hit_box_created_time) / (EXIT_TIME - hit_box_created_time));
		}
	}
	void PlayerAttack3State::OnExit(IStateMachine* machine)
	{
		if (hit_box)
		{
			hit_box->RemoveThisComponent();
			hit_box = nullptr;
		}
		animator->anim_speed = 1.0f;
	}
	void PlayerAttack3State::OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info)
	{
		if (hit_info.collision == hit_box)
		{
			if (auto enemy = SafeDynamicCast<Enemy>(hit_info.hit_collision->owner.lock()))
			{
				Vector3 knockback_dir = enemy->transform->position - owner_player->transform->position;
				knockback_dir.normalize();
				knockback_dir.y = 2;
				knockback_dir *= 10;
				enemy->Damage(5);
				enemy->Down(knockback_dir);
			}
			if (hit_stop_timer <= 0.0f) {

				owner_player->player_camera_machine->ShakeCamera(0.2f, HIT_STOP_TIME);
				hit_stop_timer = HIT_STOP_TIME;
				animator->anim_speed = 0.01f;

				{
					auto eff = SceneManager::Object::Create<GameObjectWithLifeTime>(u8"effect_attack1_hit", 1.0f);
					eff->transform->position = hit_info.hit_collision->owner->transform->position;
					eff->transform->position.y += 1.0f;
					auto eff_comp = eff->AddComponent<EffectPlayer>(u8"data/FX/KOKUSEN.efk");
					//eff_comp->SetSpeed(0.2f);
					eff_comp->Play();
				}
			}
		}
	}
}