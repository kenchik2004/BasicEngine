#include "Main.h"
#include "System/SceneManager.h"
#include "Game/SceneSample.h"

#define DEBUG_WINDOW

class SceneSample;

std::string window_classname[1] =
{
	"デバッグウィンドウ1",
};
int CreateDebugWindow(HINSTANCE& hInstance, HWND& window, int window_x, int window_y, WNDCLASS& window_parameter, int nCmdShow);
//====================================//


// メッセージ処理用関数
constexpr LRESULT CALLBACK WndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_MOVING:
		//ウィンドウ移動中は時飛ばしを行う(Physicsやアップデート処理の暴走を防ぐため)
		Time::ResetTime();
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
#ifdef DEBUG_WINDOW
	MSG msg;
	HWND window[1];
	WNDCLASS param;
#endif
	SRand((unsigned int)time(nullptr));
	SetOutApplicationLogValidFlag(FALSE);
	ChangeWindowMode(TRUE);
	SetGraphMode(800, 450, 32, 240);
	SetMainWindowText("メインウィンドウ");
	SetBackgroundColor(100, 100, 100);
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);

	SetDoubleStartValidFlag(TRUE);
	SetAlwaysRunFlag(TRUE);
	SetWaitVSyncFlag(false);


	if (DxLib_Init() == -1)	return -1;

	SetWindowSizeChangeEnableFlag(true, false);

#ifdef DEBUG_WINDOW


	RECT rect;
	GetWindowRect(GetMainWindowHandle(), &rect);
	if (CreateDebugWindow(hInstance, window[0], rect.right - rect.left, rect.bottom - rect.top, param, nCmdShow) == -1) return -1;
#endif // DEBUG_WINDOW


	timeBeginPeriod(1);
	SetDrawScreen(DX_SCREEN_BACK);
	SetTransColor(255, 0, 255);
	srand(GetNowCount() % RAND_MAX);
	SetWindowPosition(0, 0);

	Time::Init();
	Input::Init();
	PhysicsManager::Init();
	SceneManager::Init();

	//描画のFPSを設定
	Time::SetDrawFPSMAX(60);
	//内部処理のFPSを設定
	Time::SetFPSMAX(166);
	Time::SetFixedFPSMAX(50);
	Time::SetTimeScale(1);


	SetCameraNearFar(0.1f, 3000.0f);
	SetupCamera_Perspective(TO_RADIAN(45.0f));

	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	ChangeLightTypeDir(VGet(0.8f, -1.2f, 1.0f));

	SetCameraPositionAndTarget_UpVecY(float3(0, 0, 0), float3(0, 0, 1));
	SceneManager::Load<SceneSample>();
	Time::ResetTime();


	//===============================================//



	//初期化
	//GameInit();

	while (TRUE)
	{
		try {
#ifdef DEBUG_WINDOW
			//=======================//
			//片方のウィンドウが消されたら、もう片方も終了する
			if (PeekMessage(&msg, window[0], 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
#endif

			//=======================//

			Time::Update();
			Input::Update();

			//アップデート
			//GameUpdate();
			SceneManager::PreUpdate();
			SceneManager::Update();


			SceneManager::LateUpdate();
			SceneManager::PostUpdate();
			bool phys = false;
			while (Time::FixedDeltaTimeD() >= Time::GetFixedDeltaTimeMAXD())
			{
				//物理
				SceneManager::PrePhysics();
				SceneManager::Physics();
				SceneManager::PostPhysics();

				if (Time::FixedDeltaTimeD() >= Time::GetFixedDeltaTimeMAXD() * 2) {
					phys = true;

				}
				Time::FixFixedFPS();
			}
			//あまりよろしくはないが、FPS低下時にはPhysicsが暴走する可能性があるので、
			// fixed_deltatimeが2ループ分以上溜まったら時飛ばしを行う
			if (phys)
				Time::ResetTime();

			//描画
			while (Time::DrawDeltaTimeD() >= Time::GetDrawDeltaTimeMAXD())
			{


				ClearDrawScreen();
				SceneManager::PreDraw();
				SceneManager::Draw();
				SceneManager::LateDraw();


				//GameRender();
				if (!SceneManager::GetCurrentScene())
					SceneManager::Load<SceneSample>();
#ifdef DEBUG_WINDOW
				//書き込みを行うウィンドウを、メインウィンドウに設定
				SetScreenFlipTargetWindow(NULL);
				ScreenFlip();
				//============//
				// メインウィンドウの映り込みがある場合は、直下の行を有効化
				//WaitTimer(2);
				ClearDrawScreen();
				//デバッグウィンドウへの描画
				SceneManager::DebugDraw();
				SceneManager::LateDebugDraw();
				//書き込みを行うウィンドウを、デバッグウィンドウに設定
				SetScreenFlipTargetWindow(window[0]);
#endif
				ScreenFlip();
				Time::FixDrawFPS();
				//============//
			}
			//PostDrawする
			SceneManager::PostDraw();

			Time::FixFPS();
			Time::UpdateFPS();
			if (ProcessMessage())	break;
			if (CheckHitKey(KEY_INPUT_ESCAPE))	break;
		}
		catch (Exception& ex) {
			ex.Show();
			break;
		}

	}
	//終了
	SceneManager::Exit();
	//GameExit();

	//PhysXコピペソース
	//============================================//

	PxCloseExtensions();
	PhysicsManager::Exit();

	//============================================//
	timeEndPeriod(1);
	DxLib::DxLib_End();
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
			DrawTriangle3D(center, pos1, pos2, color, TRUE);
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

int CreateDebugWindow(HINSTANCE& hInstance, HWND& window, int window_x, int window_y, WNDCLASS& window_parameter, int nCmdShow)
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
		WS_MINIMIZEBOX | WS_SYSMENU,
		window_x, window_y, window_x, window_y,
		NULL, NULL, hInstance, NULL
	);
	ShowWindow(window, nCmdShow);
	UpdateWindow(window);

	return 0;
	//==================================//
}
