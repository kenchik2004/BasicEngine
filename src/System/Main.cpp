//---------------------------------------------------------------------------
//! @file   Main.cpp
//! @brief  Mainクラスの実装
//---------------------------------------------------------------------------
#include "Main.h"
#include <fstream>

//! @brief ウィンドウの幅(初期値:1920)
int SCREEN_W = 1920;
//! @brief ウィンドウの高さ(初期値:1080)
int SCREEN_H = 1080;

#define SHOW_FPS
#ifdef SHOW_FPS
std::array<float, 60> fps_array = {};
size_t fps_index = 0;
#endif


//! @brief Windowsのウィンドウプロシージャ
//! @param windowのハンドル
//! @param msg Windowsのメッセージコード
//! @param wParam Windowsのメッセージコードに付随するパラメータ1
//! @param lParam Windowsのメッセージコードに付随するパラメータ2
//! @return LRESULT Windowsのウィンドウプロシージャの処理結果
constexpr LRESULT CALLBACK WndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_MOVING:
	case WM_SIZE:
		//ウィンドウの移動やサイズ変更の際に、時間をリセットすることで、物理演算などの挙動が不安定になるのを防ぐ
		Time::ResetTime();
		break;
	default:
		return(DefWindowProc(window, msg, wParam, lParam));
	}
	return (0L);
}

//! @brief Windowsのウィンドウプロシージャ(DxLib用)
//! @param windowのハンドル
//! @param msg Windowsのメッセージコード
//! @param wParam Windowsのメッセージコードに付随するパラメータ1
//! @param lParam Windowsのメッセージコードに付随するパラメータ2
//! @return LRESULT Windowsのウィンドウプロシージャの処理結果
constexpr LRESULT CALLBACK DxWndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
	case WM_MOVING:
	case WM_SIZE:
		//ウィンドウの移動やサイズ変更の際に、時間をリセットすることで、物理演算などの挙動が不安定になるのを防ぐ
		Time::ResetTime();
		break;
	}
	return (0L);
}
//=====================================//
//---------------------------------------------------------------------------------
//! @brief Windowsアプリケーションのエントリーポイント
//! @param hInstance Windowsアプリケーションのインスタンスハンドル
//! @param hPrevInstance Windowsアプリケーションの前のインスタンスハンドル(常にNULL)
//! @param lpCmdLine Windowsアプリケーションのコマンドライン引数(文字列)
//! @param nCmdShow Windowsアプリケーションの表示方法を指定するフラグ
//! @return int Windowsアプリケーションの終了コード
//---------------------------------------------------------------------------------
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{

	//もうSJISは使わん!!UTF-8しか勝たん!!
	//MSもいい加減UTF標準にしやがれ!!
	SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);

	//==================================//

	SetOutApplicationLogValidFlag(true);

	bool not_full_screen = FileSystem::IniFileManager::GetBool("StartConfig", "full_screen", false, "data/config.ini");
	ChangeWindowMode(!not_full_screen);
	SCREEN_W = FileSystem::IniFileManager::GetInt("StartConfig", "screen_width", 1920, "data/config.ini");
	SCREEN_H = FileSystem::IniFileManager::GetInt("StartConfig", "screen_height", 1080, "data/config.ini");

	SetGraphMode(SCREEN_W, SCREEN_H, 32, 240);
	SetZBufferBitDepth(32);
	//iniファイルからウィンドウのタイトルを取得して設定する
	std::string window_text = FileSystem::IniFileManager::GetString("StartConfig", "window_name", "BasicEngine", "data/config.ini");
	SetMainWindowText(window_text.c_str());
	SetBackgroundColor(100, 100, 100);
	SetHookWinProc(DxWndProc);
	SetDoubleStartValidFlag(true);
	SetAlwaysRunFlag(TRUE);
	SetWaitVSyncFlag(false);
	Set3DSoundOneMetre(1.0f);
	SetEnableXAudioFlag(true);
	SetUseDirect3DVersion(DX_DIRECT3D_11);

	if (DxLib_Init() == -1)	return -1;

	SetWindowSizeChangeEnableFlag(true, false);

	//マウスポインタの表示設定をiniファイルから取得して設定する
	bool show_mouse = FileSystem::IniFileManager::GetBool("StartConfig", "show_mouse", true, "data/config.ini");

	SetMouseDispFlag(show_mouse);



	timeBeginPeriod(1);
	SetDrawScreen(DX_SCREEN_BACK);
	SetTransColor(255, 0, 255);
	srand(GetNowCount() % RAND_MAX);

	Time::Init();
	Input::Init();
	Random::Init();
	PhysicsManager::Init();
	SceneManager::Init();
	RenderInit();
