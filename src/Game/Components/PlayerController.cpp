// PlayerController.cpp
//---------------------------------------------------------------------------
//<! @file   PlayerController.cpp
//<! @brief  PlayerControllerコンポーネントの実装。プレイヤーキャラクターの入力処理を管理する
//---------------------------------------------------------------------------
#include "PlayerController.h"
#include "Game/Utilitys/NeonFade/StateMachines/PlayerStateMachine.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Components/PlayerCameraMachine.h"

namespace NeonFade
{
	void PlayerController::OnSleep()
	{
		// カメラマシンを無効化する
		owner_player->player_camera_machine->Sleep(false);
	}

	void PlayerController::OnWakeUp()
	{
		// カメラマシンを有効化する
		owner_player->player_camera_machine->WakeUp();
	}

	int PlayerController::Init()
	{
		// このコンポーネントを持つオーナーをPlayer型にキャストしてnullセーフなshared_ptrとして取得する
		owner_player = SafeStaticCast<Player>(owner.lock());
		// 取得したPlayerオブジェクトを基にプレイヤー専用のステートマシンを構築する
		state_machine = make_safe_unique<PlayerStateMachine>(owner_player.lock().get());

		// 親クラスの初期化処理を呼び出す
		return Super::Init();
	}

	void PlayerController::Update()
	{
		ProcessMoveInput();
		ProcessDodgeInput();
		ProcessAttackInput();
		ProcessClimbRaycast();
		UpdateFallingAndClimbingStates();
		ProcessJumpInput();
		TransmitInputToStateMachine();
	}

	void PlayerController::ProcessMoveInput()
	{
		// プレイヤーに紐づくカメラのトランスフォームを取得して移動の基準にする
		Transform* cam_trns_raw = owner_player->player_camera->transform.get();

		constexpr int PAD_INDEX = 0;
		// パッドの左スティックの入力値を取得する
		Vector2 pad_left = Input::GetPadLeftStick(PAD_INDEX);

		// 移動入力を一旦ゼロリセットする
		move_input = { 0, 0, 0 };

		// デバッグビルド時のみ、パッド入力が小さい場合にキーボード入力を評価する
#ifdef __DEBUG
		constexpr float PAD_EPSILON = FLT_EPSILON;
		// パッドの入力が極めて小さい場合はキーボード入力に基づく移動を評価する
		if (pad_left.magnitudeSquared() <= PAD_EPSILON) {
			// W・S・A・Dキーの入力に応じてカメラの前後左右のベクトルを加減算する
			if (Input::GetKey(KeyCode::W))
				move_input += cam_trns_raw->AxisZ();
			if (Input::GetKey(KeyCode::S))
				move_input -= cam_trns_raw->AxisZ();
			if (Input::GetKey(KeyCode::A))
				move_input -= cam_trns_raw->AxisX();
			if (Input::GetKey(KeyCode::D))
				move_input += cam_trns_raw->AxisX();
		}
		else
#endif
		{
			// パッド入力がある場合はスティックの傾きに応じてカメラの前後左右のベクトルを合成する
			move_input += cam_trns_raw->AxisZ() * pad_left.y;
			move_input += cam_trns_raw->AxisX() * pad_left.x;
		}

		// Y軸成分を無視して水平面上のベクトルに投影し正規化して純粋な移動方向を得る
		const Vector3 PLANE_NORMAL = { 0, 1, 0 };
		move_input = ProjectOnPlane(move_input, PLANE_NORMAL).normalized();
	}

	void PlayerController::ProcessDodgeInput()
	{
		// 回避フラグを一旦リセットする
		is_dodging = false;

		constexpr int PAD_INDEX = 0;
		// 指定されたキーまたはボタンが押されていれば回避フラグを立てる
		if (Input::GetKeyDown(KeyCode::O) || Input::GetPadButtonDown(PAD_INDEX, DODGE_BUTTON)) {
			is_dodging = true;
		}
	}

	void PlayerController::ProcessAttackInput()
	{
		constexpr int PAD_INDEX = 0;
		// 攻撃用のキーやボタンが押されているか判定してフラグを更新する
		is_attacking = Input::GetKeyDown(KeyCode::P)
			|| Input::GetPadButtonDown(PAD_INDEX, NORMAL_ATTACK_BUTTON)
			|| Input::GetPadButtonDown(PAD_INDEX, KNOCK_UP_ATTACK_BUTTON)
			|| Input::GetPadButtonDown(PAD_INDEX, SHOOT_ATTACK_BUTTON);
	}

