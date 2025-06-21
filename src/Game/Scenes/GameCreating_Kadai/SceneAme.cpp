#include "precompile.h"
#include "SceneAme.h"
#include "System/Objects/CameraObject.h"
#include "System/Components/ModelRenderer.h"
#include "System/Components/RigidBody.h"
#include "System/Components/CapsuleCollider.h"
#include "System/Components/MeshCollider.h"
#include "System/Objects/ButtonObject.h"
#include "System/Objects/TextObject.h"

/*
 長い岩1
9696,-181,-2650
0,4.712,0
0.68,0.68,0.58

長い岩2
10443,-339.825,-3398.252
0,0.224,0
0.84,0.84,0.84

デカい岩1
9311,0,-3670
0,0,0
0.48,0.48,0.48

デカい岩2
10013,0,-3874
0,3.276,0
0.48,0.48,0.48

小っちゃい岩
9628,249,-3783
0,0.494,0
10.44,10.44,10.44

神社
9714,362,-3171
0,1.571,0
102,102,102
*/

inline std::string u8str(const char8_t* str) {
	return std::string(reinterpret_cast<const char*>(str));
}
struct Ame {
	Vector3 position;
	Vector3 dir = { 0,-1,0 };
	float reflect_timer = 0;
};

using 零指定 = nullptr_t;
零指定 零 = nullptr;

namespace AmeGame
{
	std::array<Ame, 1000> ame = {};
	std::array<std::string, 10> txt{
		u8str(u8"俺はあの日、<c900>奇妙<c>なものに出会った。"),
		u8str(u8"吾輩は<c990>猫<c>である。\n名前は<c900>\"まだ\"<c>ない。"),
		u8str(u8"隣の客はよく<c960>柿<c>食う客だ"),
		u8str(u8"今は昔、<c090>竹取の翁<c>といふものありけり。"),
		u8str(u8"あの日見た<c977>花<c>の名前を僕たちはまだ知らない。"),
		u8str(u8"どうして\"こんなこと\"になってしまったのだろう...。"),
		u8str(u8"いいじゃないか 偽物の<c008>勇者<c>で。僕は<c200>魔王<c>を倒して世界の平和を取り戻す。\nそうすれば<c090>偽物<c>だろうが<c900>本物<c>だろうが関係ない。"),
		u8str(u8"<c580>落ちこぼれ<c>だって<c900>必死で努力<c>すりゃ<c779>エリート<c>を超えることがあるかもよ"),
		u8str(u8"俺は高校生探偵の「<c898>イソフラボン<c>」。\n同級生で幼馴染の「<c898>にがり<c>」と一緒に遊園地に来ていた。"),
		u8str(u8"どうしたぁ...わrrrrらえよぉ<c009>ベジータァ<c>...")
	};
	void SceneAme::Load()
	{
		ModelManager::LoadAsModel("data/Stage/Gensokyo/Gensokyo.mv1", "field");
		ModelManager::LoadAsModel("data/Stage/HakureiShrine/HakureiShrine.mv1", "shrine");
		ModelManager::LoadAsModel("data/Stage/SwordBout/nagai_iwa.mv1", "long_rock");
		ModelManager::LoadAsModel("data/Stage/SwordBout/dekai_iwa.mv1", "huge_rock");
		ModelManager::LoadAsModel("data/Stage/SwordBout/chicchai_iwa.mv1", "small_rock");

		loading_status = ModelManager::GetLoadingCount() == 0 ? LOADING_STATUS::LOADED : LOADING_STATUS::LOADING;
	}

