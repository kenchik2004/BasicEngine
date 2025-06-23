#include "precompile.h"
#include "SceneTitle.h"
#include "Game/Scenes/DiceScene.h"
#include "Game/Objects/DiceD6.h"
#include "System/Components/ModelRenderer.h"
#include <System/IniFileManager.h>
#include <System/Components/Camera.h>
namespace RLyeh {

	void SceneTitle::Load()
	{
		AudioManager::Load("data/Sound/title.mp3", "title");
		TextureManager::Load("data/title.jpg", "title_img");
		title_img = TextureManager::CloneByName("title_img");
		loading_status = ModelManager::GetLoadingCount() + AudioManager::GetLoadingCount() == 0 ? LOADING_STATUS::LOADED : LOADING_STATUS::LOADING;
		dice_scene = SceneManager::LoadAsAnother<DiceScene>();
	}

	int SceneTitle::Init()
	{
		loading_status = ModelManager::GetLoadingCount() + AudioManager::GetLoadingCount() == 0 ? LOADING_STATUS::LOADED : LOADING_STATUS::LOADING;
		if (loading_status == LOADING_STATUS::LOADING) {
			auto cam = SceneManager::Object::Create<GameObject>()->AddComponent<Camera>();
			return 0;
		}
		bgm = AudioManager::CloneByName("title");
		bgm->PlayOneShot();
		return 0;
	}

	void SceneTitle::Update()
	{
		if (loading_status == LOADING_STATUS::LOADING) {
			loading_status = ModelManager::GetLoadingCount() + AudioManager::GetLoadingCount() == 0 ? LOADING_STATUS::LOADED : LOADING_STATUS::LOADING;
			if (loading_status == LOADING_STATUS::LOADED)
				Init();
			else
				return;
		}
		//とりあえず技術検証用に裏シーンとのやり取り
		if (Input::GetKeyDown(KeyCode::Space)) {
			dice_scene->Clear();
			dice_scene->Roll(5, 10);
			//dice_scene->Roll(2, 10);
			//dice_scene->Roll(2, 4);
			//dice_scene->Roll(1, 100);
			//dice_scene->Roll(1, 12);
			//dice_scene->Roll(1, 8);
		}

	}

	void SceneTitle::LateDraw()
	{
		//タイトル画面の描画
		if (title_img)
			DrawExtendGraph(0, 0, SCREEN_W, SCREEN_H, title_img->GetHandle(), false);

		//なんかそれっぽく暗めにする
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 192);
		DrawBox(0, 0, SCREEN_W, SCREEN_H, Color::BLACK, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		//ロード中
		if (loading_status == LOADING_STATUS::LOADING) {
			std::string str("Loading");
			for (int i = 0; i < (int)(Time::GetTimeFromStart() * 2) % 5; i++)
				str += ".";
			DrawFormatString(SCREEN_W * 0.5f, SCREEN_H * 0.5f, Color::RED, "%s", str.c_str());
		}
		else {
			//裏シーンの描画領域をこっちに持ってくる
			DrawExtendGraph(0, 0, SCREEN_W, SCREEN_H, SafeStaticCast<Camera>(dice_scene->GetCurrentCamera().lock())->my_screen->GetHandle(), true);
			//デカデカとタイトル表示
			SetFontSize(150);
			int x, y;
			GetDrawStringSize(&x, &y, nullptr, "  THE\nR'LYEH", 13);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, fabsf(sinf(static_cast<float>(Time::GetTimeFromStart())) * 255));
			DrawString(SCREEN_W * 0.5f - x * 0.5f, SCREEN_H * 0.5f - y * 0.5f, "  THE\nR'LYEH", Color(1, 0, 0, 1));
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			SetFontSize(DEFAULT_FONT_SIZE);
			//とりあえず1でスキップ&結果集計
			int i = 0;
			if (Input::GetKeyDown(KeyCode::Alpha1)) {
				dice_scene->Skip();
				results.clear();
				results = dice_scene->FetchResults();
				//std::sort(results.begin(), results.end(), [](int a, int b) {return a < b; });
			}
			int sum = 0;
			for (auto& ite : results) {
				DrawFormatString(0, i * 16, Color::CYAN, "%d,", ite);
				i++;
				sum += ite;
			}
			DrawFormatString(0, i * 16, Color::CYAN, "%d", sum);
			DrawFormatString(100, 16, Color::CYAN, "%f", Time::GetFPS());

		}
		if (Input::GetMouseButtonDown(MouseButton::ButtonLeft))
			DrawString(0, 50, "MouseButtonDown!!", Color::RED);
		if (Input::GetMouseButtonRepeat(MouseButton::ButtonLeft))
			DrawString(0, 66, "MouseButtonRepeat!!", Color::GREEN);
		if (Input::GetMouseButtonUp(MouseButton::ButtonLeft))
			DrawString(0, 82, "MouseButtonUp!!", Color::BLUE);
	}

	void SceneTitle::UnLoad()
	{
		bgm = nullptr;
		title_img.reset();
	}

}