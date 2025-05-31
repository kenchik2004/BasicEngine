#include "precompile.h"
#include "DiceScene.h"
#include <cmath>

#include "Game/DiceD4.h"
#include "Game/DiceD6.h"
#include "Game/DiceD8.h"
#include "Game/DiceD10.h"
#include "Game/DiceD12.h"
#include "Game/DiceD20.h"

int DiceScene::Init()
{
	GetPhysicsScene()->addActor(*physx::PxCreatePlane(
		*PhysicsManager::GetPhysicsInstance(), physx::PxPlane(0, 1, 0, 0),
		*Material::Wool_Default)
	);
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
	ModelManager::LoadAsModel("data/dice/D4.mv1", "D4");
	ModelManager::LoadAsModel("data/dice/D6.mv1", "D6");
	ModelManager::LoadAsModel("data/dice/D8.mv1", "D8");
	ModelManager::LoadAsModel("data/dice/D10.mv1", "D10");
	ModelManager::LoadAsModel("data/dice/D12.mv1", "D12");
	ModelManager::LoadAsModel("data/dice/D20.mv1", "D20");

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
			obj->transform->position = { 0,5,0 };
		}
		break;
	case 6:
		for (int i = 0; i < first; i++)
			SceneManager::Object::Create<DiceD6>(shared_from_this(), texture[1]);
		break;
	case 8:
		for (int i = 0; i < first; i++)
			SceneManager::Object::Create<DiceD8>(shared_from_this(), texture[2]);
		break;
	case 10:
		for (int i = 0; i < first; i++)
			SceneManager::Object::Create<DiceD10>(shared_from_this(), texture[3]);
		break;
	case 12:
		for (int i = 0; i < first; i++)
			SceneManager::Object::Create<DiceD12>(shared_from_this(), texture[4]);
		break;
	case 20:
		for (int i = 0; i < first; i++)
			SceneManager::Object::Create<DiceD20>(shared_from_this(), texture[5]);
		break;

	}
}

void DiceScene::Draw()
{
	SetDrawScreen(screen);
	SetCameraPositionAndTarget_UpVecY({ 0,3,-3 }, { 0,0,0 });
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
}
