#include "PlayerController.h"
#include "Game/Utilitys/NeonFade/StateMachines/PlayerStateMachine.h"
#include "Game/Objects/NeonFade/Player.h"


namespace NeonFade
{

	int PlayerController::Init()
	{
		owner_player = SafeStaticCast<Player>(owner.lock());
		state_machine = make_safe_unique<PlayerStateMachine>(owner_player.get());

		state_machine->ChangeState("idle");
		return Super::Init();
	}

	void PlayerController::Update()
	{
		//-------------------------------------------------
		//TODO カメラの向きを基準に移動ベクトルを計算
		Transform* cam_trns = owner_player->player_camera->transform.get();
		//-------------------------------------------------
		Vector2 pad_left = Input::GetPadLeftStick(0);
		move_input = { 0,0,0 };
		// 十分に倒されていなかったらキーボード入力を使う
		if (pad_left.magnitudeSquared() <= FLT_EPSILON) {
			if (Input::GetKey(KeyCode::W))
				move_input -= cam_trns->AxisZ();
			if (Input::GetKey(KeyCode::S))
				move_input += cam_trns->AxisZ();
			if (Input::GetKey(KeyCode::A))
				move_input += cam_trns->AxisX();
			if (Input::GetKey(KeyCode::D))
				move_input -= cam_trns->AxisX();
		}
		else {
			move_input += cam_trns->AxisZ() * -pad_left.y;
			move_input += cam_trns->AxisX() * -pad_left.x;
		}
		move_input = ProjectOnPlane(move_input, { 0,1,0 });



		if (is_landed && (Input::GetKeyDown(KeyCode::Space) || Input::GetPadButtonDown(0, PadButton::Button1))) {
			fly = false;
			is_jumping = true;
			is_landed = false;
		}
		else if (is_landed) {
			is_jumping = false;
			is_falling = false;
		}

		climb_ray_start = owner_player->transform->position + owner_player->transform->AxisZ() * -1.5f + Vector3(0, 12, 0);
		Vector3 climb_ray_dir = Vector3(0, -1, 0);
		RayCastInfo climb_ray_info;

		physx::PxQueryFilterData filterdata;
		filterdata.data.word0 = Collider::Layer::Terrain;
		filterdata.flags = physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::ePREFILTER;

		owner->GetScene()->RayCast(Ray(climb_ray_start, climb_ray_dir, 5.0f), climb_ray_info, filterdata);
		if (climb_ray_info.hasBlock) {
			climb_hit_normal = climb_ray_info.block.normal;
			climb_hit_point = climb_ray_info.block.position;
			float climb_hit_distance = climb_ray_info.block.distance;
			can_climb = (climb_ray_info.hasBlock && climb_hit_distance >= 2.0f && climb_hit_normal.dot({ 0,1,0 }) > 0.8f);
			state_machine->climb_start_pos = owner_player->transform->position;
			state_machine->climb_end_pos = climb_hit_point + Vector3(0, 1.5f, 0);
		}
		else
			can_climb = false;

		fall_detect_time += Time::DeltaTime();
		if (can_climb && move_input.magnitudeSquared() > FLT_EPSILON)
			is_climbing = true;
		if (is_climbing) {
			is_jumping = false;
			fall_detect_time = 0.0f;
		}
		is_falling = (fall_detect_time >= FALL_DETECT_THRESHOLD);
		if (is_falling)
			is_jumping = false;
		is_attacking = Input::GetKeyDown(KeyCode::L) || Input::GetPadButtonDown(0, PadButton::Fuga);
		state_machine->is_attacking = is_attacking;
		state_machine->move_input = move_input;
		state_machine->is_jumping = is_jumping;
		state_machine->fly = fly;
		state_machine->is_falling = is_falling;
		state_machine->can_climb = can_climb;
		state_machine->Update(Time::DeltaTime());
	}

	void PlayerController::Exit()
	{
	}
	float gravity_param = 1.0f;
	void PlayerController::LateDebugDraw()
	{
		DrawLine3D(cast(climb_ray_start), cast(climb_ray_start + Vector3(0, -5.0f, 0)), 0xff00ff);
		if (climb_hit_point != Vector3(0, 0, 0)) {
			DrawSphere3D(cast(climb_hit_point), 0.5f, 16, can_climb ? 0xffff00 : 0x111111, 0x0000, true);
			DrawLine3D(cast(climb_hit_point), cast(climb_hit_point + climb_hit_normal * 5), 0xff0000);

		}
		state_machine->DebugDraw();
		printfDx("Gravity: %.1f", gravity_param);
	}
	void PlayerController::PrePhysics()
	{
		is_landed = false;
		if (Input::GetKey(KeyCode::Alpha9)) {
			gravity_param -= Time::DeltaTime();
			owner->GetScene()->GetPhysicsScene()->setGravity(Vector3(0, -9.81f, 0) * gravity_param);
		}

		if (Input::GetKey(KeyCode::Alpha0)) {

			gravity_param += Time::DeltaTime();
			owner->GetScene()->GetPhysicsScene()->setGravity(Vector3(0, -9.81f, 0) * gravity_param);
		}
	}
	void PlayerController::OnCollisionEnter(const HitInfo& hit_info)
	{

		if (hit_info.hit_collision->GetLayer() == Collider::Layer::Terrain) {
			is_landed = true;
			if (is_climbing)
				is_climbing = false;
		}
		state_machine->OnCollisionEnter(hit_info);
	}
	void PlayerController::OnCollisionStay(const HitInfo& hit_info)
	{
		if (hit_info.hit_collision->GetLayer() == Collider::Layer::Terrain) {
			if (owner_player->rb->velocity.y <= 1) {
				is_landed = true;
				fall_detect_time = 0.0f;
			}
		}
		state_machine->OnCollisionStay(hit_info);
	}
	void PlayerController::OnCollisionExit(const HitInfo& hit_info)
	{
		if (hit_info.hit_collision->GetLayer() == Collider::Layer::Terrain) {
			is_landed = false;
		}
		state_machine->OnCollisionExit(hit_info);
	}

	void PlayerController::OnTriggerEnter(const HitInfo& hit_info)
	{
		state_machine->OnTriggerEnter(hit_info);
	}

	void PlayerController::OnTriggerStay(const HitInfo& hit_info)
	{
		state_machine->OnTriggerStay(hit_info);
	}

	void PlayerController::OnTriggerExit(const HitInfo& hit_info)
	{
		state_machine->OnTriggerExit(hit_info);
	}

} // namespace NeonFade
