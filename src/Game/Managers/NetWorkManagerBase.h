//---------------------------------------------------------------------------
//! @file   NetWorkManagerBase.h
//! @brief  ネットワークマネージャー基底クラス
//---------------------------------------------------------------------------
#pragma once

#include <mutex>

// パケット種別
//パケット種数が増えたら適宜追加してください
enum PacketType : u32 {
	PACKET_TYPE_WAVE = 0,
	PACKET_TYPE_TEXT = 1,
	PACKET_TYPE_PLAYER_POSITION = 2,
	PACKET_TYPE_COMMAND = 3,
};
// IP -> 32bit キー化（同一送信元識別に使用）
static inline u32 MakeIPKey(const IPDATA& ip) {
	return (static_cast<u32>(ip.d1) << 24) | (static_cast<u32>(ip.d2) << 16) |
		(static_cast<u32>(ip.d3) << 8) | static_cast<u32>(ip.d4);
}

struct PacketHeader {
	IPDATA ip;
	u32  type;      // PacketType
	u32 sizeBytes; // 後続ペイロードサイズ
};

//---------------------------------------------------------------------
//! @class NetWork
//! @brief TCP通信を行うクラス
//---------------------------------------------------------------------
// TCP通信を行うためのクラス
class NetWork {
public:
	int handle = -1; //!< ソケットハンドル
	std::array<u8, 65536> buffer; //!< 受信バッファ
	IPDATA ip; //!< 接続先のIPアドレス
	const unsigned long long unique_id; //!< ユニークID
	std::function<void(void* data, size_t length)> on_receive; //!< 受信コールバック
	std::function<void(NetWork*)> on_disconnect; //!< 切断コールバック
	//! @brief データを送信する
	void Send(const void* data, size_t data_size);

	NetWork(int handle, IPDATA other_ip, unsigned long long id) :unique_id(id) {
		this->handle = handle;
		this->ip = other_ip;
		buffer.fill(0);
	}
	~NetWork();
};

//---------------------------------------------------------------------
//! @class UDPNetWork
//! @brief UDP通信を行うクラス
//---------------------------------------------------------------------
// UDP通信を行うためのクラス
class UDPNetWork {

	//UDP通信は、TCPと違い、接続という概念がない
	//そのため、on_disconnectのようなコールバックは存在しない
	//また、handleも存在しない(ソケットは1つだけ)
public:
	int socket = -1; //!< UDPソケット
	unsigned short port = 0; //!< ポート番号
	std::array<u8, 65536> buffer; //!< 受信バッファ

	std::function<void(void* data, size_t length)> on_receive; //!< 受信コールバック
	//! @brief データを送信する
	void Send(IPDATA ip, unsigned short port, const void* data, size_t data_size) const;
	UDPNetWork(int socket, unsigned short port) {
		this->socket = socket;
		this->port = port;
		buffer.fill(0);
	}
	~UDPNetWork() {
		if (socket != -1) {
			DxLib::DeleteUDPSocket(socket);
		}
	}

};

//---------------------------------------------------------------------
//! @class NetWorkManagerBase
//! @brief ネットワーク管理の基底クラス
//---------------------------------------------------------------------
class NetWorkManagerBase
{
protected:
	std::mutex mutex_; //!< スレッド同期用ミューテックス
	std::vector<std::unique_ptr<NetWork>> networks; //!< TCP接続リスト
	std::unique_ptr<UDPNetWork> udp_network; //!< UDP通信オブジェクト
	std::function<void(NetWork*)> on_new_connection; //!< 新規接続コールバック
	std::function<void(NetWork*)> on_disconnection; //!< 切断コールバック
	std::thread check_connection_thread; //!< 接続確認スレッド
	std::thread check_disconnection_thread; //!< 切断確認スレッド
	IPDATA my_ip = { 127,0,0,1 }; //!< 自IPアドレス
	unsigned short port_num = 0; //!< TCPポート番号
	unsigned short udp_port_num = 0; //!< UDPポート番号


private:
	bool kill_thread_flag = false; //!< スレッド終了フラグ
	//! @brief 新規接続を監視する
	void CheckForNewConnect(const bool& finish_flag);
	//! @brief 切断を監視する
	void CheckForDisConnect(const bool& finish_flag);
public:
	//! @brief 新規接続コールバックを設定する
	void SetOnNewConnectionCallback(std::function<void(NetWork*)> func) { on_new_connection = func; }
	//! @brief 切断コールバックを設定する
	void SetOnDisconnectionCallback(std::function<void(NetWork*)> func) { on_disconnection = func; }
	//! @brief 更新処理
	void Update();
	//! @brief 指定アドレスへ接続する
	NetWork* Connect(IPDATA other, unsigned short port,
		std::function<void(NetWork*)> on_connect = nullptr,
		std::function<void(NetWork*)> on_disconnect = nullptr);
	NetWorkManagerBase(int mode = 0, unsigned short port = 35000);
	virtual ~NetWorkManagerBase() {
		kill_thread_flag = true;	// スレッド終了フラグを立てる
		// スレッド終了まで待機
		{
			if (check_connection_thread.joinable())
				check_connection_thread.join();
			if (check_disconnection_thread.joinable())
				check_disconnection_thread.join();
		}
	};
	//! @brief UDPソケットを開く
	UDPNetWork* OpenUDPSocket(unsigned short port = 35001);
	//! @brief UDPソケットを取得する
	UDPNetWork* GetUDPSocket() const { return udp_network.get(); }
	//! @brief 自IPアドレスを取得する
	const IPDATA& GetMyIP() const { return my_ip; }
	//! @brief TCPポート番号を取得する
	const unsigned short& GetPort() const { return port_num; }
	//! @brief UDPポート番号を取得する
	const unsigned short& GetUDPPort() const { return udp_port_num; }
	//! @brief パケットを生成する
	std::vector<char> CreatePacket(PacketType type, const void* payload, u32 sizeBytes, IPDATA overrided_ip = { 0,0,0,0 });
public:
	static constexpr int NETWORK_MANAGER_MODE_LISTEN = 0; //!< 受信専用モード
	static constexpr int NETWORK_MANAGER_MODE_CONNECT = 1; //!< 接続専用モード
	static constexpr int NETWORK_MANAGER_MODE_BOTH = 2; //!< 送受信両用モード

};
// IPDATA同士の比較(DxLibにはなぜか存在しない)
constexpr bool operator== (const IPDATA& lhs, const IPDATA& rhs) {
	return (lhs.d1 == rhs.d1) && (lhs.d2 == rhs.d2) && (lhs.d3 == rhs.d3) && (lhs.d4 == rhs.d4);
}
