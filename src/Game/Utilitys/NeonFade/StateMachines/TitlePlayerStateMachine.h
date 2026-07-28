// TitlePlayerStateMachine.h
#pragma once
#include "IStateMachine.h"

namespace NeonFade {

	class TitlePlayer;
	/// @brief タイトル画面におけるプレイヤーの状態遷移を管理するクラス
	class TitlePlayerStateMachine :
		public IStateMachine
	{
	public:
		/// @brief コンストラクタ
		/// @param owner_ 所有者となるタイトルプレイヤーへのポインタ
		TitlePlayerStateMachine(TitlePlayer* owner_);

	private:
		TitlePlayer* owner_player; /// @brief 所有者であるタイトルプレイヤーオブジェクトへのポインタ
	};

}