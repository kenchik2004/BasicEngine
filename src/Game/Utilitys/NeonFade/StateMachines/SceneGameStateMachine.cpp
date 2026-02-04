#include "SceneGameStateMachine.h"
#include "Game/Scenes/NeonFade/SceneGame.h"
#include "Game/Utilitys/NeonFade/States/SceneStates/SceneGameState_KI.h"
#include "Game/Utilitys/NeonFade/States/SceneStates/SceneGameState_Show.h"
#include "Game/Utilitys/NeonFade/States/SceneStates/SceneGameState_Ten.h"
#include "Game/Utilitys/NeonFade/States/SceneStates/SceneGameState_Ketsu.h"
#include "Game/Utilitys/NeonFade/States/SceneStates/SceneGameState_Fin.h"

namespace NeonFade {
	SceneGameStateMachine::SceneGameStateMachine(SceneGame* owner_scene_)
		:ISceneStateMachine(static_cast<Scene*>(owner_scene_))
	{
		owner_scene_game = owner_scene_;
		auto ki_state = make_safe_unique<SceneGameState_KI>(owner_scene_game);
		auto show_state = make_safe_unique<SceneGameState_Show>(owner_scene_game);
		auto ten_state = make_safe_unique<SceneGameState_Ten>(owner_scene_game);
		auto ketsu_state = make_safe_unique<SceneGameState_Ketsu>(owner_scene_game);
		auto fin_state = make_safe_unique<SceneGameState_Fin>(owner_scene_game);

		AddState("KI", std::move(ki_state));

		std::function<bool()> show_to_ten = [this]() {
			return owner_scene_game->GetEnemyCount() == 0;
			};
		show_state->RegisterChangeRequest("Ten", show_to_ten, 0);
		AddState("Show", std::move(show_state));


		std::function<bool()> ketsu_to_fin = [this]() {
			return owner_scene_game->GetEnemyCount() == 0;
			};

		ketsu_state->RegisterChangeRequest("Fin", ketsu_to_fin, 0);
		AddState("Ten", std::move(ten_state));
		AddState("Ketsu", std::move(ketsu_state));
		AddState("Fin", std::move(fin_state));

		ChangeState("KI");

	}
}