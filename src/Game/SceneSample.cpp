#include "precompile.h"
#include "SceneSample.h"
#include "SceneSample2.h"
#include "SampleObject.h"
#include <System/Components/RigidBody.h>
#include <System/Components/BoxCollider.h>
#include <System/Components/ModelRenderer.h>
#include <System/Components/MeshCollider.h>
#include <System/Components/ConvexMeshCollider.h>
#include "Game/DiceD4.h"
#include "Game/DiceD6.h"
#include "Game/DiceD8.h"
#include "Game/DiceD10.h"
#include "Game/DiceD12.h"
#include "Game/DiceD20.h"
#include "System/ModelManager.h"
#include "System/Components/AudioPlayer.h" 
#include "System/Components/AudioListener.h" 
class HogeComp :public Component {
public:
	USING_SUPER(HogeComp);
};
namespace Hoge {
	class FugaComp :public HogeComp {
	public:
		USING_SUPER(FugaComp);
	};
	class FooComp :public FugaComp {
		USING_SUPER(FooComp);
	};
}
class FugaComp :public HogeComp {
public:
	USING_SUPER(FugaComp);
};
class FooComp :public FugaComp {
public:
	USING_SUPER(FooComp);
};
class PiyoComp :public FooComp {
public:
	USING_SUPER(PiyoComp);
};


void SceneSample::Load()
{
	ModelManager::LoadAsModel("data/dice/D4.mv1", "D4");
	ModelManager::LoadAsModel("data/dice/D6.mv1", "D6");
	ModelManager::LoadAsModel("data/dice/D8.mv1", "D8");
	ModelManager::LoadAsModel("data/dice/D10.mv1", "D10");
	ModelManager::LoadAsModel("data/dice/D12.mv1", "D12");
	ModelManager::LoadAsModel("data/dice/D20.mv1", "D20");
	ModelManager::LoadAsModel("data/Stage/stage00.mv1", "Stage");

	ModelManager::LoadAsModel("data/model.mv1", "man");
	ModelManager::LoadAsAnimation("data/anim_walk.mv1", "walk");
	ModelManager::LoadAsAnimation("data/anim_stand.mv1", "stand");
	ModelManager::LoadAsAnimation("data/anim_punch.mv1", "punch");
	AudioManager::Load("data/se.mp3", "cutin_se");
	AudioManager::Load("data/救急車.mp3", "ambulance", true);
	ModelManager::LoadAsModel("data/Stage/Stage00.mv1", "kakkoi_town");
}
physx::PxShape* shape = nullptr;

