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
		//! @brief 初期化処理
		int Init() override;
		//! @brief 描画前処理
		void PreDraw() override;
		//! @brief 更新処理
		void Update() override;
		//! @brief カメラシェイクを設定する
		void ShakeCamera(float intensity_, float duration_);
		//! @brief 追従対象を設定する
		void SetTarget(SafeWeakPtr<Player> target_) { target = target_; }
		//! @brief カメラモードを設定する
		void SetCameraMode(CAMERA_MODE mode_, bool transition_enable = true) { mode = mode_; if (transition_enable)transition_timer = transition_time_max; }
		//! @brief 現在のカメラモードを取得する
		CAMERA_MODE GetCameraMode() const { return mode; }
		//! @brief シネマティックオフセットを設定する
		void SetCinematicOffset(const Vector3& offset_) { cinematic_offset = offset_; }
		//! @brief トランジション時間を設定する
		void SetTransitionTime(float time_) { transition_time_max = time_; }
		float camera_distance_max = 30.0f; //!< カメラ最大距離
	private:
		CAMERA_MODE mode; //!< 現在のカメラモード
		float transition_timer = 0.0f; //!< トランジションタイマー
		float transition_time_max = 0.5f; //!< トランジション最大時間
		SafeWeakPtr<Player> target; //!< 追従対象プレイヤー
		Vector3 shake_offset = { 0,0,0 }; //!< シェイクオフセット
		float shake_intensity = 0.0f; //!< シェイク強度
		float shake_duration = 0.0f; //!< シェイク持続時間
		bool reverse_y = false; //!< Y軸反転フラグ
		bool reverse_x = false; //!< X軸反転フラグ

		Vector2 camera_rot = { 0,0 }; //!< カメラ回転角度
		Vector3 cinematic_offset = { 0,0,-1 }; //!< シネマティックオフセット
		float camera_distance = 20.0f; //!< カメラ距離

	};
}
