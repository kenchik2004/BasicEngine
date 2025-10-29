#include "precompile.h"
#include "SampleAnotherScene.h"
#include "Game/Scenes/Sample/DiceScene.h"
#include "System/Components/ModelRenderer.h"
#include <System/IniFileManager.h>
#include <System/Components/Camera.h>
namespace Sample {

	void SampleAnotherScene::Load()
	{
		AudioManager::Load("data/Sound/title.mp3", "title");
		TextureManager::Load("data/title.jpg", "title_img");
		TextureManager::Load(u8"data/カード/CardTexture_Temp.png", "card_temp");
		TextureManager::Load(u8"data/カード/CardTexture_Doman.png", "card_doman");
		TextureManager::Load(u8"data/カード/CardTexture_Cobushi.png", "card_fist");
		TextureManager::Load(u8"data/カード/CardTexture_Shinsoku.png", "card_shinsoku");
		TextureManager::Load(u8"data/カード/CardTexture_Unpu.png", "card_unpu");


		title_img = TextureManager::CloneByName("title_img");
		loading_status = TextureManager::GetLoadingCount() + ModelManager::GetLoadingCount() + AudioManager::GetLoadingCount() == 0 ? LOADING_STATUS::LOADED : LOADING_STATUS::LOADING;
		dice_scene = SceneManager::LoadAsAnother<DiceScene>();
	}

	int SampleAnotherScene::Init()
	{
		loading_status = TextureManager::GetLoadingCount() + ModelManager::GetLoadingCount() + AudioManager::GetLoadingCount() == 0 ? LOADING_STATUS::LOADED : LOADING_STATUS::LOADING;
		if (loading_status == LOADING_STATUS::LOADING && !cam) {
			cam = SceneManager::Object::Create<GameObject>()->AddComponent<Camera>();
			return 0;
		}
		bgm = AudioManager::CloneByName("title");
		bgm->PlayOneShot();
		return 0;
	}

	void SampleAnotherScene::Update()
	{
		if (loading_status == LOADING_STATUS::LOADING) {
			loading_status = TextureManager::GetLoadingCount() + ModelManager::GetLoadingCount() + AudioManager::GetLoadingCount() == 0 ? LOADING_STATUS::LOADED : LOADING_STATUS::LOADING;
			if (loading_status == LOADING_STATUS::LOADED)
				Init();
			else
				return;
		}

		//とりあえず技術検証用に裏シーンとのやり取り
		if (Input::GetKeyDown(KeyCode::Space)) {
			dice_scene->Clear();
			dice_scene->Roll(5, 100);
		}
		if (Input::GetKeyDown(KeyCode::Alpha1)) {
			dice_scene->Skip();
			results.clear();
			results = dice_scene->FetchResults();
			//std::sort(results.begin(), results.end(), [](int a, int b) {return a < b; });
		}

	}
	void SampleAnotherScene::LateDraw()
	{
		//タイトル画面の描画
		if (title_img)
			DrawExtendGraph(0, 0, SCREEN_W, SCREEN_H, *title_img, false);

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
			DrawExtendGraph(0, 0, SCREEN_W, SCREEN_H, *SafeStaticCast<Camera>(dice_scene->GetCurrentCamera().lock())->hdr, true);
			//とりあえず1でスキップ&結果集計
			int i = 0;
			int sum = 0;
			for (auto& ite : results) {
				DrawFormatString(0, i * 16 + 40, Color::CYAN, "%d,", ite);
				i++;
				sum += ite;
			}

		}
		DrawString(0, 0, u8"スペースキーでサイコロを振る(5D100)、1キーで結果を取得して表示", Color::YELLOW);
		DrawString(0, 16, u8"裏シーンとのデータのやり取りの技術検証用シーンです", Color::YELLOW);

	}

	void SampleAnotherScene::UnLoad()
	{
		bgm = nullptr;
		title_img.reset();
	}

}