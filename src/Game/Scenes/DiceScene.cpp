#include "precompile.h"
#include "DiceScene.h"
#include <cmath>

#include "Game/Objects/DiceBase.h"
#include "Game/Objects/DiceD4.h"
#include "Game/Objects/DiceD6.h"
#include "Game/Objects/DiceD8.h"
#include "Game/Objects/DiceD10.h"
#include "Game/Objects/DiceD12.h"
#include "Game/Objects/DiceD20.h"
#include "System/Components/ModelRenderer.h"
#include "System/Components/RigidBody.h"
#include "System/Components/MeshCollider.h"


namespace RLyeh {

	int DiceScene::Init()
	{
		GetPhysicsScene()->addActor(*physx::PxCreatePlane(
			*PhysicsManager::GetPhysicsInstance(), physx::PxPlane(0, 1, 0, -0.05),
			*Material::Wool_Default)
		);
		GetPhysicsScene()->addActor(*physx::PxCreatePlane(
			*PhysicsManager::GetPhysicsInstance(), physx::PxPlane(0, -1, 0, 10),
			*Material::Wool_Default)
		);
		GetPhysicsScene()->addActor(*physx::PxCreatePlane(
			*PhysicsManager::GetPhysicsInstance(), physx::PxPlane(1, 0, 0, 4),
			*Material::Wool_Default)
		);
		GetPhysicsScene()->addActor(*physx::PxCreatePlane(
			*PhysicsManager::GetPhysicsInstance(), physx::PxPlane(-1, 0, 0, 4),
			*Material::Wool_Default)
		);
		GetPhysicsScene()->addActor(*physx::PxCreatePlane(
			*PhysicsManager::GetPhysicsInstance(), physx::PxPlane(0, 0, 1, 4),
			*Material::Wool_Default)
		);
		GetPhysicsScene()->addActor(*physx::PxCreatePlane(
			*PhysicsManager::GetPhysicsInstance(), physx::PxPlane(0, 0, -1, 4),
			*Material::Wool_Default)
		);
		auto tray = SceneManager::Object::Create<Object>(shared_from_this());
		tray->AddComponent<RigidBody>();
		tray->transform->scale = { 5,5,5 };
		tray->transform->position = { 0,3,0 };
		tray->transform->AddRotation(Vector3(0, 90, 0));

		auto model = tray->AddComponent<ModelRenderer>();
		model->SetModel("Tray");
		model->pos = { 0,-3,0 };
		model->scale = { 0.01f,0.01f,0.01f };
		MV1SetTextureGraphHandle(model->GetModelHandle(), 0, tray_texture, true);
		tray->AddComponent<MeshCollider>();
		return 0;
	}

	void DiceScene::Load()
	{
		texture[0] = LoadGraph("data/dice/D4.png");
		texture[1] = LoadGraph("data/dice/D6.png");
		texture[2] = LoadGraph("data/dice/D8.png");
		texture[3] = LoadGraph("data/dice/D10.png");
		texture[4] = LoadGraph("data/dice/D12.png");
		texture[5] = LoadGraph("data/dice/D20.png");
		tray_texture = LoadGraph("data/dice/Tray.png");
		ModelManager::LoadAsModel("data/dice/D4.mv1", "D4");
		ModelManager::LoadAsModel("data/dice/D6.mv1", "D6");
		ModelManager::LoadAsModel("data/dice/D8.mv1", "D8");
		ModelManager::LoadAsModel("data/dice/D10.mv1", "D10");
		ModelManager::LoadAsModel("data/dice/D12.mv1", "D12");
		ModelManager::LoadAsModel("data/dice/D20.mv1", "D20");
		ModelManager::LoadAsModel("data/dice/Tray.mv1", "Tray");

		screen = MakeScreen(SCREEN_W, SCREEN_H, true);
	}

