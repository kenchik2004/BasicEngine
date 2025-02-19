#include "Main.h"
#include "System/SceneManager.h"
#include "Game/SceneSample.h"


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
#if 1
	MSG msg;
	HWND window[1];
	WNDCLASS param;
#endif

	SetOutApplicationLogValidFlag(FALSE);
	ChangeWindowMode(TRUE);
	SetWindowPosition(0, 100);
	SetGraphMode(800, 450, 32, 240);
	SetMainWindowText("メインウィンドウ");
	SetBackgroundColor(100, 100, 100);
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);

	SetDoubleStartValidFlag(TRUE);
	SetAlwaysRunFlag(TRUE);
	SetWaitVSyncFlag(false);


	if (DxLib_Init() == -1)	return -1;

	RECT rect;
	SetWindowSizeChangeEnableFlag(true, false);
	GetWindowRect(GetMainWindowHandle(), &rect);
	if (CreateDebugWindow(hInstance, window[0], rect.right - rect.left, rect.bottom - rect.top, param, nCmdShow) == -1) return -1;


	timeBeginPeriod(1);
	SetDrawScreen(DX_SCREEN_BACK);
	SetTransColor(255, 0, 255);
	srand(GetNowCount() % RAND_MAX);

	Time::Init();
	Input::Init();
	SceneManager::Init();

	//描画のFPSを設定
	Time::SetDrawFPSMAX(60);
	//内部処理のFPSを設定
	Time::SetFPSMAX(100000);
	Time::SetTimeScale(1);


	SetCameraNearFar(0.1f, 3000.0f);
	SetupCamera_Perspective(TO_RADIAN(45.0f));

	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	ChangeLightTypeDir(VGet(0.8f, -1.2f, 1.0f));

	SetCameraPositionAndTarget_UpVecY(float3(0, 5, -5), float3(0, 0, 0));
	SceneManager::Load<SceneSample>();
	SceneManager::Destroy(SceneManager::GetScene<SceneSample>());



	//PhysX　コピペソース
	//===============================================//
	physx::PxDefaultAllocator m_defaultAllocator;
	// エラー時用のコールバックでエラー内容が入ってる
	physx::PxDefaultErrorCallback m_defaultErrorCallback;
	// 上位レベルのSDK(PxPhysicsなど)をインスタンス化する際に必要
	physx::PxFoundation* m_pFoundation = nullptr;
	// 実際に物理演算を行う
	physx::PxPhysics* m_pPhysics = nullptr;
	// シミュレーションをどう処理するかの設定でマルチスレッドの設定もできる
	physx::PxDefaultCpuDispatcher* m_pDispatcher = nullptr;
	// シミュレーションする空間の単位でActorの追加などもここで行う
	physx::PxScene* m_pScene = nullptr;
	// PVDと通信する際に必要
	physx::PxPvd* m_pPvd = nullptr;
	if (m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_defaultAllocator, m_defaultErrorCallback); !m_pFoundation) {
		return false;
	}
	// PVDと接続する設定
	if (m_pPvd = physx::PxCreatePvd(*m_pFoundation); m_pPvd) {
		// PVD側のデフォルトポートは5425
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		int try_num = 0;
		while (try_num < 10) {
			bool success = m_pPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
			if (success)
				break;
			Sleep(20);
			try_num++;
		}
	}
	// Physicsのインスタンス化
	if (m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, physx::PxTolerancesScale(), true, m_pPvd); !m_pPhysics) {
		return false;
	}
	// 拡張機能用
	if (!PxInitExtensions(*m_pPhysics, m_pPvd)) {
		return false;
	}
	// 処理に使うスレッドを指定する
	m_pDispatcher = physx::PxDefaultCpuDispatcherCreate(8);
	// 空間の設定
	physx::PxSceneDesc scene_desc(m_pPhysics->getTolerancesScale());
	scene_desc.gravity = physx::PxVec3(0, -9, 0);
	scene_desc.filterShader = physx::PxDefaultSimulationFilterShader;
	scene_desc.cpuDispatcher = m_pDispatcher;
	// 空間のインスタンス化
	m_pScene = m_pPhysics->createScene(scene_desc);
	// PVDの表示設定
	physx::PxPvdSceneClient* pvd_client;
	if (pvd_client = m_pScene->getScenePvdClient(); pvd_client) {
		pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	physx::PxRigidDynamic* rigid_dynamic = nullptr;
	for(int i=0;i<1;i++)
	{
			rigid_dynamic= m_pPhysics->createRigidDynamic(physx::PxTransform(physx::PxIdentity));
		// 形状(Box)を作成
		physx::PxShape* box_shape
			= m_pPhysics->createShape(
				// Boxの大きさ
				physx::PxSphereGeometry(2),
				// 摩擦係数と反発係数の設定
				*m_pPhysics->createMaterial(0.5f, 0.5f, 0.99f)
			);
		// 形状のローカル座標を設定
		box_shape->setLocalPose(physx::PxTransform(physx::PxIdentity));
		rigid_dynamic->setGlobalPose(physx::PxTransform(physx::PxVec3(0, 5,0)));
		// 形状を紐づけ
		rigid_dynamic->attachShape(*box_shape);
		// 剛体を空間に追加
		m_pScene->addActor(*rigid_dynamic);
		rigid_dynamic->addForce(physx::PxVec3(0, GetRand(15), 0), physx::PxForceMode::eIMPULSE);


		physx::PxRigidStatic* plane
			= m_pPhysics->createRigidStatic(physx::PxTransform(physx::PxIdentity));


		// 剛体を空間に追加
		m_pScene->addActor(*physx::PxCreatePlane(
			*m_pPhysics, physx::PxPlane(0, 1, 0, 0),
			*m_pPhysics->createMaterial(0.5f, 0.5f, 0.1f))
		);
	}

	//===============================================//


	
	//初期化
	//GameInit();

	while (TRUE)
	{
		try {
#if 1
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
			SceneManager::PreUpdate();
			SceneManager::Update();


			physx::PxRigidStatic* sphere = nullptr;
			{
				if (Input::CheckHitKey(KEY_INPUT_0)) {
					sphere = m_pPhysics->createRigidStatic(physx::PxTransform(physx::PxIdentity));
					// 形状(Box)を作成
					physx::PxShape* sphere_shape
						= m_pPhysics->createShape(
							// Boxの大きさ
							physx::PxSphereGeometry(3),
							// 摩擦係数と反発係数の設定
							*m_pPhysics->createMaterial(0.5f, 0.5f, 0.1f)
						);
					// 形状のローカル座標を設定
					sphere_shape->setLocalPose(physx::PxTransform(physx::PxIdentity));
					sphere->setGlobalPose(physx::PxTransform(Vector3(-1, 1.5, -1)));
					// 形状を紐づけ
					sphere->attachShape(*sphere_shape);
					// 剛体を空間に追加
					m_pScene->addActor(*sphere);
				}

				if (Input::CheckHitKey(KEY_INPUT_1)) {
					if (rigid_dynamic) {
						rigid_dynamic->addForce(-0.5f*rigid_dynamic->getGlobalPose().p,physx::PxForceMode::eFORCE);
					}
				}

			}

			SceneManager::LateUpdate();
			SceneManager::PostUpdate();
			//GameUpdate();

			//物理
			SceneManager::PrePhysics();
			//PhysXコピペソース
			//=========================//
			 // シミュレーション速度を指定する
			m_pScene->simulate(Time::UnscaledDeltaTime());
			// PhysXの処理が終わるまで待つ
			m_pScene->fetchResults(true);

			if (sphere)
				sphere->release();

			//=========================//
			SceneManager::Physics();
			SceneManager::PostPhysics();

			//描画
			if (Time::DrawDeltaTime() >= Time::GetDrawDeltaTimeMAX())
			{


				ClearDrawScreen();
				SceneManager::PreDraw();
				SceneManager::Draw();
				SceneManager::LateDraw();


				//GameRender();
				SceneWP scene = SceneManager::GetScene<SceneSample>();
				if (!SceneManager::GetCurrentScene())
					SceneManager::Load<SceneSample>();
				if (Input::PushHitKey(KEY_INPUT_RETURN))
					SceneManager::Destroy(SceneManager::GetScene<SceneSample>());
#if 1
				//======================//
				SetScreenFlipTargetWindow(NULL);
				//======================//
				ScreenFlip();
				//============//
				// メインウィンドウの映り込みがある場合は、直下の行を有効化
				//WaitTimer(2);
				ClearDrawScreen();
#endif
				SceneManager::DebugDraw();
				if (rigid_dynamic) {
					auto trns = rigid_dynamic->getGlobalPose();
					trns.p.z *= -1;
					Quaternion quat = CastPhysXQuat(trns.q);

					DrawLine3D(float3(trns.p), float3(trns.p + quat.getBasisVector0()*5), GetColor(0, 255, 0));
					DrawLine3D(float3(trns.p), float3(trns.p + quat.getBasisVector1()*5), GetColor(255, 0, 0));
					DrawLine3D(float3(trns.p), float3(trns.p + quat.getBasisVector2()*5), GetColor(0, 0, 255));
					DrawSphere3D(float3(trns.p), 2, 16, GetColor(255, 0, 0), GetColor(255, 255, 255), true);
					
				}

#if 1
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
	if (rigid_dynamic)
		rigid_dynamic->release();
	PxCloseExtensions();
	m_pScene->release();
	m_pDispatcher->release();
	m_pPhysics->release();
	if (m_pPvd) {
		m_pPvd->disconnect();
		physx::PxPvdTransport* transport = m_pPvd->getTransport();
		m_pPvd->release();
		transport->release();
	}
	m_pFoundation->release();

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
		800, 450, window_x, window_y,
		NULL, NULL, hInstance, NULL
	);
	ShowWindow(window, nCmdShow);
	UpdateWindow(window);

	return 0;
	//==================================//
}