int SceneSample::Init()
{
	loading_status = (AudioManager::GetLoadingCount() + ModelManager::GetLoadingCount() == 0) ? LOADING_STATUS::LOADED : LOADING_STATUS::LOADING;
	if (loading_status == LOADING_STATUS::LOADING)
		return 0;
#if 0
	auto box = SceneManager::Object::Create<Object>();
	box->transform->position = Vector3(0, 0, 0);
	box->AddComponent<RigidBody>()->use_gravity = false;
	auto box_col = box->AddComponent<BoxCollider>();
	box_col->extension = { 200,0.1f,200 };
	box_col->GetRigidBody()->is_kinematic = true;
#endif
	obj = SceneManager::Object::Create<SampleObject>();
	obj.lock()->AddComponent<AudioListener>();
	obj.lock()->transform->rotation = Quaternion(DEG2RAD(90), { 0,1,0 });
	obj.lock()->transform->position = { -10,5,10 };
	handle_d4 = LoadGraph("data/dice/D4.png");
	handle_d6 = LoadGraph("data/dice/D6.png");
	handle_d8 = LoadGraph("data/dice/D8.png");
	handle_d10 = LoadGraph("data/dice/D10.png");
	handle_d12 = LoadGraph("data/dice/D12.png");
	handle_d20 = LoadGraph("data/dice/D20.png");

	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++)
		{
			auto dice = SceneManager::Object::Create<DiceD20>(handle_d4);

			dice->transform->position = Vector3(i - 5.0f, 10.0f, j - 5.0f);
		}

	AudioManager::Load("data/reload.wav", "reload");
	AudioManager::Load("data/arolli.wav", "arolli");
	audio = AudioManager::CloneByName("reload");
	audio2 = AudioManager::CloneByName("arolli");
	bgm = AudioManager::CloneByName("ambulance");
	//plane = physx::PxCreatePlane(
	//	*PhysicsManager::GetPhysicsInstance(), physx::PxPlane(0, 1, 0, 0),
	//	*Material::Concrete_Default);
	//GetPhysicsScene()->addActor(*plane);


	obj2 = SceneManager::Object::Create<Object>();
	obj2.lock()->tag = ObjBase::TAG::Stage;
	obj2.lock()->transform->position = { 0,-1,0 };
	obj2.lock()->transform->scale = { 0.01f,0.01f,0.01f };
	obj2.lock()->AddComponent<RigidBody>()->use_gravity = false;
	obj3 = SceneManager::Object::Create<Object>();
	auto mod2 = obj2.lock()->AddComponent<ModelRenderer>();
	mod2->SetModel("kakkoi_town");


	auto mod_col = obj2.lock()->AddComponent<MeshCollider>();

	audio_player = obj2.lock()->AddComponent<AudioPlayer>();
	audio_player.lock()->SetAudio(bgm);
	audio_player.lock()->Play(1.5f);
	audio_player->radius = 300;

	return 0;
}

void SceneSample::Update()
{
	if (loading_status == LOADING_STATUS::LOADING)
	{
		loading_status = (AudioManager::GetLoadingCount() + ModelManager::GetLoadingCount() == 0) ? LOADING_STATUS::LOADED : LOADING_STATUS::LOADING;

		if (loading_status == LOADING_STATUS::LOADED)
			Init();
		return;
	}

	if (Input::GetKey(KeyCode::F))
		Time::SetFPSMAX(Time::GetFPSMAX() + 2);

	if (Input::GetKey(KeyCode::G))
		Time::SetFPSMAX(Time::GetFPSMAX() - 2);

	if (Input::GetKeyDown(KeyCode::Return)) {
		SceneManager::Load<SceneSample>();
		return;
	}
	if (Input::GetKeyDown(KeyCode::R) && audio_player.lock())
		audio_player.lock()->Play(1.5f);
	if (Input::GetKeyDown(KeyCode::T) && audio_player.lock())
		audio_player.lock()->Stop();
	if (Input::GetKey(KeyCode::Up) && audio_player.lock())
		audio_player.lock()->pitch_rate += 0.001f;
	if (Input::GetKey(KeyCode::Down) && audio_player.lock())
		audio_player.lock()->pitch_rate -= 0.001f;

	if (Input::GetKeyDown(KeyCode::P)) {

		DrawString(SCREEN_W * 0.5f, SCREEN_H * 0.5f, "スキップ中", Color::CYAN);
		ScreenFlip();
		float delta = 1.0f / 50.0f;

		SceneManager::PrePhysics();
		for (float i = 0.0f; i < 10.0f; i += delta) {


			GetPhysicsScene()->simulate(delta);
			GetPhysicsScene()->fetchResults(true);
		}
		SceneManager::PostPhysics();

	}

	obj3.lock()->transform->position = Vector3(sinf(Time::GetTimeFromStart()) * 5, 3, cosf(Time::GetTimeFromStart()) * 5);
}

void SceneSample::PreDraw()
{
	SetCameraPositionAndTarget_UpVecY(float3(0, 7, -10), float3(0, 1, 0));
}

