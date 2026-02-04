#include "SceneGameState_Ten.h"
#include "Game/Scenes/NeonFade/SceneGame.h"
#include "Game/Objects/NeonFade/Enemy.h"

namespace NeonFade {
	SceneGameState_Ten::SceneGameState_Ten(SceneGame* owner_scene_)
		:ISceneState(static_cast<Scene*>(owner_scene_))
	{
		std::function<bool()> ten_to_ketsu = [this]() {
			return exit_timer > EXIT_TIME;
			};
		RegisterChangeRequest("Ketsu", ten_to_ketsu, 0);
		owner_scene_game = owner_scene_;
		message_text = owner_scene_game->ui_texts["txt_message"]->GetComponent<Text>();
	}
	void SceneGameState_Ten::OnEnter(ISceneStateMachine* machine)
	{
		exit_timer = 0;
		owner_scene_game->text_comp->SetText(u8"Ten State");
		message_text->WakeUp();
		message_text->SetFontSize(70);
		message_text->TextColor() = Color::YELLOW;
		message_text->SetText(u8"注意!!敵が多数接近中!!");
		{
			auto panel = SceneManager::Object::Create<UIObject>(u8"fullscreen_panel");
			panel->SetPriority(1999U);
			panel->BackGroundColor() = Color::RED;
			panel->UseBackGround() = true;
			panel->transform->scale = Vector3(SCREEN_W, SCREEN_H, 1);
			fullscreen_panel = panel;
		}
	}
	void SceneGameState_Ten::OnExit(ISceneStateMachine* machine)
	{
		message_text->Sleep();
		if (fullscreen_panel.lock()) {
			SceneManager::Object::Destroy(fullscreen_panel.lock());
			fullscreen_panel.reset();
		}
	}
	void SceneGameState_Ten::Update(ISceneStateMachine* machine, float dt)
	{
		exit_timer += dt;
		if(exit_timer > EXIT_TIME - 1.0f) {
			message_text->TextColor().a = 1.0f - (exit_timer - (EXIT_TIME - 1.0f));
		}
		if (fullscreen_panel.lock()) {
			auto panel = fullscreen_panel.lock();
			auto& color = panel->BackGroundColor();
			color.a = (sinf(Time::GetTimeFromStart() * 5) + 1.0f) * 0.25f;
		}
	}
}