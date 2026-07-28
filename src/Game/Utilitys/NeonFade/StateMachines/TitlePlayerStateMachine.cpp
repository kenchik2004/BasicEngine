// TitlePlayerStateMachine.cpp
#include "TitlePlayerStateMachine.h"
#include "Game/Objects/NeonFade/TitlePlayer.h"
#include "Game/Utilitys/NeonFade/States/TitlePlayerStates/TitlePlayerStartSpawnState.h"
#include "Game/Utilitys/NeonFade/States/TitlePlayerStates/TitlePlayerStartFlipState.h"
#include "Game/Utilitys/NeonFade/States/TitlePlayerStates/TitlePlayerStartDiveState.h"

namespace NeonFade {
	/// @brief タイトルプレイヤー用ステートマシンのコンストラクタ
	/// @param owner_ 所有者となるタイトルプレイヤーへのポインタ
	TitlePlayerStateMachine::TitlePlayerStateMachine(TitlePlayer* owner_)
		:IStateMachine(static_cast<GameObject*>(owner_))
	{
		// 所有者となるプレイヤーオブジェクトをメンバ変数に保持する
		owner_player = owner_;

		// スポーン開始ステートを生成して登録する
		auto start_spawn_state = make_safe_unique<TitlePlayerStartSpawnState>(owner_player);
		AddState("start_spawn", std::move(start_spawn_state));
		// フリップ状態ステートを生成して登録する
		auto start_flip_state = make_safe_unique<TitlePlayerStartFlipState>(owner_player);
		AddState("player_flip", std::move(start_flip_state));
		// ダイブ状態ステートを生成して登録する
		auto start_dive_state = make_safe_unique<TitlePlayerStartDiveState>(owner_player);
		AddState("player_dive", std::move(start_dive_state));

		// 初期状態をスポーン開始ステートに設定する
		ChangeState("start_spawn");
	}
}