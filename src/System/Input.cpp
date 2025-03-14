#include "precompile.h"
#include "Input.h"

namespace Input {

	char key_buffer[256];
	int key_frame[256];
	int mouse_l_frame;
	int mouse_r_frame;

	int mouse_x;
	int mouse_y;
	int mouse_x_prev;
	int mouse_y_prev;

	void Init()
	{
		for (int i = 0; i < 256; i++) {
			key_frame[i] = 0;
		}
		mouse_l_frame = 0;
		mouse_r_frame = 0;
	}

	void Update()
	{

		mouse_x_prev = mouse_x;
		mouse_y_prev = mouse_y;

		GetHitKeyStateAll(key_buffer);

		for (int i = 0; i < 256; i++) {
			if (key_buffer[i])	key_frame[i]++;
			else				key_frame[i] = 0;
		}

		if (CheckMouseInput(MOUSE_INPUT_LEFT))	mouse_l_frame++;
		else										mouse_l_frame = 0;

		if (CheckMouseInput(MOUSE_INPUT_RIGHT))	mouse_r_frame++;
		else										mouse_r_frame = 0;

		mouse_x = GetMouseX();
		mouse_y = GetMouseY();
	}

	//---------------------------------------------------------------------------------
	//	キーが押された瞬間を取得する
	//---------------------------------------------------------------------------------
	bool PushHitKey(int key)
	{
		if (key_frame[key] == 1) {
			return true;
		}
		return false;
	}
	bool CheckHitKey(int key)
	{
		return DxLib::CheckHitKey(key);
	}
	//---------------------------------------------------------------------------------
	//	マウスが押されているかを取得する
	//---------------------------------------------------------------------------------
	bool CheckMouseInput(int button)
	{
		if (GetMouseInput() & button) {
			return true;
		}
		return false;
	}
	//---------------------------------------------------------------------------------
	//	マウスが押された瞬間を取得する
	//---------------------------------------------------------------------------------
	bool PushMouseInput(int button)
	{
		if (button & MOUSE_INPUT_LEFT) {
			if (mouse_l_frame == 1) {
				return true;
			}
		}
		if (button & MOUSE_INPUT_RIGHT) {
			if (mouse_r_frame == 1) {
				return true;
			}
		}
		return false;
	}
	//---------------------------------------------------------------------------------
	//	マウスの座標を取得する
	//---------------------------------------------------------------------------------
	int GetMouseX()
	{
		int mouse_x;
		int mouse_y;
		GetMousePoint(&mouse_x, &mouse_y);
		return mouse_x;
	}
	int GetMouseY()
	{
		int mouse_x;
		int mouse_y;
		GetMousePoint(&mouse_x, &mouse_y);
		return mouse_y;
	}
	//---------------------------------------------------------------------------------
	//	マウスの移動量を取得する
	//---------------------------------------------------------------------------------
	int GetMouseMoveX()
	{
		return mouse_x - mouse_x_prev;
	}
	int GetMouseMoveY()
	{
		return mouse_y - mouse_y_prev;
	}
}