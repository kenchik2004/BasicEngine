#include "DebugCameraScene.h"



void DebugCameraObject::PostPhysics() {
	// F5キーでDebugCameraの有効/無効を切り替える
	if (Input::GetKeyDown(KeyCode::F5)) {
		is_active = !is_active;
		if (is_active) {
			camera->WakeUp();
			camera->SetCurrentCamera(); // DebugCameraをアクティブカメラに設定
		}
		else {
			camera->Sleep();
			auto& active_cameras = GetScene()->GetActiveCamerasRef();
			for (auto& cam : active_cameras) {
				if (cam->status.status_bit.is(CompStat::STATUS::ACTIVE)) {
					cam->SetCurrentCamera(); // DebugCamera以外のアクティブカメラをカレントカメラに設定
					break;
				}
			}
		}
	}
	// DebugCameraが無効ならここで終わり
	if (!is_active)
		return;

	// 右クリックが押されている間のみ移動や回転を行う
	if (!Input::GetMouseButtonRepeat(MouseButton::ButtonRight))
		return;

	move_speed += Input::GetMouseWheelRotation(); // マウスホイールの回転量に応じて移動速度を調整
	move_speed = std::clamp(move_speed, MOVE_SPEED_MIN, MOVE_SPEED_MAX); // 移動速度を制限


	Vector2 mouse_delta = Input::GetMouseDelta();
	// マウスのX軸とY軸を入れ替える(水平回転と垂直回転を対応させるため)
	std::swap(mouse_delta.x, mouse_delta.y); 

	rot_euler += mouse_delta * rot_speed; // マウスの移動量に応じて回転角を更新
	rot_euler.x = std::clamp(rot_euler.x, -89.0f, 89.0f); // 上下の回転角を制限(ジンバルロックを防止)
	rot_euler.y = std::fmod(rot_euler.y, 360.0f); // 水平の回転角を制限


	// オイラー角からクォータニオンを作成してカメラの回転に適用
	Quaternion rot_x = Quaternion(DEG2RAD(rot_euler.x), Vector3(1, 0, 0));
	Quaternion rot_y = Quaternion(DEG2RAD(rot_euler.y), Vector3(0, 1, 0));


	//Quaternionの作成は三角関数を使用しているため、若干コストが高い
	//掛け算は高速なので、rot_yのエイリアスを作成して、rot_xを掛け算することで、rot_yの値を再利用して最終的な回転を得る
	Quaternion& final_rot = rot_y;
	final_rot *= rot_x;
	transform->rotation = final_rot;



	// WASDQEキーでカメラを移動する
	Vector3 move_dir = Vector3(0, 0, 0);
	if (Input::GetKey(KeyCode::W))
		move_dir.z += 1.0f;
	if (Input::GetKey(KeyCode::S))
		move_dir.z -= 1.0f;
	if (Input::GetKey(KeyCode::A))
		move_dir.x -= 1.0f;
	if (Input::GetKey(KeyCode::D))
		move_dir.x += 1.0f;
	if (Input::GetKey(KeyCode::Q))
		move_dir.y -= 1.0f;
	if (Input::GetKey(KeyCode::E))
		move_dir.y += 1.0f;

	move_dir = move_dir.getNormalized() * move_speed * Time::DeltaTime(); // 移動量を計算

	if (Input::GetKey(KeyCode::LShift))
		move_dir *= 2.0f; // Shiftキーを押している間は移動速度を倍にする

	// カメラの回転を使用し、ローカル座標系で移動方向を変換する
	Vector3& mov_local = move_dir; // エイリアスで捉える
	mov_local = transform->rotation.rotate(mov_local); // カメラの回転を適用してローカル座標系での移動量を計算


	transform->position += mov_local; // カメラの位置を更新




}

int NeonFade::DebugCameraScene::Init()
{

	debug_camera = SceneManager::Object::Create<DebugCameraObject>("DebugCamera"); // DebugCameraオブジェクトを作成
	if (!debug_camera)
		return -1;
	debug_camera->camera->render_type = Camera::RenderType::Deferred; // ディファードレンダリングに設定

	return Super::Init();
}
