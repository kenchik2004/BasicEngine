#pragma once
#include "ISceneStateMachine.h"
namespace NeonFade {
	class SceneGame;
	class SceneGameStateMachine :
		public ISceneStateMachine
	{
	public:
		SceneGameStateMachine(SceneGame* owner_scene_);
	private:
		SceneGame* owner_scene_game = nullptr;
	};
}

