// PlayerCameraMachine.cpp
//---------------------------------------------------------------------------
//! @file   PlayerCameraMachine.cpp
//! @brief  PlayerCameraMachineコンポーネントの実装。プレイヤー追従カメラの動作を管理する
//---------------------------------------------------------------------------
#include "precompile.h"
#include "PlayerCameraMachine.h"
#include "Game/Objects/NeonFade/Player.h"

namespace NeonFade
{
	void PlayerCameraMachine::LoadCameraSettings()
	{

		// 設定ファイルからカメラのX軸およびY軸の反転設定を読み込む
		reverse_x = FileSystem::IniFileManager::GetBool("Camera", "InvertX", false, "data/config.ini");
		reverse_y = FileSystem::IniFileManager::GetBool("Camera", "InvertY", false, "data/config.ini");
	}

	int PlayerCameraMachine::Init()
	{
		// 初期状態を操作モードに設定する
		mode = MANIPULATE;

		LoadCameraSettings();  // カメラ設定をロードする
		ResetCameraRot();      // カメラの回転角度を初期化する

		shake_duration = 0.0f;  // シェイクの持続時間を初期化する
		transition_timer = 0.0f; // トランジションタイマーを初期化する

		// 親クラスの初期化処理を呼び出す
		return Super::Init();
	}

	void PlayerCameraMachine::PreDraw()
	{
		// カメラシェイクの残り時間を実際の経過時間分だけ減らす
		shake_duration -= Time::UnscaledDeltaTime();
		// トランジションの残り時間がある場合は実際の経過時間分だけ減らす
		if (transition_timer > 0.0f)
			transition_timer -= Time::UnscaledDeltaTime();

		// シェイク時間が残っている場合はランダムなオフセットを計算して揺れを表現する
		if (shake_duration > 0.0f) {
			shake_offset = Random::Position({ -1,-1,-1 }, { 1,1,1 }).getNormalized().multiply(shake_intensity);
		}
		else {
			// シェイクが終わっている場合はオフセットをゼロに戻す
			shake_offset = { 0,0,0 };
		}

		{
			// 操作モードの場合はプレイヤーの入力に基づくカメラの回転処理を行う


			// 追従対象のプレイヤーと自身のトランスフォームを取得する
			SafeSharedPtr<Transform> pl_transform = target->transform;
			Vector3 cam_vector_z = CreateCameraVector(pl_transform->position, cinematic_offset);  // カメラの向きを計算する
			AdjustCameraDistance(pl_transform.get(), cam_vector_z);  // カメラの距離を調整して最終位置を決定する


		}
	}

	void PlayerCameraMachine::Update()
	{
		// プレイヤーの入力に基づくカメラの回転処理を行う
		if (mode == MANIPULATE) {
			//入力を適用する
			ApplyInput(Time::DeltaTime());
		}
	}


	void PlayerCameraMachine::ShakeCamera(const Vector3& intensity, float duration)
	{
		// 外部から渡された揺れの強さと持続時間をメンバ変数に保存する
		shake_intensity = intensity;
		shake_duration = duration;
	}

	void PlayerCameraMachine::SetCameraMode(CAMERA_MODE mode_, bool transition_enable)
	{
		mode = mode_;
		if (transition_enable)transition_timer = transition_time_max;
	}

	void PlayerCameraMachine::ApplyInput(float dt)
	{
		// パッドの右スティックの入力値を取得する
		Vector2 pad_input = Input::GetPadRightStick(0);

		// 入力値に反転設定を適用する
		pad_input.x *= reverse_x ? -1 : 1;
		pad_input.y *= reverse_y ? -1 : 1;

		// スティックが少しでも入力されていれば回転処理を行う
		if (pad_input.magnitudeSquared() >= 1e-6f * 1e-6f) {
			// 水平方向の回転角を計算して加算する
			camera_rot.y += pad_input.x * dt * camera_rot_speed;
			// 垂直方向の回転角を計算して加減算する
			camera_rot.x += pad_input.y * dt * camera_rot_speed;

			// 垂直方向の回転角を一定の範囲内に制限して見上げ・見下ろし過ぎを防ぐ
		}
		else {
			// パッド入力がない場合はキーボードの矢印キーで回転角を制御する
			if (Input::GetKey(KeyCode::Left))
				camera_rot.y -= dt * camera_rot_speed;
			if (Input::GetKey(KeyCode::Right))
				camera_rot.y += dt * camera_rot_speed;
			if (Input::GetKey(KeyCode::Up))
				camera_rot.x += dt * camera_rot_speed;
			if (Input::GetKey(KeyCode::Down))
				camera_rot.x -= dt * camera_rot_speed;
		}
		// 垂直方向の回転角を制限する
		camera_rot.x = physx::PxClamp(camera_rot.x, camera_rot_limit_min, camera_rot_limit_max);
	}

