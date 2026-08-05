// PlayerCameraMachine.h
//---------------------------------------------------------------------------
//! @file   PlayerCameraMachine.h
//! @brief  プレイヤーカメラコンポーネント（プレイヤー追従カメラ制御）
//---------------------------------------------------------------------------
#pragma once
namespace NeonFade {
	class Player;
	//---------------------------------------------------------------------
	//! @class PlayerCameraMachine
	//! @brief プレイヤーに追従するカメラ制御コンポーネント
	//---------------------------------------------------------------------
	class PlayerCameraMachine :
		public Component
	{
	public:
		//! @brief カメラモード
		enum CAMERA_MODE {
			MANIPULATE, //!< 操作モード
			CINEMATIC,  //!< シネマティックモード
		};
		USING_SUPER(Component);

		//カメラの設定をロードする
		void LoadCameraSettings();

		//! @brief 初期化処理
		int Init() override;

		//! @brief 更新処理
		void Update() override;

		//! @brief 描画前処理
		void PreDraw() override;

		//! @brief カメラシェイクを開始する
		//! @param intensity_ シェイクの強さ
		//! @param duration_ シェイクの持続時間
		void ShakeCamera(const Vector3& intensity_, float duration_);

		//! @brief 追従対象を設定する
		//! @param target_ 追従対象のプレイヤーへの弱参照
		void SetTarget(SafeWeakPtr<Player> target_) { target = target_; }

		//! @brief カメラモードを設定する
		//! @param mode_ 設定するカメラモード
		//! @param transition_enable トランジションを有効にするかどうか
		void SetCameraMode(CAMERA_MODE mode_, bool transition_enable = true);
		
		//! @brief 現在のカメラモードを取得する
		CAMERA_MODE GetCameraMode() const { return mode; }

		//! @brief シネマティックオフセットを設定する
		//! @param offset_ 設定するシネマティックオフセット
		void SetCinematicOffset(const Vector3& offset_) { cinematic_offset = offset_; }

		//! @brief トランジション時間を設定する
		//! @param time_ 設定するトランジション時間
		void SetTransitionTime(float time_) { transition_time_max = time_; }

		//! @brief カメラの回転角度を初期状態にリセットする
		void ResetCameraRot() { camera_rot = { 0,0 }; }

		float camera_distance_max = 30.0f; //!< カメラ最大距離
	private:
		CAMERA_MODE mode; //!< 現在のカメラモード

		float transition_timer = 0.0f; //!< トランジションタイマー
		float transition_time_max = 0.5f; //!< トランジション最大時間

		SafeWeakPtr<Player> target; //!< 追従対象プレイヤー

		Vector3 shake_offset = { 0,0,0 }; //!< シェイクオフセット
		Vector3 shake_intensity = { 0.0f, 0.0f, 0.0f }; //!< シェイク強度
		float shake_duration = 0.0f; //!< シェイク持続時間

		bool reverse_y = false; //!< Y軸反転フラグ
		bool reverse_x = false; //!< X軸反転フラグ

		Vector2 camera_rot = { 0,0 }; //!< カメラ回転角度
		Vector3 cinematic_offset = { 0,0,-1 }; //!< シネマティックオフセット
		float camera_distance = 20.0f; //!< カメラ距離


		static constexpr float camera_rot_speed = 270.0f; //!< カメラ回転速度(度/秒)
		static constexpr float camera_rot_limit_min = -45.0f; //!< カメラ回転制限角度(度)
		static constexpr float camera_rot_limit_max = 30.0f; //!< カメラ回転制限角度(度)


		//! @brief 入力を適用する
		//! @param dt デルタタイム
		void ApplyInput(float dt);

		//! @brief カメラのオフセットを計算する
		//! @param target_pos 追従対象の位置
		//! @param offset カメラのオフセット
		Vector3 CreateCameraVector(const Vector3& target_pos, const Vector3& offset);
		
		//! @brief カメラ距離を調整する
		//! @param target_transform 追従対象のTransform
		//! @param cam_vector_z カメラの前方ベクトル
		void AdjustCameraDistance(Transform* target_transform, Vector3& cam_vector_z);


	};
}