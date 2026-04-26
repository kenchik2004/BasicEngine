#include "Main.h"
#include <fstream>
//#define DEBUG_WINDOW
//#define USE_DEBUG_DRAW
//#define FULL_SCREEN

//#define SECONDARY
//! @brief ��ʂ̉���(�����l:1920)
int SCREEN_W = 1920;
//! @brief ��ʂ̏c��(�����l:1080)
int SCREEN_H = 1080;

//! @brief �E�B���h�E�̃N���X���ꗗ
std::string window_classname[1] =
{
	"�f�o�b�O�E�B���h�E1",
};
int CreateDebugWindow(HINSTANCE& hInstance, HWND& window, int window_x, int window_y, WNDCLASS& window_parameter, int nCmdShow);
//====================================//

//! @brief �W���I��Windows���b�Z�[�W�����֐�
//! @param window �E�B���h�E�n���h��
//! @param msg �E�B���h�E���b�Z�[�W
//! @param wParam �p�����[�^1
//! @param lParam �p�����[�^2
//! @return LRESULT �E�B���h�E���b�Z�[�W��������
constexpr LRESULT CALLBACK WndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_MOVING:
	case WM_SIZE:
		//�E�B���h�E�ړ����͎���΂����s��(Physics��A�b�v�f�[�g�����̖\����h������)
		Time::ResetTime();
		break;
	default:
		return(DefWindowProc(window, msg, wParam, lParam));
	}
	return (0L);
}

//! @brief DxLib���̃��b�Z�[�W�t�b�N�p�̃E�B���h�E���b�Z�[�W�����֐�
//! @param window �E�B���h�E�n���h��
//! @param msg �E�B���h�E���b�Z�[�W
//! @param wParam �p�����[�^1
//! @param lParam �p�����[�^2
//! @return LRESULT �E�B���h�E���b�Z�[�W��������
constexpr LRESULT CALLBACK DxWndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
	case WM_MOVING:
	case WM_SIZE:
		//�E�B���h�E�ړ����͎���΂����s��(Physics��A�b�v�f�[�g�����̖\����h������)
		Time::ResetTime();
		break;
	}
	return (0L);
}
//=====================================//
//---------------------------------------------------------------------------------
//! @brief Windows�A�v���P�[�V�����̃G���g���|�C���g (WinMain)
//! @param hInstance ���݂̃C���X�^���X�n���h��
//! @param hPrevInstance �ȑO�̃C���X�^���X�n���h�� (���NULL)
//! @param lpCmdLine �R�}���h���C������
//! @param nCmdShow �E�B���h�E�̕\���T�C�Y����(�ő�E�ŏ�����)�������l
//! @return �v���O�����̏I���R�[�h
//---------------------------------------------------------------------------------
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{

	//����UTF-8�����g���!SHIFT_JIS�̓N�\!!
	SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);

	//==================================//
#ifdef DEBUG_WINDOW
	MSG msg;
	HWND window[1];
	WNDCLASS param;
#endif
	SetOutApplicationLogValidFlag(FALSE);

	bool not_full_screen = FileSystem::IniFileManager::GetBool("StartConfig", "full_screen", false, "data/config.ini");
	ChangeWindowMode(!not_full_screen);
	SCREEN_W = FileSystem::IniFileManager::GetInt("StartConfig", "screen_width", 1920, "data/config.ini");
	SCREEN_H = FileSystem::IniFileManager::GetInt("StartConfig", "screen_height", 1080, "data/config.ini");

#ifdef FULL_SCREEN
	//ChangeWindowMode(false);
#endif
	SetGraphMode(SCREEN_W, SCREEN_H, 32, 240);
	SetZBufferBitDepth(32);
	std::string window_text = FileSystem::IniFileManager::GetString("StartConfig", "window_name", "���C���E�B���h�E", "data/config.ini");
	SetMainWindowText(window_text.c_str());
	SetBackgroundColor(100, 100, 100);
	//SetWindowStyleMode(4);
	//SetWindowSizeChangeEnableFlag(true, true);
	SetHookWinProc(DxWndProc);
	SetDoubleStartValidFlag(true);
	SetAlwaysRunFlag(TRUE);
	SetWaitVSyncFlag(false);
	Set3DSoundOneMetre(1.0f);
	SetEnableXAudioFlag(true);
	SetUseDirect3DVersion(DX_DIRECT3D_11);

	if (DxLib_Init() == -1)	return -1;

	SetWindowSizeChangeEnableFlag(true, false);


	bool show_mouse = FileSystem::IniFileManager::GetBool("StartConfig", "show_mouse", true, "data/config.ini");

	SetMouseDispFlag(show_mouse);

#ifdef DEBUG_WINDOW


	RECT rect;
	GetWindowCRect(&rect);
	if (CreateDebugWindow(hInstance, window[0], rect.right - rect.left, rect.bottom - rect.top, param, nCmdShow) == -1) return -1;