	void InitStageUnits(TransformP root, std::string mod_name, Vector3 pos, Quaternion rot, Vector3 scale) {

		auto stage = SceneManager::Object::Create<Object>();
		stage->transform->position = pos;
		stage->transform->rotation = rot;
		stage->transform->scale = scale;
		auto model = stage->AddComponent<ModelRenderer>();
		model->SetModel(mod_name);
		stage->AddComponent<RigidBody>();
		stage->AddComponent<MeshCollider>()->AttachToModel();
		root->SetChild(stage->transform); //ステージをシーンのルートに配置

	}
	int SceneAme::Init()
	{
		if (!CheckForLoading())
			return 0;

		//地形ルートパーツ
		auto root = SceneManager::Object::Create<Object>();

		{//地形の子供となるパーツの初期化
			InitStageUnits(root->transform, "field", { 0,0,0 }, { 0,0,0,1 }, { 1,1,1 });
			InitStageUnits(root->transform, "shrine", { 9800,362,-3171 }, Quaternion(1.571f, { 0,1,0 }), { 102,102,102 });
			InitStageUnits(root->transform, "long_rock", { 10543,-339.8f,-3398.2f }, Quaternion(0.224f, { 0,1,0 }), { 0.84f,0.84f,0.84f });
			InitStageUnits(root->transform, "long_rock", { 9796,-181,-2650 }, Quaternion(4.712f, { 0,1,0 }), { 0.68f,0.68f,0.68f });
			InitStageUnits(root->transform, "huge_rock", { 9401,0,-3670 }, { 0,0,0,1 }, { 0.48f,0.48f,0.48f });
			InitStageUnits(root->transform, "huge_rock", { 10103,0,-3874 }, Quaternion(3.276f, { 0,1,0 }), { 0.48f,0.48f,0.48f });
			InitStageUnits(root->transform, "small_rock", { 9728,249,-3783 }, Quaternion(0.494f, { 0,1,0 }), { 10.44f,10.44f,10.44f });

		}
		if constexpr (false)
		{
			auto stage = SceneManager::Object::Create<Object>();
			auto model = stage->AddComponent<ModelRenderer>();
			model->SetModel("shrine");
			stage->AddComponent<RigidBody>();
			stage->AddComponent<MeshCollider>()->AttachToModel();
			root->transform->SetChild(stage->transform); //ステージをシーンのルートに配置
		}
		root->transform->scale = { 0.1f,0.1f,0.1f };



		//プレイヤーの初期化(仮)
		auto camera_ = SceneManager::Object::Create<GameObject>();
		auto rb = camera_->AddComponent<RigidBody>();
		rb->freeze_rotation = { true, true, true }; //カメラは回転しない
		auto col = camera_->AddComponent<CapsuleCollider>();
		col->radius = 0.5f;
		col->height = 1.45f;
		col->rotation = Quaternion(DEG2RAD(90), { 0,0,1 });
		col->position = { -1, 0, 0 }; //カプセルの上端をカメラの位置に合わせる
		camera_->transform->position = { 0,10,0 };
		camera_->transform->rotation = { 0, 0, 0 ,1 };
		camera = camera_;
		camera_ = SceneManager::Object::Create<CameraObject>();
		camera_->transform->rotation = { 0, 0, 0 ,1 };
		camera->transform->SetChild(camera_->transform);
		camera_->transform->local_position = { 0,-0.5f,0 };
		camera->transform->position = { 900,20,-350 };
		camera = camera_;
		for (auto& ame_ : ame)
		{
			ame_.position = Random::Position(camera->transform->position + Vector3(-40, 0, -40), camera->transform->position + Vector3(40, 40, 40));

		}
		txtnavi = SceneManager::Object::Create<ButtonObject>();
		txtnavi->CanvasAnchorType() = UIObject::ANCHOR_TYPE::LEFT_BOTTOM;
		txtnavi->AnchorType() = UIObject::ANCHOR_TYPE::LEFT_BOTTOM;
		auto text = txtnavi->GetComponent<Text>();
		text->SetText(txt[0]);
		text->SetAlignment(Text::ALIGNMENT::AUTO);
		text->FontSize() = 28;
		text->ResetDrawChar();

		txtnavi->transform->scale = { 500,400,1 };
		txtnavi->transform->position = { 0,100,0 };
		auto&& lambda = [this]() {int a = GetRand(8); txtnavi->AnchorType() = static_cast<UIObject::ANCHOR_TYPE>(a); };
		txtnavi->GetComponent<Button>()->SetFunc(std::move(lambda));


		return 0;
	}

