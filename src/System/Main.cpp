#include "Main.h"
#include "System/SceneManager.h"
#include "Game/SceneSample.h"

#define DEBUG_WINDOW

class SceneSample;

std::string window_classname[1] =
{
	"�f�o�b�O�E�B���h�E1",
};
int CreateDebugWindow(HINSTANCE& hInstance, HWND& window, int window_x, int window_y, WNDCLASS& window_parameter, int nCmdShow);
//====================================//


// ���b�Z�[�W�����p�֐�
constexpr LRESULT CALLBACK WndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_MOVING:
		//�E�B���h�E�ړ����͎���΂����s��(Physics��A�b�v�f�[�g�����̖\����h������)
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
	SetMainWindowText("���C���E�B���h�E");
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

	//�`���FPS��ݒ�
	Time::SetDrawFPSMAX(60);
	//����������FPS��ݒ�
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



	//������
	//GameInit();

	while (TRUE)
	{
		try {
#ifdef DEBUG_WINDOW
			//=======================//
			//�Е��̃E�B���h�E�������ꂽ��A�����Е����I������
			if (PeekMessage(&msg, window[0], 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
#endif

			//=======================//

			Time::Update();
			Input::Update();

			//�A�b�v�f�[�g
			//GameUpdate();
			SceneManager::PreUpdate();
			SceneManager::Update();


			SceneManager::LateUpdate();
			SceneManager::PostUpdate();
			bool phys = false;
			while (Time::FixedDeltaTimeD() >= Time::GetFixedDeltaTimeMAXD())
			{
				//����
				SceneManager::PrePhysics();
				SceneManager::Physics();
				SceneManager::PostPhysics();

				if (Time::FixedDeltaTimeD() >= Time::GetFixedDeltaTimeMAXD() * 2) {
					phys = true;

				}
				Time::FixFixedFPS();
			}
			//���܂��낵���͂Ȃ����AFPS�ቺ���ɂ�Physics���\������\��������̂ŁA
			// fixed_deltatime��2���[�v���ȏ㗭�܂����玞��΂����s��
			if (phys)
				Time::ResetTime();

			//�`��
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
				//�������݂��s���E�B���h�E���A���C���E�B���h�E�ɐݒ�
				SetScreenFlipTargetWindow(NULL);
				ScreenFlip();
				//============//
				// ���C���E�B���h�E�̉f�荞�݂�����ꍇ�́A�����̍s��L����
				//WaitTimer(2);
				ClearDrawScreen();
				//�f�o�b�O�E�B���h�E�ւ̕`��
				SceneManager::DebugDraw();
				SceneManager::LateDebugDraw();
				//�������݂��s���E�B���h�E���A�f�o�b�O�E�B���h�E�ɐݒ�
				SetScreenFlipTargetWindow(window[0]);
#endif
				ScreenFlip();
				Time::FixDrawFPS();
				//============//
			}
			//PostDraw����
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
	//�I��
	SceneManager::Exit();
	//GameExit();

	//PhysX�R�s�y�\�[�X
	//============================================//

	PxCloseExtensions();
	PhysicsManager::Exit();

	//============================================//
	timeEndPeriod(1);
	DxLib::DxLib_End();
	return 0;
}

//---------------------------------------------------------------------------------
//	�x�����W�A���ɕϊ�����֐�
//---------------------------------------------------------------------------------
float TO_RADIAN(float degree)
{
	return degree * 3.14159265f / 180.0f;
}
//---------------------------------------------------------------------------------
//	���W�A����x�ɕϊ�����֐�
//---------------------------------------------------------------------------------
float TO_DEGREE(float radian)
{
	return radian * 180.0f / 3.14159265f;
}
//---------------------------------------------------------------------------------
//	�w�y�����ɉ~��`�悷��
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
//	�w�y�����Ɏl�p��`�悷��
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


// �f�o�b�O�E�C���h�E�̍쐬
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
		"�f�o�b�O�E�B���h�E",
		WS_MINIMIZEBOX | WS_SYSMENU,
		window_x, window_y, window_x, window_y,
		NULL, NULL, hInstance, NULL
	);
	ShowWindow(window, nCmdShow);
	UpdateWindow(window);

	return 0;
	//==================================//
}
