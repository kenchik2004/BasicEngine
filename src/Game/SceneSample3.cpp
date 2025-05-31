#include "precompile.h"
#include "SceneSample3.h"
#include "Game/PlaneObject.h"
#include "System/Components/RigidBody.h"
#include "System/Components/ModelRenderer.h"
#include "System/Components/MeshCollider.h"


void SceneSample3::Load()
{
	ModelManager::LoadAsModel("data/aircraft.mv1", "aircraft");
	ModelManager::LoadAsModel("data/Stage/stage00.mv1", "Stage");
}

int SceneSample3::Init()
{
	plane = SceneManager::Object::Create<PlaneObject>();
	auto obj2 = SceneManager::Object::Create<Object>();
	obj2->tag = ObjBase::TAG::Stage;
	obj2->transform->position = { 0,-50,0 };
	obj2->transform->scale = { 0.01f,0.01f,0.01f };
	obj2->AddComponent<RigidBody>();
	auto model = obj2->AddComponent<ModelRenderer>();
	model->SetModel("Stage");
	auto mod_col = obj2->AddComponent<MeshCollider>();
	mod_col->AttachToModel();
	return 0;
}

void SceneSample3::Update()
{
	if (Input::GetKeyDown(KeyCode::Return))
		SceneManager::Change(SafeSharedPtr(shared_from_this()));
	if (!plane.lock())
		SceneManager::Load<SceneSample3>();

	if (Input::GetKeyDown(KeyCode::I))
	{

		SafeWeakPtr<ObjBase> obj;

		obj->AddComponent<RigidBody>();
	}

	
}