	void SceneAme::Update()
	{
		if (!CheckForLoading())
			return;
		if (Input::GetKey(KeyCode::Left)) {
			camera->transform->parent->AddRotation(Quaternion(DEG2RAD(Time::DeltaTime() * -45), { 0, 1, 0 }));
		}
		if (Input::GetKey(KeyCode::Right)) {
			camera->transform->parent->AddRotation(Quaternion(DEG2RAD(Time::DeltaTime() * 45), { 0, 1, 0 }));
		}
		if (Input::GetKey(KeyCode::Up)) {
			camera->transform->local_rotation *= (Quaternion(DEG2RAD(Time::DeltaTime() * -45), { 1, 0, 0 }));
		}
		if (Input::GetKey(KeyCode::Down)) {
			camera->transform->local_rotation *= (Quaternion(DEG2RAD(Time::DeltaTime() * 45), { 1, 0, 0 }));
		}
		auto cam_parent = camera->transform->parent.lock();
		auto rb = cam_parent->owner->GetComponent<RigidBody>();
		if (cam_parent) {
			Vector3 mov = { 0,0,0 };
			if (Input::GetKey(KeyCode::W)) {
				mov += cam_parent->AxisZ();
			}
			if (Input::GetKey(KeyCode::S)) {
				mov -= cam_parent->AxisZ();
			}
			if (Input::GetKey(KeyCode::A)) {
				mov -= cam_parent->AxisX();
			}
			if (Input::GetKey(KeyCode::D)) {
				mov += cam_parent->AxisX();
			}
			if (Input::GetKeyDown(KeyCode::Space)) {
				if (rb) {
					rb->AddForce(Vector3(0, 5, 0), ForceMode::Impulse);
				}
			}
			mov.normalize();
			mov *= 5.0f;
			if (Input::GetKey(KeyCode::LShift))
			{
				mov *= 2; //シフトキーで移動速度を2倍にする
			}
			mov.y = rb->velocity.y; //重力を無視して移動する
			rb->velocity = mov;
		}
		if (Input::GetKeyDown(KeyCode::Return)) {
			static int idx = 0;
			idx++;
			if (idx > 9)
				idx = 0;
			txtnavi->GetComponent<Text>()->ResetDrawChar();
			txtnavi->GetComponent<Text>()->SetText(txt[idx]);
		}
		for (auto& ame_ : ame)
		{
			ame_.position += ame_.dir * Time::RealDeltaTime() * 30.0f * (1.0f - ame_.reflect_timer);
			if (ame_.reflect_timer > 0)
				ame_.reflect_timer += Time::DeltaTime() * 10;
		}
	}

	void SceneAme::Draw()
	{
		if (!CheckForLoading())
			return;
		SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
		Vector3 cam_pos = camera->transform->position;
		for (auto& ame_ : ame)
		{
			physx::PxRaycastBuffer ray;
			if (ame_.position.y - cam_pos.y < 20)
				GetPhysicsScene()->raycast(ame_.position, ame_.dir, 1, ray);
			if (ame_.position.y - cam_pos.y < -5 || ame_.reflect_timer > 1.0f)
			{
				ame_.position = Random::Position(cam_pos + Vector3(-40, 20, -40), cam_pos + Vector3(40, 40, 40));
				ame_.reflect_timer = 0;
				ame_.dir = { 0,-1,0 };
			}
			if (ray.hasBlock) {
				ame_.dir = ray.block.normal;
				ame_.position = ray.block.position;
				ame_.reflect_timer += Time::DeltaTime();
			}
			DrawLine3D(cast(ame_.position), cast(ame_.position + ame_.dir * (1.0f - ame_.reflect_timer)), Color::BLUE);
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	void SceneAme::LateDraw()
	{
		if (!CheckForLoading()) {
			std::string str("Loading");
			for (int i = 0; i < (int)(Time::GetTimeFromStart() * 10) % 5; i++)
				str += ".";
			DrawFormatString(SCREEN_W * 0.5f, SCREEN_H * 0.5f, Color::RED, "%s", str.c_str());
			return;
		}
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 192);
		DrawString(10, 10, "SceneAme", Color::WHITE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		DrawFormatString(10, 30, Color::WHITE, "FPS: %.1f", Time::GetFPS());

		{
			DrawFormatString(10, SCREEN_H - 30, Color::WHITE, "Press Left/Right to rotate camera");
			DrawFormatString(10, SCREEN_H - 50, Color::WHITE, "Press Up/Down to rotate camera view");
			DrawFormatString(10, SCREEN_H - 70, Color::WHITE, "Press W/A/S/D to move camera");
			DrawFormatString(10, SCREEN_H - 90, Color::WHITE, "Press Space to jump");
		}
	}

	bool SceneAme::CheckForLoading()
	{
		if (loading_status == LOADING_STATUS::LOADING) {
			loading_status = ModelManager::GetLoadingCount() == 0 ? LOADING_STATUS::LOADED : LOADING_STATUS::LOADING;
			if (loading_status == LOADING_STATUS::LOADED) {
				Init();
				return true;
			}
			return false;
		}
		return true;
	}

}