	void PlayerController::ProcessClimbRaycast()
	{
		constexpr float FORWARD_OFFSET = 1.5f;
		constexpr float HEIGHT_OFFSET = 7.0f;
		constexpr float RAY_DISTANCE = 5.0f;
		constexpr float HIT_DISTANCE_THRESHOLD = 2.0f;
		constexpr float NORMAL_DOT_THRESHOLD = 0.8f;
		constexpr float CLIMB_END_HEIGHT_OFFSET = 1.5f;
		const Vector3 UP_VECTOR = { 0, 1, 0 };
		const Vector3 RAY_DIR = { 0, -1, 0 };

		// 登攀判定用のレイの開始位置をプレイヤーの前方上空に設定する
		climb_ray_start = owner_player->transform->position + owner_player->transform->AxisZ() * FORWARD_OFFSET + Vector3(0, HEIGHT_OFFSET, 0);

		// レイキャストの結果を格納する変数を準備する
		RayCastInfo climb_ray_info;

		// 地形レイヤーのみを対象としたフィルタデータを構築する
		physx::PxQueryFilterData filter_data;
		filter_data.data.word0 = Collider::Layer::Terrain;
		filter_data.flags = physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::ePREFILTER;

		// シーン空間に対してレイキャストを実行して壁の上端や足場を検知する
		owner->GetScene()->RayCast(Ray(climb_ray_start, RAY_DIR, RAY_DISTANCE), climb_ray_info, filter_data);

		// レイが何かにヒットした場合の処理を行う
		if (climb_ray_info.hasBlock) {
			// ヒットした地点の法線と座標を保存する
			climb_hit_normal = climb_ray_info.block.normal;
			climb_hit_point = climb_ray_info.block.position;
			// レイの開始位置からの距離を取得する
			float climb_hit_distance = climb_ray_info.block.distance;
			// ヒット位置が十分に低くかつ上を向いている平面であれば登攀可能と判定する
			can_climb = (climb_ray_info.hasBlock && climb_hit_distance >= HIT_DISTANCE_THRESHOLD && climb_hit_normal.dot(UP_VECTOR) > NORMAL_DOT_THRESHOLD);
			// ステートマシンに対して登攀開始位置と終了位置を計算してセットする
			state_machine->climb_start_pos = owner_player->transform->position;
			state_machine->climb_end_pos = climb_hit_point + Vector3(0, CLIMB_END_HEIGHT_OFFSET, 0);
		}
		else {
			// ヒットしなかった場合は登攀不可とする
			can_climb = false;
		}
	}

	void PlayerController::UpdateFallingAndClimbingStates()
	{
		// 落下を検知するための時間を加算する
		fall_detect_time += Time::UnscaledDeltaTime();

		constexpr float MOVE_EPSILON = FLT_EPSILON;
		// 登攀可能でありかつ移動入力が存在する場合は登攀状態に移行する
		if (can_climb && move_input.magnitudeSquared() > MOVE_EPSILON)
			is_climbing = true;

		// 空中にいる時間が閾値を超えたら落下中と判定する
		is_falling = (fall_detect_time >= FALL_DETECT_THRESHOLD);
	}

	void PlayerController::ProcessJumpInput()
	{
		// ジャンプフラグを一旦リセットする
		is_jumping = false;

		constexpr int PAD_INDEX = 0;

		// デバッグビルド時のみジャンプ入力の検知をデバッグ出力する
#ifdef __DEBUG
		// ジャンプの入力検知とデバッグ出力を実行する
		if ((Input::GetKeyDown(KeyCode::Space) || Input::GetPadButtonDown(PAD_INDEX, JUMP_BUTTON)))
			printfDx("jump!!\n");
#endif

		// 地面に接している状態でジャンプ入力があればジャンプフラグを立てて着地フラグをへし折る
		if (is_landed && (Input::GetKeyDown(KeyCode::Space) || Input::GetPadButtonDown(PAD_INDEX, JUMP_BUTTON))) {
			is_jumping = true;
			is_landed = false;
		}
		else if (is_landed) {
			// ジャンプ入力がなく着地していれば落下とジャンプのフラグをへし折る
			is_jumping = false;
			is_falling = false;
		}
		// 登攀中及び攻撃中の場合はジャンプをキャンセルし落下検知タイマーをリセットする
		if (is_climbing || is_attacking) {
			is_jumping = false;
			constexpr float RESET_TIME = 0.0f;
			fall_detect_time = RESET_TIME;
		}
	}

	void PlayerController::TransmitInputToStateMachine()
	{
		// ステートマシンに入力を渡す
		if (!ignore_input)
		{
			// 収集した各種入力状態をステートマシンに伝達する
			state_machine->is_attacking = is_attacking;
			state_machine->move_input = move_input;
			state_machine->is_jumping = is_jumping;
			state_machine->is_dodging = is_dodging;
			state_machine->can_climb = can_climb;
		}
		// 入力を無視する場合は各種入力フラグをリセットしてステートマシンに渡す
		else {
			state_machine->is_attacking = is_attacking = false;
			state_machine->move_input = move_input = { 0, 0, 0 };
			state_machine->is_jumping = is_jumping = false;
			state_machine->is_dodging = is_dodging = false;
			state_machine->can_climb = can_climb = false;
		}
		// 環境判定系のフラグは入力無視設定に関わらず伝達する
		state_machine->is_landed = is_landed;
		state_machine->is_falling = is_falling;

		// 前フレームでのダメージ状態をステートマシンに伝達する
		state_machine->is_damaged = is_damaged;
		// ステートマシンを更新して状態遷移やアニメーション処理を進める
		state_machine->Update(Time::UnscaledDeltaTime());

		// 伝達し終えたら自身のダメージフラグを折る
		is_damaged = false;
	}

