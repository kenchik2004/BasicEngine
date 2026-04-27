//---------------------------------------------------------------------------
//! @file   SceneGameState_Over.cpp
//! @brief  SceneGameState_Overの実装。ゲームオーバー時のシーン状態処理を行う
//---------------------------------------------------------------------------
#include "SceneGameState_Over.h"
#include "Game/Scenes/NeonFade/SceneGame.h"
namespace NeonFade {
	class GameOverEffect {
	public:

		GameOverEffect() {
			auto owner_scene = SceneManager::GetScene<SceneGame>();
			owner_scene_game = owner_scene.get();
			CreateBackgroundEffect(owner_scene_game);
		}
		~GameOverEffect() {
			if (back_ground_img_obj)
				SceneManager::Object::Destroy(back_ground_img_obj.lock());
			if (game_over_text_obj)
				SceneManager::Object::Destroy(game_over_text_obj.lock());
			if (txt_navi_obj)
				SceneManager::Object::Destroy(txt_navi_obj.lock());
		}

		bool IsEffectFinished() const {
			return effect_timer > txt_navi_delay + 1.0f; //ナビテキストが完全に表示されてからエフェクト終了とする
		}

		void Update(const float dt) {
			effect_timer += dt;
			back_ground_img_obj.lock()->BackGroundColor().a = min(max(effect_timer, 0.0f), effect_full_alpha_time);
			if (effect_timer > txt_game_over_delay && !game_over_text_obj)
			{
				CreateGameOverText(owner_scene_game);
			}
			if (effect_timer > txt_navi_delay && !txt_navi_obj)
			{
				CreateNaviText(owner_scene_game);
			}
			UpdateTextsAlpha();

		}
	private:
		UIObjectWP back_ground_img_obj;
		UIObjectWP game_over_text_obj;
		UIObjectWP txt_navi_obj;
		TextWP game_over_text_comp;
		TextWP txt_navi_text_comp;
		float effect_timer = 0;
		static constexpr float txt_game_over_delay = 1.0f;
		static constexpr float txt_navi_delay = 2.0f;
		static constexpr float effect_full_alpha_time = 1.0f;

		SceneGame* owner_scene_game = nullptr;

		void CreateBackgroundEffect(SceneGame* owner_scene) {
			//半透明で背景を覆うオブジェクトの生成
			//ただの黒背景(半透明)
			{
				auto bg_obj = SceneManager::Object::Create<UIObject>(owner_scene->shared_from_this());
				bg_obj->transform->scale = { 1920,1080,1 };
				bg_obj->BackGroundColor() = Color(0.1f, 0.1f, 0.1f, 0.0f);
				bg_obj->UseBackGround() = true;
				back_ground_img_obj = bg_obj;
			}
		}
		void CreateGameOverText(SceneGame* owner_scene) {
			auto game_over_obj = SceneManager::Object::Create<UIObject>(owner_scene->shared_from_this());
			game_over_obj->name = "GameOverText";
			game_over_obj->AnchorType() = UIObject::ANCHOR_TYPE::CENTER;
			game_over_obj->CanvasAnchorType() = UIObject::ANCHOR_TYPE::CENTER;
			game_over_obj->transform->scale = { 800, 200, 1 };
			game_over_obj->transform->position.y += 100;
			auto text_comp = game_over_obj->AddComponent<Text>();
			text_comp->SetText(u8"Time is Over...");
			text_comp->TextColor() = Color::RED;
			text_comp->SetFontSize(120);
			game_over_text_comp = text_comp;
			game_over_text_obj = game_over_obj;
		}
		void CreateNaviText(SceneGame* owner_scene) {
			auto navi_obj = SceneManager::Object::Create<UIObject>(owner_scene->shared_from_this());
			navi_obj->name = "NaviText";
			navi_obj->AnchorType() = UIObject::ANCHOR_TYPE::CENTER;
			navi_obj->CanvasAnchorType() = UIObject::ANCHOR_TYPE::CENTER;
			navi_obj->transform->scale = { 600, 150, 1 };
			navi_obj->transform->position.y -= 200;
			auto text_comp = navi_obj->AddComponent<Text>();
			text_comp->SetText(u8"EnterキーまたはBボタンを押してリスタート");
			text_comp->TextColor() = Color::WHITE;
			text_comp->SetFontSize(60);
			txt_navi_text_comp = text_comp;
			txt_navi_obj = navi_obj;
		}
		void UpdateTextsAlpha() {
			if (game_over_text_comp)
			{
				float alpha = min(max(effect_timer - txt_game_over_delay, 0.0f), 1);
				Color color = game_over_text_comp->TextColor();
				color.a = alpha;
				game_over_text_comp->TextColor() = color;
			}
			if (txt_navi_text_comp)
			{
				float alpha = min(max(effect_timer - txt_navi_delay, 0.0f), 1);
				Color color = txt_navi_text_comp->TextColor();
				color.a = alpha;
				txt_navi_text_comp->TextColor() = color;
			}
		}
	};
	SafeUniquePtr<GameOverEffect> game_over_effect;
	SceneGameState_Over::SceneGameState_Over(SceneGame* owner_scene_)
		:ISceneState(static_cast<Scene*>(owner_scene_))
	{
		owner_scene_game = owner_scene_;
		std::function<bool()> over_to_ki = [this]() {
			return is_exit_requested;
			};
		RegisterChangeRequest("KI", over_to_ki, 0);

	}
	void SceneGameState_Over::OnEnter(ISceneStateMachine* machine)
	{
		owner_scene_game->text_comp->SetText(u8"Over State");
		game_over_effect = make_safe_unique<GameOverEffect>();
		owner_scene_game->ClearAllEnemy();
	}
	void SceneGameState_Over::OnExit(ISceneStateMachine* machine)
	{
		game_over_effect.reset();
		is_exit_requested = false;

		owner_scene_game->StopGameTimer();
		owner_scene_game->ResetGameTimer();
	}
	void SceneGameState_Over::Update(ISceneStateMachine* machine, float dt)
	{
		game_over_effect->Update(dt);
		if (game_over_effect->IsEffectFinished()) {
			if (Input::GetKeyDown(KeyCode::Return) || Input::GetPadButtonDown(0, PadButton::Button1))
			{
				is_exit_requested = true;
			}
		}
	}
}