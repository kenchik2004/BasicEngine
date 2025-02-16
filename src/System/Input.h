#pragma once
namespace Input
{
	void Init();
	void Update();
	bool PushHitKey(int key);
	bool CheckHitKey(int key);
	bool CheckMouseInput(int button = MOUSE_INPUT_LEFT | MOUSE_INPUT_RIGHT);
	bool PushMouseInput(int button = MOUSE_INPUT_LEFT | MOUSE_INPUT_RIGHT);
	int GetMouseX();
	int GetMouseY();
	int GetMouseMoveX();
	int GetMouseMoveY();
};