	void DiceScene::Roll(int first, int second)
	{
		if (first <= 0 || second <= 0)
			return;
		switch (second) {
		case 4:
			for (int i = 0; i < first; i++) {
				auto obj = SceneManager::Object::Create<DiceD4>(shared_from_this(), texture[0]);
				obj->transform->position = { 0.1f * (GetRand(50) - 25),8, 0.1f * (GetRand(50) - 25) };
				DicePair pair(DicePair::D4, obj);
				dices.push_back(std::move(pair));
			}
			break;
		case 6:
			for (int i = 0; i < first; i++) {
				auto obj = SceneManager::Object::Create<DiceD6>(shared_from_this(), texture[1]);
				obj->transform->position = { 0.1f * (GetRand(50) - 25),8, 0.1f * (GetRand(50) - 25) };
				DicePair pair(DicePair::D6, obj);
				dices.push_back(std::move(pair));
			}
			break;
		case 8:
			for (int i = 0; i < first; i++) {
				auto obj = SceneManager::Object::Create<DiceD8>(shared_from_this(), texture[2]);
				obj->transform->position = { 0.1f * (GetRand(50) - 25),8, 0.1f * (GetRand(50) - 25) };
				DicePair pair(DicePair::D8, obj);
				dices.push_back(std::move(pair));
			}
			break;
		case 10:
			for (int i = 0; i < first; i++) {
				auto obj = SceneManager::Object::Create<DiceD10>(shared_from_this(), texture[3]);
				obj->transform->position = { 0.1f * (GetRand(50) - 25),8, 0.1f * (GetRand(50) - 25) };
				DicePair pair(DicePair::D10, obj);
				dices.push_back(std::move(pair));
			}
			break;
		case 12:
			for (int i = 0; i < first; i++) {
				auto obj = SceneManager::Object::Create<DiceD12>(shared_from_this(), texture[4]);
				obj->transform->position = { 0.1f * (GetRand(50) - 25),8, 0.1f * (GetRand(50) - 25) };
				DicePair pair(DicePair::D12, obj);
				dices.push_back(std::move(pair));
			}
			break;
		case 20:
			for (int i = 0; i < first; i++) {
				auto obj = SceneManager::Object::Create<DiceD20>(shared_from_this(), texture[5]);
				obj->transform->position = { 0.1f * (GetRand(50) - 25),8, 0.1f * (GetRand(50) - 25) };
				DicePair pair(DicePair::D20, obj);
				dices.push_back(std::move(pair));
			}
			break;
		case 100:
			for (int i = 0; i < first; i++) {
				DicePair pair;
				pair.mode = DicePair::D100;
				for (int i = 0; i < 2; i++) {

					auto obj = SceneManager::Object::Create<DiceD10>(shared_from_this(), texture[3]);
					obj->transform->position = { 0.1f * (GetRand(50) - 25),8, 0.1f * (GetRand(50) - 25) };
					pair.pair[i] = obj;
				}
				dices.push_back(std::move(pair));
			}
			break;
		default:
			for (int i = 0; i < first; i++)
				results_out_of_dices.push_back(GetRand(second - 1) + 1);

		}
	}

	void DiceScene::Skip()
	{
		constexpr float delta = 1.0f / 50.0f;
		for (int i = 0; i < 50 * 40; i++) {
			GetPhysicsScene()->simulate(delta);
			GetPhysicsScene()->fetchResults(true);
		}
		SceneManager::PostPhysics();
	}

	void DiceScene::Clear()
	{
		for (auto ite = dices.begin(); ite != dices.end();) {
			SceneManager::Object::Destory(shared_from_this(), (*ite).pair[0]);
			SceneManager::Object::Destory(shared_from_this(), (*ite).pair[1]);
			ite = dices.erase(ite);
		}
	}

	std::vector<int> DiceScene::FetchResults()
	{
		std::vector<int> results;
		//ダイスがあるならいいが、1D3などはダイスが存在しないので、予め計算した結果から持ってくる
		if (!dices.empty())
			for (auto& ite : dices)
				results.push_back(ite.Result());
		else {
			for (auto& ite : results_out_of_dices)
				results.push_back(std::move(ite));
			results_out_of_dices.clear();
		}
		return results;
	}

	void DiceScene::Exit()
	{
		Clear();
	}

	void DiceScene::Draw()
	{
		SetDrawScreen(screen);
		SetCameraPositionAndTarget_UpVecY({ 0,8,-5 }, { 0,0,0 });
		SetCameraNearFar(0.1f, 1000);
		ClearDrawScreen();
	}

	void DiceScene::OnDrawFinish()
	{
		SetDrawScreen(DX_SCREEN_BACK);
		SetCameraPositionAndTarget_UpVecY({ 0,5,-5 }, { 0,0,0 });
		SetCameraNearFar(0.1f, 1000);
	}

	void DiceScene::UnLoad()
	{
		for (int i = 0; i < 6; i++)
			DeleteGraph(texture[i]);
		DeleteGraph(screen);
		DeleteGraph(tray_texture);
	}

}