#include "precompile.h"
#include "SceneTitle.h"
#include "Game/Scene/DiceScene.h"


namespace RLyeh {

	void SceneTitle::Load()
	{
		AudioManager::Load("data/Sound/title.mp3", "title");
		title_img = LoadGraph("data/title.jpg");
		loading_status = ModelManager::GetLoadingCount() + AudioManager::GetLoadingCount() == 0 ? LOADING_STATUS::LOADED : LOADING_STATUS::LOADING;
		dice_scene = SceneManager::LoadAsAnother<DiceScene>();
	}

	int SceneTitle::Init()
	{
		loading_status = ModelManager::GetLoadingCount() + AudioManager::GetLoadingCount() == 0 ? LOADING_STATUS::LOADED : LOADING_STATUS::LOADING;
		if (loading_status == LOADING_STATUS::LOADING)
			return 0;
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
		if (Input::GetKeyDown(KeyCode::Space))
			dice_scene->Roll(1, 4);
	}

	void SceneTitle::LateDraw()
	{
		//タイトル画面の描画
		DrawExtendGraph(0, 0, SCREEN_W, SCREEN_H, title_img, false);

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
			DrawExtendGraph(0, 0, SCREEN_W, SCREEN_W, dice_scene->screen, true);
			//デカデカとタイトル表示
			SetFontSize(150);
			int x, y;
			GetDrawStringSize(&x, &y, nullptr, "  THE\nR'LYEH", 13);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, fabsf(sinf(Time::GetTimeFromStart()) * 255));
			DrawString(SCREEN_W * 0.5f - x * 0.5f, SCREEN_H * 0.5f - y * 0.5f, "  THE\nR'LYEH", Color::RED);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			SetFontSize(DEFAULT_FONT_SIZE);
		}
	}

	void SceneTitle::UnLoad()
	{
		bgm = nullptr;
		DeleteGraph(title_img);
	}

}