	void PlayerController::Exit()
	{}

	void PlayerController::LateDebugDraw()
	{
		constexpr float LINE_LENGTH = -5.0f;
		constexpr float NORMAL_LINE_LENGTH = 5.0f;
		constexpr float SPHERE_RADIUS = 0.5f;
		constexpr int SPHERE_DIVISIONS = 16;
		constexpr unsigned int COLOR_MAGENTA = 0xff00ff;
		constexpr unsigned int COLOR_YELLOW = 0xffff00;
		constexpr unsigned int COLOR_DARK_GRAY = 0x111111;
		constexpr unsigned int COLOR_RED = 0xff0000;
		constexpr unsigned int COLOR_BLACK = 0x0000;

		// 登攀判定用のレイの軌跡をマゼンタの線で描画する
		DrawLine3D(cast(climb_ray_start), cast(climb_ray_start + Vector3(0, LINE_LENGTH, 0)), COLOR_MAGENTA);
		// レイがヒットした地点が存在する場合のデバッグ描画処理を行う
		if (climb_hit_point != Vector3(0, 0, 0)) {
			// 登攀可能かどうかに応じて色を変えてヒット地点に球を描画する
			DrawSphere3D(cast(climb_hit_point), SPHERE_RADIUS, SPHERE_DIVISIONS, can_climb ? COLOR_YELLOW : COLOR_DARK_GRAY, COLOR_BLACK, true);
			// ヒット地点から法線方向に赤い線を描画する
			DrawLine3D(cast(climb_hit_point), cast(climb_hit_point + climb_hit_normal * NORMAL_LINE_LENGTH), COLOR_RED);
		}
		// ステートマシン自身のデバッグ描画を呼び出す
		state_machine->DebugDraw();
	}

	void PlayerController::OnCollisionEnter(const HitInfo& hit_info)
	{
		// 衝突した相手のレイヤー情報を取得する
		auto layer = hit_info.hit_collision->GetLayer();
		// 地形または敵に衝突した場合は着地とみなす
		if (layer == Collider::Layer::Terrain || layer == Collider::Layer::Enemy) {
			is_landed = true;
			// 登攀中であれば着地により登攀状態を解除する
			if (is_climbing)
				is_climbing = false;
		}
		// ステートマシンに衝突開始イベントを伝達する
		state_machine->OnCollisionEnter(hit_info);
	}

	void PlayerController::OnCollisionStay(const HitInfo& hit_info)
	{
		// 接触し続けている相手のレイヤー情報を取得する
		auto layer = hit_info.hit_collision->GetLayer();
		// 地形または敵に接触し続けている場合は着地状態を維持し落下タイマーをリセットする
		if (layer == Collider::Layer::Terrain || layer == Collider::Layer::Enemy) {
			is_landed = true;
			constexpr float RESET_TIME = 0.0f;
			fall_detect_time = RESET_TIME;
		}
		// ステートマシンに衝突継続イベントを伝達する
		state_machine->OnCollisionStay(hit_info);
	}

	void PlayerController::OnCollisionExit(const HitInfo& hit_info)
	{
		// 離れた相手のレイヤー情報を取得する
		auto layer = hit_info.hit_collision->GetLayer();
		// 地形または敵から離れた場合は着地状態を解除する
		if (layer == Collider::Layer::Terrain || layer == Collider::Layer::Enemy) {
			printfDx("exit landed\n");
			is_landed = false;
		}
		// ステートマシンに衝突終了イベントを伝達する
		state_machine->OnCollisionExit(hit_info);
	}

	void PlayerController::OnTriggerEnter(const HitInfo& hit_info)
	{
		// ステートマシンにトリガー領域への侵入イベントを伝達する
		state_machine->OnTriggerEnter(hit_info);
	}

	void PlayerController::OnTriggerStay(const HitInfo& hit_info)
	{
		// ステートマシンにトリガー領域での滞在イベントを伝達する
		state_machine->OnTriggerStay(hit_info);
	}

	void PlayerController::OnTriggerExit(const HitInfo& hit_info)
	{
		// ステートマシンにトリガー領域からの退出イベントを伝達する
		state_machine->OnTriggerExit(hit_info);
	}

	void PlayerController::Damage(int damage_)
	{
		// ダメージを受けたことを示すフラグを立てる
		is_damaged = true;
	}

} // namespace NeonFade