#endif // DEBUG_WINDOW


	timeBeginPeriod(1);
	SetDrawScreen(DX_SCREEN_BACK);
	SetTransColor(255, 0, 255);
	srand(GetNowCount() % RAND_MAX);
	//SetWindowPosition(0, 0);

	Time::Init();
	Input::Init();
	Random::Init();
	PhysicsManager::Init();
	SceneManager::Init();
	RenderInit();
#if 0
	ImGuiInit(false);
#endif
	//�`���FPS��ݒ�
	int d_fps = FileSystem::IniFileManager::GetInt("StartConfig", "draw_fps", 60, "data/config.ini");
	Time::SetDrawFPSMAX(d_fps);

	//����������FPS��ݒ�
	int fps = FileSystem::IniFileManager::GetInt("StartConfig", "update_fps", 60, "data/config.ini");
	Time::SetFPSMAX(fps);

	int fix_fps = FileSystem::IniFileManager::GetInt("StartConfig", "fixed_fps", 50, "data/config.ini");
	Time::SetFixedFPSMAX(fix_fps);

	int time_scale = FileSystem::IniFileManager::GetInt("StartConfig", "time_scale", 1, "data/config.ini");
	Time::SetTimeScale(time_scale);


	SetCameraNearFar(0.1f, 3000.0f);
	SetupCamera_Perspective(TO_RADIAN(45.0f));

	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	SetLightEnable(FALSE);

	SetCameraPositionAndTarget_UpVecY(float3(0, 0, 0), float3(0, 0, 1));

	SetUseSetDrawScreenSettingReset(false);

	//===============================================//


	auto start_scene_name = FileSystem::IniFileManager::GetString("StartConfig", "start_scene", "SceneSample", "data/config.ini");

	auto start_scene = CreateInstanceFromName<Scene>(start_scene_name);
	SceneManager::Load<Scene>(start_scene);

	while (TRUE)
	{

	MAIN_LOOP:
		try {
#ifdef DEBUG_WINDOW
			//=======================//
			//�Е��̃E�B���h�E�������ꂽ��A�����Е����I������
			if (PeekMessage(&msg, window[0], 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if (PeekMessage(&msg, GetMainWindowHandle(), 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
#endif

			//=======================//
			Time::Update();
			Input::Update();
#if 0
			ImGuiUpdate();
#endif
			//�A�b�v�f�[�g
			//GameUpdate();
			SceneManager::PreUpdate();
			SceneManager::Update();


			SceneManager::LateUpdate();
			SceneManager::PostUpdate();
			bool phys = false;
			double real_delta = Time::RealDeltaTimeD();
			double fixed_max = Time::GetFixedDeltaTimeMAXD();
			double max = max(fixed_max, real_delta);
			//PC�̃X�y�b�N����ł͎��ۂɏo�Ă���FPS���������X�V���s�����Ƃ���̂ŁA
			//FPS�������X�V�p�x����������ꍇ��FPS����Ɍv�Z�p�x�����߂�
			SceneManager::PrePhysics();
			u64 loops = static_cast<u64>(Time::FixedDeltaTimeD() / max);
			for (int i = 0; i < loops; i++)
			{
				//����
				SceneManager::Physics();

			}
			Time::FixFixedFPS(loops);
			SceneManager::PostPhysics();

			bool imgui_drawed = !(Time::DrawDeltaTimeD() >= Time::GetDrawDeltaTimeMAXD());
			//�`��
			if (!imgui_drawed)
			{


				ClearDrawScreen();
				SceneManager::Draw();
				//GameRender();
#ifdef DEBUG_WINDOW
				//�������݂��s���E�B���h�E���A���C���E�B���h�E�ɐݒ�
				SetScreenFlipTargetWindow(NULL);
				ScreenFlip();
				//============//
				// ���C���E�B���h�E�̉f�荞�݂�����ꍇ�́A�����̍s��L����
				//WaitTimer(2);
				ClearDrawScreen();
				//�f�o�b�O�E�B���h�E�ւ̕`��
#endif
#ifdef USE_DEBUG_DRAW
				SceneManager::DebugDraw();
				SceneManager::LateDebugDraw();
#endif
#ifdef DEBUG_WINDOW
				//�������݂��s���E�B���h�E���A�f�o�b�O�E�B���h�E�ɐݒ�
				SetScreenFlipTargetWindow(window[0]);
#endif
#if 0
				ID3D11Texture2D* backBufferTex = reinterpret_cast<ID3D11Texture2D*>(const_cast<void*>(GetUseDirect3D11BackBufferTexture2D()));
				ID3D11ShaderResourceView* g_BackBufferSRV = nullptr;
				ID3D11Device* device = reinterpret_cast<ID3D11Device*>(const_cast<void*>(GetUseDirect3D11Device())); // DxLib����擾

				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MipLevels = 1;
				HRESULT hr = device->CreateShaderResourceView(backBufferTex, &srvDesc, &g_BackBufferSRV);
				if (FAILED(hr)) {
					// �G���[����
					PostQuitMessage(0);
				}
				else {
					//�������傢���̂��܂��BImGui�̒��ŕ`�悷�郓�S
					ImGui::Begin("Game View", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
					auto size = ImGui::GetWindowSize();
					auto real_size = ImVec2(SCREEN_W, SCREEN_H);
					ImVec2 scale = size / real_size;
					scale = scale.x < scale.y ? ImVec2(scale.x, scale.x) : ImVec2(scale.y, scale.y);
					ImGui::Image((ImTextureID)g_BackBufferSRV, real_size * scale); // �𑜓x�͉�ʃT�C�Y�ɉ�����
					ImGui::End();
				}
#endif
				//�{����ImGuiDraw�̈ʒu�͂���
				//�Ȃ񂩍Ō�ɕ`�悵�����̂���ImGui�ɕ`��̈悪�z����̂łƂ肠������ʊO�ɉ�����������
#if 0
				ImGuiDraw();
#endif
				//ImGui�̒��Ńh���[����Ȃ�A�������͌Ă΂Ȃ��Ă���
				ScreenFlip();
				Time::FixDrawFPS();
				//============//
			}
#if 0
			else     //ImGui�����͖��t���[�������Ȃ���{����̂ŁA�����I�Ƀh���[
				ImGuiDraw();
#endif
			//PostDraw����
			SceneManager::PostDraw();

			Time::FixFPS();
			Time::UpdateFPS();
			if (ProcessMessage())	break;
			if (Input::GetKey(KeyCode::Escape))	break;
			if (SceneManager::IsApplicationClosing())	break;
		}
		catch (Exception& ex) {
			ex.Show();
			//ImGuiDraw();
			goto MAIN_LOOP;

		}

	}
#ifdef PACKAGE_BUILD
	std::quick_exit(0);
	//ImGuiExit();
#endif
	//�I��
	try {
		SceneManager::Exit();
	}
	catch (Exception& ex) {
		ex.Show();
	}

	try {
		PhysicsManager::Exit();
	}
	catch (Exception& ex) {
		ex.Show();
	}
	try {
		RenderExit();
	}
	catch (Exception& ex) {
		ex.Show();
	}

	timeEndPeriod(1);
	DxLib::DxLib_End();
	std::ofstream f("data/tree.txt");
	if (!f.fail()) {


		auto& base_type = TypeInfo::Root();
		const TypeInfo* p = base_type.Child();
		bool            returnFromTraverse = false;
		const TypeInfo* next = nullptr;
		f << base_type.ClassName() << std::endl;
		//----------------------------------------------------------
		// �p���c���[�\����T��
		// �X�^�b�N�ċA���g��Ȃ������ȃc���[�T�� (stackless tree traversal)
		//----------------------------------------------------------
		int nest = 0;
		while (p && (p != &base_type)) {
			if (!returnFromTraverse) {
				for (int i = 0; i < nest; i++)
					f << "|        " << std::flush;
				f << "|-----" << p->ClassName() << std::endl;
			}
			if (p->Child() && !returnFromTraverse) {
				// �q������ꍇ�͎q���ɒ��ׂ�B(�q����T���Ŗ߂��Ă����ꍇ�͏��O)
				nest++;
				next = p->Child();
				returnFromTraverse = false;
			}
			else if (p->Sibling()) {
				// �Z�킪����ꍇ�͌Z��𒲂ׂ�
				next = p->Sibling();
				returnFromTraverse = false;
			}
			else {
				// �e�֖߂�B
				next = p->Parent();
				for (int i = 0; i < nest; i++)
					f << "|        " << std::flush;
				f << "end_of_node" << std::endl;
				for (int i = 0; i < nest; i++)
					f << "|        " << std::flush;
				f << std::endl;
				nest--;

				returnFromTraverse = true;
			}

			p = next;
		}
		f.close();
	}
	//system("pause");
	return 0;
}


//---------------------------------------------------------------------------------
//! @brief �x�����W�A���ɕϊ�����֐�
//! @param degree �p�x(�x���@)
//! @return float �p�x(���W�A��)
//---------------------------------------------------------------------------------
float TO_RADIAN(float degree)
{
	return degree * 3.14159265f / 180.0f;
}
//---------------------------------------------------------------------------------
//! @brief ���W�A����x�ɕϊ�����֐�
//! @param radian �p�x(���W�A��)
//! @return float �p�x(�x���@)
//---------------------------------------------------------------------------------
float TO_DEGREE(float radian)
{
	return radian * 180.0f / 3.14159265f;
}
//---------------------------------------------------------------------------------
//! @brief �w�y���ʏ�ɉ~��3D�`�悷��
//! @param center �~�̒��S���W
//! @param radius �~�̔��a
//! @param color �`��F
//! @param fill �h��Ԃ��t���O(true�œh��Ԃ�)
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
//! @brief �w�y���ʏ�Ɏl�p�`��3D�`�悷��
//! @param center �l�p�`�̒��S���W
//! @param half_w �����̔���
//! @param half_h �c���̔���
//! @param color �`��F
//! @param fill �h��Ԃ��t���O(true�œh��Ԃ�)
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



// デバッグ用サブウィンドウを作成する
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
		window_x * 0.5f, window_y * 0.5f, window_x, window_y,
		NULL, NULL, hInstance, NULL
	);
	ShowWindow(window, nCmdShow);
	UpdateWindow(window);

	return 0;
	//==================================//
}