#if 0
	ImGuiInit(false);
#endif
	//DrawFPSの上限をiniファイルから取得して設定する
	int d_fps = FileSystem::IniFileManager::GetInt("StartConfig", "draw_fps", 60, "data/config.ini");
	Time::SetDrawFPSMAX(d_fps);

	//内部更新頻度の上限をiniファイルから取得して設定する
	int fps = FileSystem::IniFileManager::GetInt("StartConfig", "update_fps", 60, "data/config.ini");
	Time::SetFPSMAX(fps);

	//物理演算の更新頻度の上限をiniファイルから取得して設定する
	int fix_fps = FileSystem::IniFileManager::GetInt("StartConfig", "fixed_fps", 50, "data/config.ini");
	Time::SetFixedFPSMAX(fix_fps);

	//時間のスケールをiniファイルから取得して設定する
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


	//iniファイルから開始シーンのクラス名を取得して、シーンマネージャーにロードする
	auto start_scene_name = FileSystem::IniFileManager::GetString("StartConfig", "start_scene", "SceneSample", "data/config.ini");

	auto start_scene = CreateInstanceFromName<Scene>(start_scene_name);
	SceneManager::Load<Scene>(start_scene);

	while (TRUE)
	{

		try {


			//=======================//
			Time::Update();
			Input::Update();
#if 0
			ImGuiUpdate();
#endif
			//シーンの更新
			//GameUpdate();
			SceneManager::PreUpdate();
			SceneManager::Update();


			SceneManager::LateUpdate();
			SceneManager::PostUpdate();
			bool phys = false;
			double real_delta = Time::RealDeltaTimeD();
			double fixed_max = Time::GetFixedDeltaTimeMAXD();
			double max = max(fixed_max, real_delta);
			//PCの性能によっては、物理演算の更新頻度の上限を超えるほどフレームレートが高くなってしまうことがあるため、
			//その場合は物理演算の更新頻度の上限に合わせて、物理演算の更新を複数回行うようにする
			SceneManager::PrePhysics();
			u64 loops = static_cast<u64>(Time::FixedDeltaTimeD() / max);
			for (int i = 0; i < loops; i++)
			{
				//物理演算の更新
				SceneManager::Physics();

			}
			Time::FixFixedFPS(loops);
			SceneManager::PostPhysics();

			//描画FPSを越えてしまう場合は、描画をスキップする
			bool draw_current_frame = !(Time::DrawDeltaTimeD() >= Time::GetDrawDeltaTimeMAXD());
			//描画
			if (!draw_current_frame)
			{


				ClearDrawScreen();
				SceneManager::Draw();
				//GameRender();
#ifdef SHOW_FPS
				float fps = Time::GetDrawFPS();
				fps_array[fps_index] = fps;
				fps_index = (fps_index + 1) % fps_array.size();
				float average_fps = 0.0f;
				for (u32 i = 0; i < fps_array.size(); i++)
					average_fps += fps_array[i];
				average_fps /= fps_array.size();
				printfDx("%.2f fps_avg\n", average_fps);
				printfDx("%.2f fps\n", fps);

#endif
				ScreenFlip();
				//描画FPSを計測し、描画delta_timeをリセットする
				Time::FixDrawFPS();
				//============//
			}

			//PostDraw
			SceneManager::PostDraw();

			Time::FixFPS();
			Time::UpdateFPS();
			if (ProcessMessage())	break;
			if (Input::GetKey(KeyCode::Escape))	break;
			if (SceneManager::IsApplicationClosing())	break;
		}
		//本当は非常によくない。
		//ここでは最低限の良心として、Log出力を行うことにとどめる。
		//また、std::exceptionをキャッチしないようにし、自作フレームワークのみを対象とする。
		//(std::exceptionは予想外の致命的な例外である可能性が高いため、キャッチしてしまうと、バグの原因を見つけるのが非常に困難になる)
		catch (Exception& ex) {
			ex.Show();

		}

	}
