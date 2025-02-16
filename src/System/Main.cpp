#include "Main.h"
#include "System/SceneManager.h"
#include "Game/SceneSample.h"


class SceneSample;

std::string window_classname[1] =
{
	"デバッグウィンドウ1",
};
int CreateDebugWindow(HINSTANCE& hInstance, HWND& window, WNDCLASS& window_parameter, int nCmdShow);
//====================================//


// メッセージ処理用関数
constexpr LRESULT CALLBACK WndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return(DefWindowProc(window, msg, wParam, lParam));
	}
	return (0L);
}

//=====================================//
//---------------------------------------------------------------------------------
//	WinMain
//---------------------------------------------------------------------------------
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{

	//==================================//
	MSG msg;
	HWND window[1];
	WNDCLASS param;
	if (CreateDebugWindow(hInstance, window[0], param, nCmdShow) == -1) return -1;


	SetOutApplicationLogValidFlag(FALSE);
	ChangeWindowMode(TRUE);
	SetWindowPosition(0, 0);
	SetGraphMode(SCREEN_W, SCREEN_H, 32, 240);
	SetMainWindowText("メインウィンドウ");
	SetBackgroundColor(100, 100, 100);


	SetDoubleStartValidFlag(TRUE);
	SetAlwaysRunFlag(TRUE);
	SetWaitVSyncFlag(false);


	if (DxLib_Init() == -1)	return -1;

	timeBeginPeriod(1);
	SetDrawScreen(DX_SCREEN_BACK);
	SetTransColor(255, 0, 255);
	srand(GetNowCount() % RAND_MAX);

	Time::Init();
	Input::Init();
	SceneManager::Init();


	SetCameraNearFar(0.1f, 3000.0f);
	SetupCamera_Perspective(TO_RADIAN(45.0f));

	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	ChangeLightTypeDir(VGet(0.8f, -1.2f, 1.0f));

	SceneManager::Load<SceneSample>();


	//初期化
	//GameInit();

	while (TRUE)
	{

		//=======================//
		//片方のウィンドウが消されたら、もう片方も終了する
		if (PeekMessage(&msg, window[0], 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//=======================//


		Time::Update();
		Input::Update();
		//アップデート
		SceneManager::PreUpdate();
		SceneManager::Update();
		SceneManager::LateUpdate();
		SceneManager::PostUpdate();
		//GameUpdate();

		//物理
		SceneManager::PrePhysics();
		SceneManager::Physics();
		SceneManager::PostPhysics();

		//描画
		ClearDrawScreen();
		SceneManager::PreDraw();
		SceneManager::Draw();
		SceneManager::LateDraw();

		//GameRender();

		//======================//
		SetScreenFlipTargetWindow(NULL);
		//======================//
		ScreenFlip();
		//============//
		// メインウィンドウの映り込みがある場合は、直下の行を有効化
		//WaitTimer(2);
		ClearDrawScreen();
		SceneManager::DebugDraw();
		SetScreenFlipTargetWindow(window[0]);
		ScreenFlip();
		//============//

		//PostDrawする
		SceneManager::PostDraw();

		Time::FixFPS();
		Time::UpdateFPS();
		if (ProcessMessage())	break;
		if (CheckHitKey(KEY_INPUT_ESCAPE))	break;
	}

	//終了
	SceneManager::Exit();
	//GameExit();

	timeEndPeriod(1);
	DxLib_End();
	return 0;
}

//---------------------------------------------------------------------------------
//	度をラジアンに変換する関数
//---------------------------------------------------------------------------------
float TO_RADIAN(float degree)
{
	return degree * 3.14159265f / 180.0f;
}
//---------------------------------------------------------------------------------
//	ラジアンを度に変換する関数
//---------------------------------------------------------------------------------
float TO_DEGREE(float radian)
{
	return radian * 180.0f / 3.14159265f;
}
//---------------------------------------------------------------------------------
//	ＸＺ方向に円を描画する
//---------------------------------------------------------------------------------
void DrawCircle3D_XZ(float3 center, float radius, int color, bool fill)
{
	VECTOR pos1;
	VECTOR pos2;
	for (int i = 0; i < 36; i++) {
		pos1.x = center.x + radius * sinf(TO_RADIAN(i * 10.0f));
		pos1.y = center.y;
		pos1.z = center.z + radius * cosf(TO_RADIAN(i * 10.0f));

		pos2.x = center.x + radius * sinf(TO_RADIAN((i + 1) * 10.0f));
		pos2.y = center.y;
		pos2.z = center.z + radius * cosf(TO_RADIAN((i + 1) * 10.0f));

		if (fill) {
			DrawTriangle3D(center.VGet(), pos1, pos2, color, TRUE);
		}
		else {
			DrawLine3D(pos1, pos2, color);
		}
	}
}
//---------------------------------------------------------------------------------
//	ＸＺ方向に四角を描画する
//---------------------------------------------------------------------------------
void DrawBox3D_XZ(float3 center, float half_w, float half_h, int color, bool fill)
{
	VECTOR pos1, pos2, pos3, pos4;

	pos1.x = center.x + half_w;
	pos1.y = center.y;
	pos1.z = center.z + half_h;

	pos2.x = center.x + half_w;
	pos2.y = center.y;
	pos2.z = center.z - half_h;

	pos3.x = center.x - half_w;
	pos3.y = center.y;
	pos3.z = center.z - half_h;

	pos4.x = center.x - half_w;
	pos4.y = center.y;
	pos4.z = center.z + half_h;

	if (fill) {
		DrawTriangle3D(pos1, pos2, pos3, color, TRUE);
		DrawTriangle3D(pos1, pos3, pos4, color, TRUE);
	}
	else {
		DrawLine3D(pos1, pos2, color);
		DrawLine3D(pos2, pos3, color);
		DrawLine3D(pos3, pos4, color);
		DrawLine3D(pos4, pos1, color);
	}

}

TypeInfo TypeInfo::Root = TypeInfo("root", 0, nullptr);

const char* TypeInfo::ClassName() const
{
	return class_name.c_str();
}

size_t TypeInfo::ClassSize() const
{
	return class_size;
}

const TypeInfo* TypeInfo::Parent() const
{
	return parent;
}

int CreateDebugWindow(HINSTANCE& hInstance, HWND& window, WNDCLASS& window_parameter, int nCmdShow)
{
	//==================================//


// デバッグウインドウの作成
	window_parameter.style = CS_HREDRAW | CS_VREDRAW;
	window_parameter.lpfnWndProc = WndProc;
	window_parameter.cbClsExtra = 0;
	window_parameter.cbWndExtra = 0;
	window_parameter.hInstance = hInstance;
	window_parameter.hIcon = NULL;
	window_parameter.hCursor = LoadCursor(NULL, IDC_ARROW);
	window_parameter.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	window_parameter.lpszMenuName = NULL;
	window_parameter.lpszClassName = window_classname[0].c_str();

	if (!RegisterClass(&window_parameter))
	{
		return -1;
	}

	window = CreateWindow(
		window_classname[0].c_str(),
		"デバッグウィンドウ",
		WS_OVERLAPPED | WS_SYSMENU,
		800, 450, SCREEN_W*1.27f, SCREEN_H*1.35f,
		NULL, NULL, hInstance, NULL
	);
	ShowWindow(window, nCmdShow);
	UpdateWindow(window);

	return 0;
	//==================================//
}
