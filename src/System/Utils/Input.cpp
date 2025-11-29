//----------------------------------------------------------------------------
//!	@file	Input.cpp
//!	@brief	入力関連の実装。Input 名前空間の関数を実装し、入力を更新して状態を取得。
//!　@author　岩野 健太郎
//----------------------------------------------------------------------------

#include "Input.h"

namespace Input {

	// 現在のキー状態バッファ
	char key_buffer[256];
	// 前のフレームのキー状態バッファ
	char key_buffer_prev[256];
	// 現在のマウスボタン状態
	unsigned int mouse_buffer;
	// 前のフレームのマウスボタン状態
	unsigned int mouse_buffer_prev;
	// ゲームパッドの状態バッファ
	std::vector<std::pair<XINPUT_STATE, XINPUT_STATE>> pad_buffers;

	// 現在のマウス位置
	Vector2 mouse_pos(0.0f, 0.0f);
	// 前のフレームのマウス位置
	Vector2 mouse_pos_prev(0.0f, 0.0f);

	//----------------------------------------------------
	// @brief 入力を初期化する。
	//----------------------------------------------------
	void Init()
	{
		// キー状態を初期化
		for (int i = 0; i < 256; i++) {
			key_buffer[i] = 0;
			key_buffer_prev[i] = 0;
		}
		mouse_buffer = 0;
		mouse_buffer_prev = 0;
	}

	//----------------------------------------------------
	// @brief 入力を更新する。
	//----------------------------------------------------
	void Update()
	{
		// マウス位置を更新
		mouse_pos_prev = mouse_pos;
		mouse_pos = GetMousePosition();
		// キー状態をコピー
		memcpy(key_buffer_prev, key_buffer, sizeof(char) * 256);
		mouse_buffer_prev = mouse_buffer;

		// キー状態を取得
		GetHitKeyStateAll(key_buffer);
		mouse_buffer = 0;
		mouse_buffer |= GetMouseInput();

		// 毎フレーム接続されているコントローラーの数を取得し、バッファをリサイズ
		u64 pad_nums = GetJoypadNum();
		if (pad_buffers.size() != pad_nums)
			pad_buffers.resize(pad_nums);
		// 各コントローラーの状態を取得
		for (u64 i = 0; i < pad_nums; i++) {
			pad_buffers[i].first = pad_buffers[i].second;
			XINPUT_STATE state;
			GetJoypadXInputState(DX_INPUT_PAD1, &state);
			//DxLibにはなぜか10,11のボタンが未定義なので、とりあえずトリガーの情報を入れておく
			state.Buttons[static_cast<u8>(PadButton::LTrigger)] = state.LeftTrigger > 0;
			state.Buttons[static_cast<u8>(PadButton::RTrigger)] = state.RightTrigger > 0;
			pad_buffers[i].second = state;

		}
	}

	//----------------------------------------------------
	// @brief 指定したキーが押された瞬間かどうかを取得する。
	// @param key チェックするキー。
	// @return 押された瞬間ならtrue。
	//----------------------------------------------------
	bool GetKeyDown(KeyCode key)
	{
		return key_buffer[(unsigned char)key] == 1 && key_buffer_prev[(unsigned char)key] == 0;
	}
	//----------------------------------------------------
	// @brief 指定したキーが押されているかどうかを取得する。
	// @param key チェックするキー。
	// @return 押されているならtrue。
	//----------------------------------------------------
	bool GetKey(KeyCode key)
	{
		return key_buffer[(unsigned char)key] == 1 && key_buffer_prev[(unsigned char)key] == 1;
	}

	//----------------------------------------------------
	// @brief 指定したキーが離された瞬間かどうかを取得する。
	// @param key チェックするキー。
	// @return 離された瞬間ならtrue。
	//----------------------------------------------------
	bool GetKeyUp(KeyCode key) {
		return key_buffer[(unsigned char)key] == 0 && key_buffer_prev[(unsigned char)key] == 1;
	}

	//----------------------------------------------------
	// @brief マウスが押されているかを取得する。
	// @param button チェックするボタン。
	// @return 押されているならtrue。
	//----------------------------------------------------
	bool CheckMouseInput(int button)
	{
		if (GetMouseInput() & button) {
			return true;
		}
		return false;
	}

	//----------------------------------------------------
	// @brief 指定したマウスボタンが押された瞬間かどうかを取得する。
	// @param button チェックするマウスボタン。
	// @return 押された瞬間ならtrue。
	//----------------------------------------------------
	bool GetMouseButtonDown(MouseButton button)
	{
		return mouse_buffer & (unsigned char)button & (~mouse_buffer_prev);
	}

