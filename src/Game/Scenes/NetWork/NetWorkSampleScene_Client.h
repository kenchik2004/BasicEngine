#pragma once
#include "System/Scene.h"

#include "Game/Managers/NetWorkManagerBase.h"

namespace NetWorkTest_Client {

	class NetWorkSampleScene_Client :
		public Scene
	{
	public:
		USING_SUPER(NetWorkSampleScene_Client);
		void Load() override;
		int Init() override;
		void Update() override;
		void PreDraw() override;
		void LateDraw() override;
		void Exit() override;
		bool is_connected = false;
	private:

		void ProcessReceivedBytes(u8* data, size_t length);
		void ProcessReceivedUDPBytes(u8* data, size_t length);




		SafeUniquePtr<NetWorkManagerBase> net_manager;
		std::vector<NetWork*> chat_network;
		UDPNetWork* udp_network = nullptr;


		//-----------------------------
		//自分のプレイヤー関係
		std::string input_text = "";
		GameObjectWP player;
		GameObjectWP camera;


		float camera_distance = 20.0f;
		float camera_rot_y = 0.0f;
		//-----------------------------
		//他のプレイヤー関係

		std::vector<std::pair<u32, std::string>> received_text;
		std::vector<std::pair<u32, GameObjectWP>> another_players;

	};
};