#ifdef PACKAGE_BUILD
	//パッケージビルドの場合は、終了コードを0にしてすぐに終了する
	std::quick_exit(0);
#endif
	//終了処理
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

	//Effekseerを導入したらDxLib_Endでアクセス違反が出るようになった
	//現場じゃ絶対やっちゃいけないが、手元にソースもないので手の施しようがないため
	//最悪の手段としてDxLib_End()を呼ばないことにする
	//リソースの解放はOSに任せるしかない
	//DxLib::DxLib_End();

	//継承ツリーの構造をテキストファイルに出力する
	std::ofstream f("data/tree.txt");
	if (!f.fail()) {


		auto& base_type = TypeInfo::Root();
		const TypeInfo* p = base_type.Child();
		bool            returnFromTraverse = false;
		const TypeInfo* next = nullptr;
		f << base_type.ClassName() << std::endl;
		//----------------------------------------------------------
		// 継承ツリーの構造をテキストファイルに出力するための高速なツリー探索アルゴリズム
		// 高速なツリー探索 (stackless tree traversal)
		//----------------------------------------------------------
		int nest = 0;
		while (p && (p != &base_type)) {
			if (!returnFromTraverse) {
				for (int i = 0; i < nest; i++)
					f << "|        " << std::flush;
				f << "|-----" << p->ClassName() << std::endl;
			}
			if (p->Child() && !returnFromTraverse) {
				// 子ノードがある場合は子ノードに移動する
				nest++;
				next = p->Child();
				returnFromTraverse = false;
			}
			else if (p->Sibling()) {
				// 子ノードがない場合は兄弟ノードに移動する
				next = p->Sibling();
				returnFromTraverse = false;
			}
			else {
				// 子ノードも兄弟ノードもない場合は親ノードに移動する
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
	return 0;
}


//---------------------------------------------------------------------------------
//! @brief degree角をradian角に変換する
//! @param degree degree角(0~360で表される)
//! @return float radian角(0~2πで表される)
//---------------------------------------------------------------------------------
float TO_RADIAN(float degree)
{
	return degree * 3.14159265f / 180.0f;
}
//---------------------------------------------------------------------------------
//! @brief radian角をdegree角に変換する
//! @param radian radian角(0~2πで表される)
//! @return float degree角(0~360で表される)
//---------------------------------------------------------------------------------
float TO_DEGREE(float radian)
{
	return radian * 180.0f / 3.14159265f;
}
//---------------------------------------------------------------------------------
//! @brief XZ平面における円を描画する
//! @param center 円の中心座標
//! @param radius 円の半径
//! @param color 円の色
//! @param fill 円を塗りつぶすかどうか(trueで塗りつぶす、falseで線のみ)
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
//! @brief XZ平面における矩形を描画する
//! @param center 矩形の中心座標
//! @param half_w 横幅の半分
//! @param half_h 高さの半分
//! @param color 矩形の色
//! @param fill 矩形を塗りつぶすかどうか(trueで塗りつぶす、falseで線のみ)
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