	//----------------------------------------------------
	// @brief 指定したマウスボタンが押されているかどうかを取得する。
	// @param button チェックするマウスボタン。
	// @return 押されているならtrue。
	//----------------------------------------------------
	bool GetMouseButtonRepeat(MouseButton button)
	{
		return mouse_buffer_prev & mouse_buffer & (unsigned char)button;
	}

	//----------------------------------------------------
	// @brief 指定したマウスボタンが離された瞬間かどうかを取得する。
	// @param button チェックするマウスボタン。
	// @return 離された瞬間ならtrue。
	//----------------------------------------------------
	bool GetMouseButtonUp(MouseButton button)
	{
		return mouse_buffer_prev & (~mouse_buffer) & (unsigned char)button;
	}
	//----------------------------------------------------
	// @brief 指定したゲームパッドのボタンが押された瞬間かどうかを取得する。
	// @param pad_index ゲームパッドのインデックス。
	// @param button チェックするボタン。
	// @return 押された瞬間ならtrue。
	//----------------------------------------------------
	bool GetPadButtonDown(u64 pad_index, PadButton button)
	{
		if (pad_buffers.size() <= pad_index)
			return false;
		return pad_buffers[pad_index].second.Buttons[(unsigned char)button] == 1 &&
			pad_buffers[pad_index].first.Buttons[(unsigned char)button] == 0;
	}

	//----------------------------------------------------
	// @brief 指定したゲームパッドのボタンが押されているかどうかを取得する。
	// @param pad_index ゲームパッドのインデックス。
	// @param button チェックするボタン。
	// @return 押されているならtrue。
	//----------------------------------------------------
	bool GetPadButtonRepeat(u64 pad_index, PadButton button)
	{
		if (pad_buffers.size() <= pad_index)
			return false;
		return pad_buffers[pad_index].second.Buttons[(unsigned char)button] == 1 &&
			pad_buffers[pad_index].first.Buttons[(unsigned char)button] == 1;
	}

	//----------------------------------------------------
	// @brief 指定したゲームパッドのボタンが離された瞬間かどうかを取得する。
	// @param pad_index ゲームパッドのインデックス。
	// @param button チェックするボタン。
	// @return 離された瞬間ならtrue。
	//----------------------------------------------------
	bool GetPadButtonUp(u64 pad_index, PadButton button)
	{
		if (pad_buffers.size() <= pad_index)
			return false;
		return pad_buffers[pad_index].second.Buttons[(unsigned char)button] == 0 &&
			pad_buffers[pad_index].first.Buttons[(unsigned char)button] == 1;
	}

	//----------------------------------------------------
	// @brief 指定したゲームパッドの左スティックの座標を取得する。
	// @param pad_index ゲームパッドのインデックス。
	// @return 左スティックの座標。
	//----------------------------------------------------
	Vector2 GetPadLeftStick(u64 pad_index)
	{
		if (pad_buffers.size() <= pad_index)
			return { 0,0 };
		constexpr float denominator = 1.0f / 32768.0f;
		return Vector2(
			static_cast<float>(pad_buffers[pad_index].second.ThumbLX) * denominator,
			static_cast<float>(pad_buffers[pad_index].second.ThumbLY) * denominator
		);
	}

	//----------------------------------------------------
	// @brief 指定したゲームパッドの右スティックの座標を取得する。
	// @param pad_index ゲームパッドのインデックス。
	// @return 右スティックの座標。
	//----------------------------------------------------
	Vector2 GetPadRightStick(u64 pad_index)
	{
		if (pad_buffers.size() <= pad_index)
			return { 0,0 };
		constexpr float denominator = 1.0f / 32768.0f;
		return Vector2(
			static_cast<float>(pad_buffers[pad_index].second.ThumbRX) * denominator,
			static_cast<float>(pad_buffers[pad_index].second.ThumbRY) * denominator
		);
	}

	//----------------------------------------------------
	// @brief マウスの座標を取得する。
	// @return マウスの座標。
	//----------------------------------------------------
	Vector2 GetMousePosition()
	{
		int mouse_x;
		int mouse_y;
		GetMousePoint(&mouse_x, &mouse_y);
		return Vector2(static_cast<float>(mouse_x), static_cast<float>(mouse_y));
	}

	//----------------------------------------------------
	// @brief マウスの座標を設定する。
	// @param new_pos 新しい座標。
	// @param reset_delta デルタをリセットするかどうか。
	//----------------------------------------------------
	void SetMousePosition(Vector2 new_pos, bool reset_delta) {
		SetMousePoint(static_cast<int>(new_pos.x), static_cast<int>(new_pos.y));
		mouse_pos = GetMousePosition();
		if (reset_delta)
			mouse_pos_prev = mouse_pos;
	}

	//----------------------------------------------------
	// @brief マウスの移動量を取得する。
	// @return マウスの移動量。
	//----------------------------------------------------
	Vector2 GetMouseDelta()
	{
		return mouse_pos - mouse_pos_prev;
	}

}