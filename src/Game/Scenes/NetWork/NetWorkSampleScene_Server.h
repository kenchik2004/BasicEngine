#pragma once
#include "System/Scene.h"

#include "Game/Managers/NetWorkManagerBase.h"

namespace NetWorkTest_Server {

	class NetWorkSampleScene_Server :
		public Scene
	{
	public:
		USING_SUPER(NetWorkSampleScene_Server);
		int Init() override;
		void Update() override;
		void LateDraw() override;
		void Exit() override;
	private:
		void OpenUDPSocket();
		void OpenTCPSocket();


		void ProcessReceivedUDPBytes(u8* data, size_t length);
		void ProcessReceivedBytes(u8* data, size_t length);
		//-----------------------------
		//ネットワーク関係

		unsigned short open_port = 11451;
		unsigned short open_udp_port = 11453;
		SafeUniquePtr<NetWorkManagerBase> net_manager;
		std::vector<NetWork*> chat_network;
		UDPNetWork* udp_network = nullptr;

		//-----------------------------
		//自分のプレイヤー関係
		std::string input_text = "";
		GameObjectWP player;




		//-----------------------------
		//他のプレイヤー関係
		std::vector<std::pair<u32, GameObjectWP>> another_players;
		std::vector<std::pair<u32, std::string>> received_text;
		std::vector<IPDATA> another_clients;

	};
};