	Vector3 PlayerCameraMachine::CreateCameraVector(const Vector3& target_pos, const Vector3& offset)
	{
		// カメラの基準となる前方ベクトルを定義する
		Vector3 cam_vector_z = { 0,0,1 };

		// シネマティックモード用のオフセットベクトルを正規化して向きを取得する
		Vector3 cinematic_z = cinematic_offset.getNormalized();

		// プレイヤーの操作による回転角からクォータニオンを生成し基準ベクトルを回転させる
		Quaternion cam_rot_around = Quaternion(DEG2RAD(camera_rot.y), { 0,1,0 }) * Quaternion(DEG2RAD(camera_rot.x), { 1,0,0 });
		Vector3 manipulate_z = cam_rot_around.rotate(cam_vector_z);

		// 現在のモードがシネマティックかどうかを判定する
		bool is_cinem = (mode == CINEMATIC);
		// トランジション期間中の場合はカメラの向きを補間して滑らかに移行させる
		if (transition_timer > 0.0f) {
			// トランジションの進行割合を計算する
			float t = transition_timer / transition_time_max;
			// シネマティックモードへの移行の場合は補間割合を逆転させる
			if (is_cinem)
				t = 1.0f - t;
			// 現在のモードに応じたベクトル間で球面線形補間を行う
			cam_vector_z = Slerp(manipulate_z, cinematic_z, t);
		}
		else {
			// トランジション期間外であれば現在のモードに紐づくベクトルを直接使用する
			cam_vector_z = is_cinem ? cinematic_z : manipulate_z;
		}
		return cam_vector_z;  // 計算されたベクトルに距離を掛けて最終的なカメラオフセットを返す
	}

	void PlayerCameraMachine::AdjustCameraDistance(Transform* target_transform, Vector3& cam_vector_z)
	{

		// 障害物判定のためのレイキャスト情報を準備する
		physx::PxRaycastHit hits[2];
		RayCastInfo info(hits, 2);
		// 一旦カメラの距離を最大値に設定する
		camera_distance = camera_distance_max;
		// 地形レイヤーのみを対象としたフィルタデータを構築する
		physx::PxQueryFilterData filter_data;
		//地形コリジョンにのみ当たる
		filter_data.data.word0 = Collider::Layer::Terrain;
		// 静的および動的オブジェクトの両方を対象とする
		filter_data.flags = physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::ePREFILTER;

		// 追従対象の位置に高さを加えた位置をカメラの注視点とする

		Vector3 look_at = target_transform->position + Vector3(0, 2, 0);

		// 追従対象の少し上の位置からカメラ方向へレイを飛ばして障害物を検知する
		owner->GetScene()->RayCast(Ray{ look_at + shake_offset, -cam_vector_z, camera_distance }, info, filter_data);
		// 障害物にヒットした場合はカメラの距離を障害物の手前までに短縮する
		if (info.hasBlock) {
			camera_distance = info.block.distance - 0.5f;
		}
		// 方向ベクトルに計算された距離を掛けて実際のオフセットベクトルを算出する
		cam_vector_z = cam_vector_z * camera_distance;
		// 追従対象の位置に高さとシェイクを加味しそこからオフセットを引いて最終位置を決定する
		Vector3 finaly_position = look_at - cam_vector_z;
		finaly_position += shake_offset;

		// 計算された最終位置と向きをカメラのトランスフォームに適用する
		auto cam_trns = owner->transform;
		cam_trns->position = finaly_position;
		cam_trns->SetAxisZ(cam_vector_z);
	}

}