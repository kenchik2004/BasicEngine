#include "Player.h"
#include "System/Components/RigidBody.h"
#include "System/Components/ModelRenderer.h"
#include "System/Components/Animator.h"
#include "Game/Components/PlayerController.h"
#include "System/Objects/CameraObject.h"


namespace NeonFade
{
	int Player::Init()
	{
		rb = AddComponent<RigidBody>();
		rb->GetBody()->is<physx::PxRigidDynamic>()->setSleepThreshold(0.0f);

		auto model_obj = SceneManager::Object::Create<GameObject>("pl_model");
		model_obj->transform->SetParent(transform);
		model_obj->transform->scale = { 0.05f,0.05f,0.05f };
		model_obj->transform->local_position = { 0,-4.5f,0 };
		model_obj->transform->local_rotation = Quaternion(DEG2RAD(180), { 0,1,0 });

		model = model_obj->AddComponent<ModelRenderer>();
		animator = model_obj->AddComponent<Animator>();

		model->SetModel("player_model");
		animator->SetAnimation("idle", 1);
		animator->SetAnimation("walk", 1);
		animator->SetAnimation("run", 1);
		animator->SetAnimation("sprint", 0);
		animator->SetAnimation("jump", 0);
		animator->SetAnimation("fall_loop", 0);
		animator->SetAnimation("combat_combo", 0);
		animator->SetAnimation("climb", 0);
		animator->SetAnimation("jump_attack", 0);
		animator->SetAnimation("back_flip", 0);
		animator->SetAnimation("player_damage", 0);
		animator->SetAnimation("smash_charge", 0);
		animator->SetAnimation("smash_attack", 0);
		animator->SetAnimation("smash_finish", 0);
		animator->SetAnimation("spin", 0);
		animator->SetAnimation("spin_kick", 0);
		animator->SetAnimation("clouch_inv", 0);
		animator->SetAnimation("leg_sweep", 0);


		rb->freeze_rotation = { 1,1,1 };
		auto col = AddComponent<CapsuleCollider>();
		col->height = 5.7f;
		col->radius = 1.5f;
		//col->position = { 4.5f,0,0 };
		col->rotation = Quaternion(DEG2RAD(90), { 0,0,1 });
		col->SetLayer(Collider::Layer::Player);
		pl_controller = AddComponent<PlayerController>();
		//auto foot_col = AddComponent<BoxCollider>();
		//foot_col->extension = { 1.4f,0.5f,1.4f };
		//foot_col->position = { 0,0.0f,0 };
		//foot_col->SetLayer(Collider::Layer::Player);
		//foot_col->SetHitGroup(Collider::Layer::Terrain);
		//foot = foot_col;
		//AddComponent<AudioListener>();

		return 0;
	}
	void Player::Update()
	{
	}
	void Player::PreDraw()
	{

		{
			Vector2 pad_input = Input::GetPadRightStick(0);
			if (pad_input.magnitudeSquared() >= FLT_EPSILON) {
				camera_rot.y += pad_input.x * Time::DrawDeltaTime() * 90 * 3;
				camera_rot.x += pad_input.y * Time::DrawDeltaTime() * 90 * 3;
				camera_rot.x = physx::PxClamp(camera_rot.x, -45.0f, 30.0f);
			}
			else {
				if (Input::GetKey(KeyCode::Left))
					camera_rot.y -= Time::DrawDeltaTime() * 90;
				if (Input::GetKey(KeyCode::Right))
					camera_rot.y += Time::DrawDeltaTime() * 90;
			}


			auto cam_lock = player_camera.lock();
			Transform* cam_trns = cam_lock->transform.get();
			Quaternion cam_rot_around = Quaternion(DEG2RAD(camera_rot.y), { 0,1,0 }) * Quaternion(DEG2RAD(camera_rot.x), { 1,0,0 });
			Vector3 cam_vector_z = { 0,0,1 };
			cam_vector_z = cam_rot_around.rotate(cam_vector_z);
			physx::PxRaycastHit hits[2];
			RayCastInfo info(hits, 2);
			camera_distance = camera_distance_max;
			physx::PxQueryFilterData a;
			a.data.word0 = Collider::Layer::Terrain;
			a.flags = physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::ePREFILTER;
			GetScene()->RayCast(Ray{ transform->position + Vector3(0,2,0), -cam_vector_z, camera_distance }, info, a);
			if (info.hasBlock) {
				camera_distance = info.block.distance - 0.05f;
			}
			cam_vector_z = cam_vector_z * camera_distance;
			cam_trns->position = transform->position - cam_vector_z + Vector3(0, 2, 0);
			cam_trns->SetAxisZ(cam_vector_z);
		}
	}
	void Player::LateDebugDraw()
	{
		return;
		if (model) {
			auto dx_mat = model->GetFrameWorldMatDX(9);
			auto px_mat = model->GetFrameWorldMat(9);
			printfDx("Frame 10 World Mat DX:\n");
			for (int i = 0; i < 4; i++) {
				printfDx("| %.2f %.2f %.2f %.2f |\n", dx_mat.m[i][0] * 100, dx_mat.m[i][1] * 100, dx_mat.m[i][2] * 100, dx_mat.m[i][3]);
			}
			printfDx("Frame 10 World Mat PX:\n");
			for (int i = 0; i < 4; i++) {
				printfDx("| %.2f %.2f %.2f %.2f |\n", px_mat[i][0] * 100, px_mat[i][1] * 100, px_mat[i][2] * 100, px_mat[i][3]);
			}
			DrawLine3D(cast(px_mat.getPosition()), cast(px_mat.getPosition() + px_mat.column0.getXYZ() * 100), Color::RED);
			DrawLine3D(cast(px_mat.getPosition()), cast(px_mat.getPosition() + px_mat.column1.getXYZ() * 100), Color::GREEN);
			DrawLine3D(cast(px_mat.getPosition()), cast(px_mat.getPosition() + px_mat.column2.getXYZ() * 100), Color::BLUE);
		}
	}
	void Player::Exit()
	{
	}
	void Player::OnCollisionEnter(const HitInfo& hit_info)
	{
		pl_controller->OnCollisionEnter(hit_info);
	}
	void Player::OnCollisionStay(const HitInfo& hit_info)
	{
		pl_controller->OnCollisionStay(hit_info);
	}
	void Player::OnCollisionExit(const HitInfo& hit_info)
	{
		pl_controller->OnCollisionExit(hit_info);
	}
	void Player::OnTriggerEnter(const HitInfo& hit_info)
	{
		pl_controller->OnTriggerEnter(hit_info);
	}
	void Player::OnTriggerStay(const HitInfo& hit_info)
	{
		pl_controller->OnTriggerStay(hit_info);
	}
	void Player::OnTriggerExit(const HitInfo& hit_info)
	{
		pl_controller->OnTriggerExit(hit_info);
	}

	void Player::Damage(int damage)
	{
		pl_controller->Damage(1);
	}
} // namespace NeonFade