#include "precompile.h"
#include "SampleObject.h"

int SampleObject::Init()
{
	transform->SetPosition(float3(0, 1, 0));

	{
		auto scene = SceneManager::GetCurrentScene();
		auto p_scene = scene->GetPhysicsScene();
		rigid_dynamic
			= PhysicsManager::GetPhysicsInstance()->createRigidDynamic(physx::PxTransform(physx::PxIdentity));
		rigid_dynamic->userData = new std::weak_ptr<ObjBase>(shared_from_this());
		// Œ`ó(Box)‚ðì¬
		my_shape
			= PhysicsManager::GetPhysicsInstance()->createShape(
				// Box‚Ì‘å‚«‚³
				physx::PxSphereGeometry(1.f),
				// –€ŽCŒW”‚Æ”½”­ŒW”‚ÌÝ’è
				*PhysicsManager::GetPhysicsInstance()->createMaterial(0.99f, 0.99f, 0.1f)
			);
		// Œ`ó‚ð•R‚Ã‚¯
		my_shape->setLocalPose(physx::PxTransform(physx::PxIdentity));
		my_shape->setSimulationFilterData(physx::PxFilterData(1, 1, 0, 0));
		rigid_dynamic->attachShape(*my_shape);
		p_scene->addActor(*rigid_dynamic);
		rigid_dynamic->addForce(physx::PxVec3(0, 10, 0), physx::PxForceMode::eIMPULSE);
	}
	model = MV1LoadModel("data/Player.mv1");
	return 0;
}

void SampleObject::Update()
{
	float3 velocity_factor = float3(0, 0, 0);
	if (Input::CheckHitKey(KEY_INPUT_UP))
		velocity_factor += float3(0, 0, 1);
	if (Input::CheckHitKey(KEY_INPUT_DOWN))
		velocity_factor -= float3(0, 0, 1);
	if (Input::CheckHitKey(KEY_INPUT_RIGHT))
		velocity_factor += float3(1, 0, 0);
	if (Input::CheckHitKey(KEY_INPUT_LEFT))
		velocity_factor -= float3(1, 0, 0);
	if (Input::CheckHitKey(KEY_INPUT_SPACE))
		SceneManager::Object::Create<SampleObject3>();
	velocity_factor.normalize();
	velocity_factor *= 5;
	velocity_factor.y = velocity.y;
	velocity = velocity_factor;


	if (Input::CheckHitKey(KEY_INPUT_W))
		transform->AddRotation(float3(-3 * Time::UnscaledDeltaTime(), 0, 0));
	if (Input::CheckHitKey(KEY_INPUT_S))
		transform->AddRotation(float3(3 * Time::UnscaledDeltaTime(), 0, 0));
	if (Input::CheckHitKey(KEY_INPUT_A))
		transform->AddRotation(float3(0, -3 * Time::UnscaledDeltaTime(), 0));
	if (Input::CheckHitKey(KEY_INPUT_D))
		transform->AddRotation(float3(0, 3 * Time::UnscaledDeltaTime(), 0));
	if (Input::CheckHitKey(KEY_INPUT_Q))
		transform->AddRotation(float3(0, 0, 3 * Time::UnscaledDeltaTime()));
	if (Input::CheckHitKey(KEY_INPUT_E))
		transform->AddRotation(float3(0, 0, -3 * Time::UnscaledDeltaTime()));
	if (Input::CheckHitKey(KEY_INPUT_U))
		transform->SetAxisZ(float3(0, 0, 1));



}

void SampleObject::Draw()
{
	if (!camera) {
		DrawSphere3D(cast(transform->position), 1.0f, 8, GetColor(0, 0, 255), GetColor(0, 0, 255), true);
	}

	mat4x4 mat(CastPhysXQuat(transform->rotation));
	mat.scale(Vector4(transform->scale * 0.01f, 1));
	mat.setPosition(transform->position);
	MV1SetMatrix(model, cast(mat));
	MV1DrawModel(model);
}

void SampleObject::PreDraw()
{
	if (camera)
		SetCameraPositionAndTargetAndUpVec(float3(transform->position), float3(transform->position + transform->AxisZ()), float3(transform->AxisY()));
}

void SampleObject::PrePhysics()
{

	rigid_dynamic->setGlobalPose(physx::PxTransform(transform->position, transform->rotation));
	rigid_dynamic->setLinearVelocity(velocity);
}

void SampleObject::PostPhysics()
{
	auto trns = rigid_dynamic->getGlobalPose();
	transform->position = trns.p;
	transform->rotation = trns.q;
	velocity = rigid_dynamic->getLinearVelocity();
}

void SampleObject::DebugDraw()
{

	float3 pos = transform->position;
	if (!camera) {
		SetUseLighting(false);
		SetLightEnable(false);
		DrawSphere3D(cast(transform->position), 1.0f, 8, GetColor(0, 0, 255), GetColor(0, 0, 255), false);
		SetUseLighting(true);
		SetLightEnable(true);
	}
}

void SampleObject::Exit()
{

	SceneManager::GetCurrentScene()->DeleteActor(rigid_dynamic);

	if (model > 0)
		MV1DeleteModel(model);
}


