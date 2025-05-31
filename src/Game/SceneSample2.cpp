#include "precompile.h"
#include "SceneSample.h"
#include "SceneSample2.h"
#include "System/Components/RigidBody.h" 
#include "System/Components/CapsuleCollider.h" 
#include "System/Components/MeshCollider.h"
#include "System/Components/ModelRenderer.h" 
#include "System/Components/Animator.h" 


bool is_jump = false;
int SceneSample2::Init()
{
	obj = SceneManager::Object::Create<Object>();
	obj.lock()->name = "AAAAA";
	auto rb = obj.lock()->AddComponent<RigidBody>();
	auto col = obj.lock()->AddComponent<CapsuleCollider>();
	col->rotation = Quaternion(DEG2RAD(90), Vector3(0, 0, 1));
	col->position = Vector3(1.0f, 0, 0);
	obj.lock()->transform->position = Vector3(0, 5, 0);
	rb->freeze_rotation = { 1,1,1 };
	auto model = obj.lock()->AddComponent<ModelRenderer>();
	model->scale = { 0.01f,0.01f,0.01f };
	model->rot = Quaternion(DEG2RAD(180), Vector3(0, 1, 0));
	model->SetModel("man");
	auto animator = obj.lock()->AddComponent<Animator>();
	animator->SetAnimation("walk", 1);
	animator->SetAnimation("stand", 1);
	animator->SetAnimation("run", 1);
	animator->SetAnimation("jump", 0);
	animator->PlayIfNoSame("walk", true);
	animator->anim_speed = 2.0f;
	GetPhysicsScene()->addActor(*physx::PxCreatePlane(
		*PhysicsManager::GetPhysicsInstance(), physx::PxPlane(0, 1, 0, 0),
		*Material::Concrete_Default)
	);
	obj2 = SceneManager::Object::Create<Object>();
	obj2.lock()->tag = ObjBase::TAG::Stage;
	obj2.lock()->transform->position = { 0,0,0 };
	obj2.lock()->transform->scale = { 0.01f,0.01f,0.01f };
	obj2.lock()->AddComponent<RigidBody>();
	obj2.lock()->AddComponent<ModelRenderer>()->SetModel("stage");
	auto mod_col = obj2.lock()->AddComponent<MeshCollider>();
	mod_col->AttachToModel();

	return 0;
}

void SceneSample2::Update()
{
	auto rb = obj.lock()->GetComponent<RigidBody>();
	float3 velocity_factor = float3(0, 0, 0);
	auto model = obj.lock()->GetComponent<ModelRenderer>();
	auto animator = obj.lock()->GetComponent<Animator>();

	if (Input::GetKey(KeyCode::Left))
		obj.lock()->transform->AddRotation(float3(0, -90 * Time::UnscaledDeltaTime(), 0));
	if (Input::GetKey(KeyCode::Right))
		obj.lock()->transform->AddRotation(float3(0, 90 * Time::UnscaledDeltaTime(), 0));

	if (Input::GetKey(KeyCode::Up))
		obj.lock()->transform->AddRotation(float3(-90 * Time::UnscaledDeltaTime(), 0, 0));
	if (Input::GetKey(KeyCode::Down))
		obj.lock()->transform->AddRotation(float3(90 * Time::UnscaledDeltaTime(), 0, 0));

	if (Input::GetKey(KeyCode::W)) {
		velocity_factor += obj.lock()->transform->AxisZ();
		model->rot = Quaternion(DEG2RAD(180), Vector3(0, 1, 0));
	}
	if (Input::GetKey(KeyCode::S)) {
		velocity_factor -= obj.lock()->transform->AxisZ();
		model->rot = Quaternion(DEG2RAD(0), Vector3(0, 1, 0));

	}
	if (Input::GetKey(KeyCode::D)) {
		velocity_factor += obj.lock()->transform->AxisX();
		model->rot = Quaternion(DEG2RAD(270), Vector3(0, 1, 0));
	}
	if (Input::GetKey(KeyCode::A)) {
		velocity_factor -= obj.lock()->transform->AxisX();
		model->rot = Quaternion(DEG2RAD(90), Vector3(0, 1, 0));

	}
	velocity_factor = ProjectOnPlane(velocity_factor, float3(0, 1, 0)).normalized() * 3.5f;
	if (Input::GetKey(KeyCode::LShift))
		velocity_factor *= 1.5f;
	if (velocity_factor.GetLength() < FLT_EPSILON && !is_jump)
		animator->PlayIfNoSame("stand", true);
	else if (velocity_factor.GetLength() <= 4.0f && !is_jump)
		animator->PlayIfNoSame("walk", true);
	else if (!is_jump)
		animator->PlayIfNoSame("run", true);
	else
		animator->PlayIfNoSame("jump", false, 0.5f);

	velocity_factor.y = rb->velocity.y;
	rb->velocity = velocity_factor;

	if (Input::GetKeyDown(KeyCode::Space))
		rb->AddForce(Vector3(0, 10.0f, 0), ForceMode::Impulse);
	rb = nullptr;
	model = nullptr;
	if (Input::GetKeyDown(KeyCode::Return))
		SceneManager::Change(SceneManager::GetScene<SceneSample>());
}

void SceneSample2::PreDraw()
{
	float3 cam_pos = obj.lock()->transform->position;
	cam_pos -= obj.lock()->transform->AxisZ() * 3.0f;
	cam_pos += obj.lock()->transform->AxisY() * 2.0f;

	SetCameraPositionAndTargetAndUpVec(cam_pos, cast(obj.lock()->transform->position + obj.lock()->transform->AxisY()), cast(obj.lock()->transform->AxisY()));
}

void SceneSample2::LateDebugDraw()
{

	DrawBox(0, 0, 300, 50, GetColor(128, 0, 0), true);
	DrawFormatString(0, 0, GetColor(255, 255, 255), "FPS : %.2f / %.2f", Time::GetFPSMAX(), Time::GetFPS());
	DrawFormatString(0, 24, GetColor(255, 255, 255), "Objects : %d", SceneManager::Object::GetArray<ObjBase>().size());
}

void SceneSample2::Exit()
{
	obj.reset();
	obj2.reset();
}
