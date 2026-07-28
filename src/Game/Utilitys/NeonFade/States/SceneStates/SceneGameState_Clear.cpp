//---------------------------------------------------------------------------
//! @file   SceneGameState_Clear.cpp
//! @brief  SceneGameState_Clearの実装。ゲームクリア時のシーン状態処理を行う
//---------------------------------------------------------------------------
#include "Game/Utilitys/NeonFade/States/SceneStates/SceneGameState_Clear.h"
#include "Game/Scenes/NeonFade/SceneGame.h"
#include "Game/Scenes/NeonFade/SceneTitle.h"

namespace NeonFade {

	class FinishEffect
	{
	private:
		UIObjectWP back_ground_img_obj;
		GameObjectWP player_model_obj;

		UIObjectWP txt_time;
		UIObjectWP txt_rank;
		float effect_timer = 0;
		static constexpr float txt_time_delay = 1.0f;
		static constexpr float txt_rank_delay = 3.0f;
		SafeSharedPtr<AudioClip> score_se;

		static constexpr float slidein_time = 0.1f;


	public:
		USING_SUPER(FinishEffect);
		FinishEffect(SceneGame* owner_scene) {
			if (!score_se)
				score_se = AudioManager::CloneByName(u8"score_se");

			//半透明で背景を覆うオブジェクトの生成
			//裏シーンでプレイヤーをアニメーションさせて、そのカメラの吐き出しバッファを背景に貼る

			{
				auto bg_obj = SceneManager::Object::Create<UIObject>(owner_scene->shared_from_this());
				bg_obj->name = "FinishEffectBackground";
				auto bg_img = bg_obj->AddComponent<ImageRenderer>();
				auto mat = MaterialManager::GetMaterial("hunt_eff_mat");
				bg_img->SetMaterial(mat);
				bg_obj->transform->scale = { 1920,1080,1 };
				bg_obj->BackGroundColor() = Color(0.5f, 0.5f, 0.5f, 0.4f);
				bg_obj->UseBackGround() = true;
				back_ground_img_obj = bg_obj;
				{
					auto pl_finish = SceneManager::Object::Create<GameObject>(SceneManager::GetDontDestoryOnLoadScene());
					auto mdl = pl_finish->AddComponent<ModelRenderer>();
					mdl->SetModel("player_model");
					auto anim = pl_finish->AddComponent<Animator>();
					anim->SetAnimation("jump_cool", 0);
					anim->Play("jump_cool");
					pl_finish->transform->position = { -8,-9,30 };
					pl_finish->transform->rotation = Quaternion(DEG2RAD(-10), { 0,1,0 }) * Quaternion(DEG2RAD(15), { 1,0,0 });
					pl_finish->transform->scale = { 0.05f,0.05f,0.05f };
					player_model_obj = pl_finish;
				}

			}
		}
		void Update(SceneGame* owner_scene) {
			effect_timer += Time::UnscaledDeltaTime();
			float t = std::clamp(effect_timer / slidein_time, 0.0f, 1.0f);
			if (auto bg_obj = back_ground_img_obj.lock())
			{
				bg_obj->transform->position.x = SCREEN_W * (1.0f - t);
			}

			if (effect_timer > txt_time_delay && !txt_time.lock())
			{
				auto time_text_obj = SceneManager::Object::Create<UIObject>(owner_scene->shared_from_this());
				time_text_obj->BackGroundColor() = Color(0.2f, 0.2f, 0.2f, 0.5f);
				time_text_obj->UseBackGround() = true;
				time_text_obj->name = "TimeText";
				time_text_obj->AnchorType() = UIObject::ANCHOR_TYPE::RIGHT_MIDDLE;
				time_text_obj->CanvasAnchorType() = UIObject::ANCHOR_TYPE::RIGHT_MIDDLE;
				time_text_obj->transform->scale = { 500, 500, 1 };

				{

					float game_time = owner_scene->GetGameTimer();
					int minutes = static_cast<int>(game_time) / 60;
					float seconds = game_time - minutes * 60;


					std::string time_str = u8"かかった時間:\n " + std::format("{:d}:{:.02f}\n", minutes, seconds);
					auto text_comp = time_text_obj->AddComponent<Text>();
					text_comp->SetText(time_str);
					text_comp->TextColor() = Color::BLACK;
					text_comp->SetFontSize(45);

				}
				txt_time = time_text_obj;
			}
			if (effect_timer > txt_rank_delay && !txt_rank.lock())
			{
				float game_time = owner_scene->GetGameTimer();
				std::string rank_str;
				Color rank_color;
				if (game_time < 120.0f) {
					rank_str = u8"ランク: S";
					rank_color = Color::YELLOW;
				}
				else if (game_time < 180.0f) {
					rank_str = u8"ランク: A";
					rank_color = Color::RED;
				}
				else if (game_time < 270.0f) {
					rank_str = u8"ランク: B";
					rank_color = Color::GREEN;
				}
				else {
					rank_str = u8"ランク: C";
					rank_color = Color::BLUE;
				}
				auto rank_text_obj = SceneManager::Object::Create<UIObject>(owner_scene->shared_from_this());
				rank_text_obj->name = "RankText";
				rank_text_obj->AnchorType() = UIObject::ANCHOR_TYPE::RIGHT_MIDDLE;
				rank_text_obj->CanvasAnchorType() = UIObject::ANCHOR_TYPE::RIGHT_MIDDLE;
				rank_text_obj->transform->position.y -= 200;
				rank_text_obj->transform->scale = { 500, 200, 1 };
				auto text_comp = rank_text_obj->AddComponent<Text>();
				text_comp->SetText(rank_str);
				text_comp->TextColor() = Color::RED;
				text_comp->SetFontSize(70);
				txt_rank = rank_text_obj;
				score_se->PlayOneShot(SceneGame::GetSEVolume());
			}
		}
		~FinishEffect() {
			if (back_ground_img_obj)
				SceneManager::Object::Destroy(back_ground_img_obj.lock());
			if (player_model_obj)
				SceneManager::Object::Destroy(player_model_obj.lock());
			if (txt_time)
				SceneManager::Object::Destroy(txt_time.lock());
			if (txt_rank)
				SceneManager::Object::Destroy(txt_rank.lock());
		}
	};


