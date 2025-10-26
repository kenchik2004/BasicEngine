#pragma once
#include "System/Scene.h"

namespace Sample {
	class SampleRoomScene :
		public Scene
	{
	public:
		USING_SUPER(SampleRoomScene);
		void Load() override;
		int Init() override;
		void Update() override;
		void LateDraw() override;
		void Exit() override;
	private:
		bool CheckForLoading(); //!< ロード状況の確認
		GameObjectWP room_obj;

	};


}