int SampleObject2::Init()
{
	transform->SetPosition(float3((GetRand(140) - 70) * 0.1f, GetRand(50), (GetRand(140) - 70) * 0.1f));
	//SetCameraPositionAndTargetAndUpVec(float3(transform->position), float3(transform->position + transform->AxisZ()), float3(transform->AxisY()));
	color = GetColor(0, 0, 255);
	{
		auto scene = SceneManager::GetCurrentScene();
		auto p_scene = scene->GetPhysicsScene();
		rigid_dynamic
			= PhysicsManager::GetPhysicsInstance()->createRigidDynamic(physx::PxTransform(physx::PxIdentity));
		rigid_dynamic->userData = new std::weak_ptr<ObjBase>(shared_from_this());
		// Œ`ó(Sphere)‚ðì¬
		my_shape
			= PhysicsManager::GetPhysicsInstance()->createShape(
				// Sphere‚Ì”¼Œa
				physx::PxSphereGeometry(0.5f),
				// –€ŽCŒW”‚Æ”½”­ŒW”‚ÌÝ’è
				*PhysicsManager::GetPhysicsInstance()->createMaterial(0.99f, 0.99f, 0.1f)
			);
		// Œ`ó‚ð•R‚Ã‚¯
		my_shape->setLocalPose(physx::PxTransform(physx::PxIdentity));
		my_shape->setSimulationFilterData(physx::PxFilterData(1, 1, 0, 0));
		rigid_dynamic->attachShape(*my_shape);
		p_scene->addActor(*rigid_dynamic);
	}
	return 0;
}


void SampleObject2::Draw()
{
	DrawSphere3D(cast(transform->position), 0.5f, 8, color, color, true);
}

void SampleObject2::Update()
{
	if (Input::CheckHitKey(KEY_INPUT_SPACE))
		rigid_dynamic->addForce(float3(0, GetRand(20), 0), physx::PxForceMode::eIMPULSE);
}


void SampleObject2::PrePhysics()
{
	rigid_dynamic->setGlobalPose(physx::PxTransform(transform->position, transform->rotation));
	rigid_dynamic->setLinearVelocity(velocity);
}

void SampleObject2::PostPhysics()
{
	auto trns = rigid_dynamic->getGlobalPose();
	transform->position = trns.p;
	transform->rotation = trns.q;
	velocity = rigid_dynamic->getLinearVelocity();
}

void SampleObject2::OnCollisionEnter(const HitInfo& hit_info)
{
	color = GetColor(255, 0, 0);
}

void SampleObject2::OnCollisionStay(const HitInfo& hit_info)
{
	color = GetColor(0, 255, 0);
}

void SampleObject2::OnCollisionExit(const HitInfo& hit_info)
{
	color = GetColor(0, 0, 255);
}

void SampleObject2::DebugDraw()
{
	SetUseLighting(false);
	SetLightEnable(false);
	DrawSphere3D(cast(transform->position), 0.5f, 8, color, color, false);
	SetUseLighting(true);
	SetLightEnable(true);
}

void SampleObject2::Exit()
{
	SceneManager::GetCurrentScene()->DeleteActor(rigid_dynamic);

}


int SampleObject3::Init()
{
	//SetCameraPositionAndTargetAndUpVec(float3(transform->position), float3(transform->position + transform->AxisZ()), float3(transform->AxisY()));
	color = GetColor(255, 0, 255);
	transform->SetPosition(float3(GetRand(14) - 7, GetRand(14), GetRand(14) - 7));
	tag = Player;
	obj = SceneManager::Object::Get<SampleObject>();
	{
		auto scene = SceneManager::GetCurrentScene();
		auto p_scene = scene->GetPhysicsScene();
		rigid_dynamic
			= PhysicsManager::GetPhysicsInstance()->createRigidDynamic(physx::PxTransform(physx::PxIdentity));
		rigid_dynamic->userData = new std::weak_ptr<ObjBase>(shared_from_this());
		// Œ`ó(Sphere)‚ðì¬
		my_shape
			= PhysicsManager::GetPhysicsInstance()->createShape(
				// Sphere‚Ì”¼Œa
				physx::PxSphereGeometry(0.5f),
				// –€ŽCŒW”‚Æ”½”­ŒW”‚ÌÝ’è
				*PhysicsManager::GetPhysicsInstance()->createMaterial(0.99f, 0.99f, 0.1f)
			);
		// Œ`ó‚ð•R‚Ã‚¯
		my_shape->setLocalPose(physx::PxTransform(physx::PxIdentity));
		my_shape->setSimulationFilterData(physx::PxFilterData(1, 1, 0, 0));
		rigid_dynamic->attachShape(*my_shape);
		p_scene->addActor(*rigid_dynamic);
		rigid_dynamic->addForce(obj->transform->AxisZ() * 5, physx::PxForceMode::eVELOCITY_CHANGE);
	}
	return 0;
}


void SampleObject3::Draw()
{
	DrawSphere3D(cast(transform->position), 0.5f, 8, color, color, true);
}


void SampleObject3::PrePhysics()
{
	rigid_dynamic->setGlobalPose(physx::PxTransform(transform->position, transform->rotation));
	rigid_dynamic->setLinearVelocity(velocity);
}

void SampleObject3::PostPhysics()
{
	auto trns = rigid_dynamic->getGlobalPose();
	transform->position = trns.p;
	transform->rotation = trns.q;
	velocity = rigid_dynamic->getLinearVelocity();
}

void SampleObject3::OnCollisionEnter(const HitInfo& hit_info)
{
	color = GetColor(255, 0, 0);
	if (hit_info.hit_collision != obj && hit_info.hit_collision->tag != tag)
		SceneManager::Object::Destory(hit_info.hit_collision);
}

void SampleObject3::OnCollisionStay(const HitInfo& hit_info)
{
	color = GetColor(0, 255, 0);
}

void SampleObject3::OnCollisionExit(const HitInfo& hit_info)
{
	color = GetColor(0, 0, 255);
}

void SampleObject3::DebugDraw()
{
	SetUseLighting(false);
	SetLightEnable(false);
	DrawSphere3D(cast(transform->position), 0.5f, 8, color, color, false);
	SetUseLighting(true);
	SetLightEnable(true);
}

void SampleObject3::Exit()
{

	SceneManager::GetCurrentScene()->DeleteActor(rigid_dynamic);

}


