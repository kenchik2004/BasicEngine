#include "precompile.h"
#include "SceneSample3.h"
#include "Game/PlaneObject.h"
#include "System/Components/RigidBody.h"
#include "System/Components/ModelRenderer.h"
#include "System/Components/MeshCollider.h"


int SceneSample3::Init()
{
	plane = SceneManager::Object::Create<PlaneObject>();
	auto obj2 = SceneManager::Object::Create<Object>();
	obj2->tag = ObjBase::TAG::Stage;
	obj2->transform->position = { 0,-50,0 };
	obj2->transform->scale = { 0.1f,0.1f,0.1f };
	obj2->AddComponent<RigidBody>();
	auto model = obj2->AddComponent<ModelRenderer>();
	model->Load("data/Stage/stage00.mv1");
	auto mod_col = obj2->AddComponent<MeshCollider>();
	mod_col->AttachToModel();
	GetPhysicsScene()->addActor(*physx::PxCreatePlane(
		*PhysicsManager::GetPhysicsInstance(), physx::PxPlane(0, 1, 0, 0),
		*PhysicsManager::GetPhysicsInstance()->createMaterial(0.99f, 0.99f, 0.0f))
	);
	return 0;
}

void SceneSample3::Update()
{
	if (Input::PushHitKey(KEY_INPUT_RETURN))
		SceneManager::Change(SafeSharedPtr(shared_from_this()));
	if (!plane.lock())
		SceneManager::Load<SceneSample3>();
	if (Input::PushHitKey(KEY_INPUT_I))
	{
		SafeSharedPtr<ObjBase> obj = nullptr;
		obj->AddComponent<RigidBody>();
	}

	
}


