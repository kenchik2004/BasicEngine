#include "precompile.h"
#include "PlayerCameraMachine.h"
#include "Game/Objects/NeonFade/Player.h"

namespace NeonFade
{

	int PlayerCameraMachine::Init()
	{
		return Super::Init();
	}

	void PlayerCameraMachine::PreDraw()
	{
		shake_duration -= Time::UnscaledDeltaTime();
		if (shake_duration > 0.0f) {
			shake_offset = Random::Position({ -1,-1,-1 }, { 1,1,1 }).getNormalized() * shake_intensity;
		}
		else {
			shake_offset = { 0,0,0 };
		}
		{
			Vector2 pad_input = Input::GetPadRightStick(0);
			if (pad_input.magnitudeSquared() >= FLT_EPSILON) {
				camera_rot.y += pad_input.x * Time::DrawDeltaTime() * 90 * 3;
				if (reverse_y)
					camera_rot.x -= pad_input.y * Time::DrawDeltaTime() * 90 * 3;
				else
					camera_rot.x += pad_input.y * Time::DrawDeltaTime() * 90 * 3;
				camera_rot.x = physx::PxClamp(camera_rot.x, -45.0f, 30.0f);
			}
			else {
				if (Input::GetKey(KeyCode::Left))
					camera_rot.y -= Time::DrawDeltaTime() * 90;
				if (Input::GetKey(KeyCode::Right))
					camera_rot.y += Time::DrawDeltaTime() * 90;
			}

			SafeSharedPtr<Transform> pl_transform = target->transform;
			SafeSharedPtr<Transform> cam_trns = owner->transform;
			Quaternion cam_rot_around = Quaternion(DEG2RAD(camera_rot.y), { 0,1,0 }) * Quaternion(DEG2RAD(camera_rot.x), { 1,0,0 });
			Vector3 cam_vector_z = { 0,0,1 };
			cam_vector_z = cam_rot_around.rotate(cam_vector_z);
			physx::PxRaycastHit hits[2];
			RayCastInfo info(hits, 2);
			camera_distance = camera_distance_max;
			physx::PxQueryFilterData a;
			a.data.word0 = Collider::Layer::Terrain;
			a.flags = physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::ePREFILTER;
			owner->GetScene()->RayCast(Ray{ pl_transform->position + Vector3(0,2,0) + shake_offset, -cam_vector_z, camera_distance }, info, a);
			if (info.hasBlock) {
				camera_distance = info.block.distance - 0.05f;
			}
			cam_vector_z = cam_vector_z * camera_distance;
			cam_trns->position = pl_transform->position - cam_vector_z + Vector3(0, 2, 0) + shake_offset;
			cam_trns->SetAxisZ(cam_vector_z);
		}
	}

	void PlayerCameraMachine::Update()
	{
		if (Input::GetPadButtonDown(0, PadButton::Back))
			reverse_y = !reverse_y;
	}


	void PlayerCameraMachine::ShakeCamera(float intensity, float duration)
	{
		shake_intensity = intensity;
		shake_duration = duration;
	}

}