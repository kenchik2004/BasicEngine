#include "OptionStart.h"
#include "Game/Scenes/NeonFade/SceneGame.h"
#include "Game/Objects/NeonFade/TitlePlayer.h"
#include "Game/Utilitys/NeonFade/StateMachines/TitlePlayerStatemachine.h"

namespace NeonFade {

	int OptionStart::Init() {
		int ret = Super::Init();
		SetText(u8"GAME START");
		on_decide = [this]() {
			if (disable)
				return;
			auto player = SceneManager::Object::Get<TitlePlayer>(GetScene());
			player->GetStateMachine()->ChangeState("player_flip");
			SceneGame::LoadResources(); // ゲームシーンのリソースを先読みする
			disable = true;
			enable_timer = -1.0f; // タイマーをリセットして、選択不可状態にする
			};
		disable = false;
		enable_timer = 0.0f; // タイマーを初期化 
		return ret;
	}

	void OptionStart::Update()
	{
		if (enable_timer >= 0.0)
			enable_timer += Time::DeltaTime();
		if (disable && enable_timer > enable_delay) {
			disable = false;
		}
	}

	void OptionStart::Exit()
	{
		disable = false;
		enable_timer = 0.0f; // タイマーを初期化 
	}

}
