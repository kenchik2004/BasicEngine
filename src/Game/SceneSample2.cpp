#include "precompile.h"
#include "SceneSample.h"
#include "SceneSample2.h"
#include "System/Components/RigidBody.h" 
#include "System/Components/CapsuleCollider.h" 

int SceneSample2::Init()
{
	auto obj = SceneManager::Object::Create<Object>();
	obj->AddComponent<RigidBody>();
	obj->AddComponent<CapsuleCollider>()->rotation=Quaternion(DEG2RAD(90),Vector3(1,0,0));
	obj->transform->position = Vector3(0, 5, 0);

	GetPhysicsScene()->addActor(*physx::PxCreatePlane(
		*PhysicsManager::GetPhysicsInstance(), physx::PxPlane(0, 1, 0, 0),
		*PhysicsManager::GetPhysicsInstance()->createMaterial(0.99f, 0.99f, 0.0f))
	);
	return 0;
}

void SceneSample2::Update()
{
	if (Input::PushHitKey(KEY_INPUT_RETURN))
		SceneManager::Change(SceneManager::GetScene<SceneSample>());
}
