#include "precompile.h"
#include "SampleRoomScene.h"
#include "System/Components/ModelRenderer.h"
#include "System/Objects/CameraObject.h"


namespace Sample {


	void SampleRoomScene::Load()
	{
		loading_status = LOADING_STATUS::LOADING;
		ModelManager::LoadAsModel(u8"data/BedRoom.mv1", "room_model");
		CheckForLoading();
	}
	int SampleRoomScene::Init()
	{
		if (!CheckForLoading())
			return Super::Init();
		auto room = SceneManager::Object::Create<GameObject>("Room");
		room->AddComponent<ModelRenderer>()->SetModel("room_model");
		room->transform->scale = { 0.05f,0.05f,0.05f };
		room_obj = room;
		SceneManager::Object::Create<DebugCameraObject>(u8"デバッグカメラ");
		return Super::Init();
	}

	void SampleRoomScene::Update()
	{
		if (!CheckForLoading())
			return;
	}

	void SampleRoomScene::LateDraw()
	{
		if (!CheckForLoading())
		{
			DrawString(0, 0, u8"Now Loading...", GetColor(255, 255, 255));
		}
	}

	void SampleRoomScene::Exit()
	{
		room_obj.reset();
		Super::Exit();
	}

	bool SampleRoomScene::CheckForLoading()
	{
		if (loading_status == LOADING_STATUS::LOADED)
			return true;
		loading_status = ModelManager::GetLoadingCount() == 0 ? LOADING_STATUS::LOADED : LOADING_STATUS::LOADING;
		if (loading_status == LOADING_STATUS::LOADED)
			Init();
		return loading_status == LOADING_STATUS::LOADED;
	}


}