void SceneSample::Draw()
{
	if (loading_status == LOADING_STATUS::LOADING)
		return;

	//	SetCameraPositionAndTarget_UpVecY(float3(3, 3, -3), float3(0, 1, 0));
	for (int x = -7; x <= 7; x++)
	{
		DrawLine3D(float3(x, 0, -7), float3(x, 0, 7), GetColor(255, 255, 255));
		for (int z = -7; z <= 7; z++)
			DrawLine3D(float3(-7, 0, z), float3(7, 0, z), GetColor(255, 255, 255));
	}

	DrawLine3D(float3(0, 0, 0), float3(1, 0, 0), GetColor(0, 255, 0));
	DrawLine3D(float3(0, 0, 0), float3(0, 1, 0), GetColor(255, 0, 0));
	DrawLine3D(float3(0, 0, 0), float3(0, 0, 1), GetColor(0, 0, 255));
	DrawSphere3D(cast(obj3.lock()->transform->position), 1, 32, Color::BLUE, Color::BLUE, true);
}

void SceneSample::LateDraw()
{
	if (loading_status == LOADING_STATUS::LOADING)
	{
		std::string msg = "Loading";
		for (int i = 0; i < (int)(Time::GetRealTimeFromStart() * 5) % 6; i++)
			msg += ".";
		DrawString(SCREEN_W * 0.5f, SCREEN_H * 0.5f, msg.c_str(), Color::RED);
		for (int i = 0; i < 7; i++)
			DrawCircle(SCREEN_W * 0.9f - sinf(DEG2RAD(Time::GetRealTimeFromStart() * 5 + i * 45)) * 20, 50 - cosf(DEG2RAD(Time::GetRealTimeFromStart() * 5 + i * 45)) * 20, 5, Color::YELLOW);
		return;
	}
	DrawBox(0, 0, 300, 50, GetColor(128, 0, 0), true);
	DrawFormatString(0, 0, GetColor(255, 255, 255), "FPS : %.2f / %.2f", Time::GetFPSMAX(), Time::GetFPS());
	DrawFormatString(0, 24, GetColor(255, 255, 255), "Objects : %d", SceneManager::Object::GetArray<ObjBase>().size());
}
void SceneSample::DebugDraw()
{
	if (loading_status == LOADING_STATUS::LOADING)
		return;

	DrawLine3D(float3(0, 0, 0), float3(5, 0, 0), GetColor(0, 255, 0));
	DrawLine3D(float3(0, 0, 0), float3(0, 5, 0), GetColor(255, 0, 0));
	DrawLine3D(float3(0, 0, 0), float3(0, 0, 5), GetColor(0, 0, 255));

	auto obj_ = SceneManager::Object::Get<SampleObject>();
	float3 pos = obj_->transform->position;
	float3 vec = obj_->transform->AxisX();
	float3 vec2 = obj_->transform->AxisY();
	float3 vec3 = obj_->transform->AxisZ();


	DrawLine3D(pos, pos + vec * 5, GetColor(255, 255, 0));
	DrawLine3D(pos, pos + vec2 * 5, GetColor(255, 255, 0));
	DrawLine3D(pos, pos + vec3 * 5, GetColor(255, 255, 0));



}
void SceneSample::LateDebugDraw()
{
	if (loading_status == LOADING_STATUS::LOADING)
	{
		DrawString(SCREEN_W * 0.5f, SCREEN_H * 0.5f, "Loading...", Color::RED);
		return;
	}
	DrawBox(0, 0, 300, 50, GetColor(128, 0, 0), true);
	DrawFormatString(0, 0, GetColor(255, 255, 255), "FPS : %.2f / %.2f", Time::GetFPSMAX(), Time::GetFPS());
	DrawFormatString(0, 24, GetColor(255, 255, 255), "Objects : %d", SceneManager::Object::GetArray<ObjBase>().size());
}
void SceneSample::Exit()
{
	if (plane)
		plane->release();
	if (audio)
		audio.reset();
	if (audio2)
		audio2.reset();
	if (bgm)
		bgm.reset();
	DeleteGraph(handle_d4);
	DeleteGraph(handle_d6);
	DeleteGraph(handle_d8);
	DeleteGraph(handle_d10);
	DeleteGraph(handle_d12);
	DeleteGraph(handle_d20);
}
void SceneSample::UnLoad()
{
}