	SceneGameState_Clear::SceneGameState_Clear(SceneGame* owner_scene_)
		:ISceneState(static_cast<Scene*>(owner_scene_))
	{
		owner_scene_game = owner_scene_;
		std::function<bool()> fin_to_restart = [this]() {
			return Input::GetKeyDown(KeyCode::Return) || Input::GetPadButtonDown(0, PadButton::Button2);
			};
		RegisterChangeRequest("KI", fin_to_restart, 0);
		if (!fin_se)
			fin_se = AudioManager::CloneByName(u8"finish_se");

	}
	void SceneGameState_Clear::OnEnter(ISceneStateMachine* machine)
	{
		exit_timer = 0;
		owner_scene_game->text_comp->SetText(u8"Clear State");

		owner_scene_game->audio_player->audio = AudioManager::CloneByName(u8"result_bgm");
		owner_scene_game->audio_player->loop = true;
		owner_scene_game->audio_player->Play();
		fin_se->PlayOneShot(SceneGame::GetSEVolume());

		if (!finish_effect)
			finish_effect = std::make_shared<FinishEffect>(owner_scene_game);

		owner_scene_game->PauseGame(true);
		//ゲームクリア時は一時停止可能状態を無効化する
		owner_scene_game->SetPauseAvailable(false);


	}
	void SceneGameState_Clear::OnExit(ISceneStateMachine* machine)
	{
		finish_effect.reset();
		owner_scene_game->PauseGame(false);
		auto title_scene = SceneManager::GetScene<SceneTitle>();
		if (title_scene)
		{
			SceneManager::Destroy(title_scene);
		}
		SceneManager::Load<SceneTitle>();
	}
	void SceneGameState_Clear::Update(ISceneStateMachine* machine, float dt)
	{
		exit_timer += dt;
		finish_effect->Update(owner_scene_game);
	}
}
