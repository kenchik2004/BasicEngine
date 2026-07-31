//---------------------------------------------------------------------------
//! @file   PlayerAttack3State.cpp
//! @brief  PlayerAttack3Stateの実装。プレイヤーの第3攻撃状態の処理を行う
//---------------------------------------------------------------------------
#include "PlayerAttack3State.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Utilitys/NeonFade/StateMachines/PlayerStateMachine.h"
#include "Game/Objects/NeonFade/GameObjectWithLifeTime.h"
#include "Game/Components/PlayerCameraMachine.h"

namespace NeonFade {
	//! @brief 第3攻撃状態を構築し、参照コンポーネントとヒット判定生成コールバックを設定する。
	//! @param player_ この状態を所有するプレイヤーオブジェクト。
	PlayerAttack3State::PlayerAttack3State(Player* player_)
		:IState(static_cast<GameObject*>(player_))
	{
		// 所有プレイヤーと使用コンポーネントへの参照を保持する。
		owner_player = player_;
		rb = player_->rb.lock().get();
		animator = player_->animator.lock().get();
		// アニメーションの特定フレームで回転蹴り用ヒットボックスを生成する。
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
	//! @brief 第3攻撃開始時の初期化（アニメ再生・速度調整・停止演出リセット）。
	//! @param machine ステートマシン本体。
	void PlayerAttack3State::OnEnter(IStateMachine* machine)
	{
		exit_timer = 0.0f;
		animator->Play("leg_sweep", true, 0, 0);
		animator->anim_speed = 1.5f;
		rb->velocity *= 0.3f;
		hit_stop_timer = 0.0f;
		stop_counter = 0;
	}
	//! @brief 攻撃更新処理。ヒットストップ制御とヒットボックス回転更新を行う。
	//! @param machine ステートマシン本体。
	//! @param dt 前フレームからの経過時間。
	void PlayerAttack3State::Update(IStateMachine* machine, float dt)
	{

		if (hit_stop_timer > 0.0f) {
			hit_stop_timer -= dt;
			if (hit_stop_timer <= 0.0f)
				animator->anim_speed = 1.5f;
		}
		else
			exit_timer += dt;
		if (hit_box) {
			hit_box->rotation = Slerp(Quaternion(DEG2RAD(90), { 0,1,0 }), Quaternion(DEG2RAD(-90), { 0,1,0 }), (exit_timer - hit_box_created_time) / (EXIT_TIME - 0.1f - hit_box_created_time));
		}
	}
	//! @brief 攻撃終了時にヒットボックスとアニメ速度を後始末する。
	//! @param machine ステートマシン本体。
	void PlayerAttack3State::OnExit(IStateMachine* machine)
	{
		if (hit_box)
		{
			hit_box->RemoveThisComponent();
			hit_box = nullptr;
		}
		animator->anim_speed = 1.0f;
	}
	//! @brief 攻撃ヒット時にダメージ・吹き飛ばし・ヒットストップ演出を適用する。
	//! @param machine ステートマシン本体。
	//! @param hit_info トリガー衝突情報。
	void PlayerAttack3State::OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info)
	{
		if (hit_info.collision == hit_box)
		{
			if (auto enemy = SafeDynamicCast<Enemy>(hit_info.hit_collision->owner.lock()))
			{
				Vector3 knockback_dir = enemy->transform->position - owner_player->transform->position;
				knockback_dir.normalize();
				knockback_dir *= KNOCK_BACK_FORCE;
				knockback_dir.y = KNOCK_BACK_UP_FORCE;
				enemy->Damage(DAMAGE);
				enemy->Down(knockback_dir);
			}
			if (hit_stop_timer <= 0.0f && stop_counter < MAX_STOP_COUNT) {
				stop_counter++;
				owner_player->player_camera_machine->ShakeCamera(CAMERA_SHAKE_INTENSITY, CAMERA_SHAKE_TIME);
				hit_stop_timer = HIT_STOP_TIME;
				animator->anim_speed = 0.001f;

				{
					auto eff = SceneManager::Object::Create<GameObjectWithLifeTime>(u8"effect_attack1_hit", 1.0f);
					eff->transform->position = hit_info.hit_collision->owner->transform->position;
					eff->transform->position.y += 1.0f;
					auto eff_comp = eff->AddComponent<EffectPlayer>(u8"data/FX/KOKUSEN.efkefc");
					//eff_comp->SetSpeed(0.2f);
					eff_comp->Play();
				}
			}
		}
	}
}