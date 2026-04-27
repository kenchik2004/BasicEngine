//---------------------------------------------------------------------------
//! @file   PoliceCar.h
//! @brief  パトカーゲームオブジェクト
//---------------------------------------------------------------------------
#pragma once

class PointLight;
namespace NeonFade
{
	//---------------------------------------------------------------------
	//! @class PoliceCar
	//! @brief パトカーを表すゲームオブジェクト
	//---------------------------------------------------------------------
	class PoliceCar :
		public GameObject
	{
	public:
		USING_SUPER(GameObject);
		//! @brief 初期化処理
		int Init() override;
		//! @brief 更新処理
		void Update() override;
		//! @brief 終了処理
		void Exit() override;
		//! @brief トリガー開始イベント
		void OnTriggerEnter(const HitInfo& hit_info)override;
		//! @brief トリガー終了イベント
		void OnTriggerExit(const HitInfo& hit_info) override;
		//! @brief パス上の位置を設定する
		void SetPointOnpath(const float& t_) { t = t_; }

	private:
		static inline bool player_rideon = false; //!< プレイヤー乗車フラグ
		float t = 0.0f; //!< パス上のパラメーター
		RigidBodyWP rb = nullptr; //!< 物理ボディ
		AudioPlayerWP audio_player = nullptr; //!< オーディオプレイヤー
		SafeSharedPtr<PointLight> moving_light_red = nullptr; //!< 赤色回転灯ライト
		SafeSharedPtr<PointLight> moving_light_blue = nullptr; //!< 青色回転灯ライト
		std::vector<SafeWeakPtr<Object>> rideon_objs; //!< 乗車中のオブジェクトリスト
	};
}
