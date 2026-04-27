//---------------------------------------------------------------------------
//! @file   HuntedEffect.h
//! @brief  ハンテッドエフェクトオブジェクト（追跡エフェクト演出）
//---------------------------------------------------------------------------
#pragma once

namespace NeonFade {
	//---------------------------------------------------------------------
	//! @class HuntedEffect
	//! @brief 追跡エフェクトを表示するUIオブジェクト
	//---------------------------------------------------------------------
	class HuntedEffect :
		public UIObject
	{
	public:
		USING_SUPER(UIObject);
		//! @brief 初期化処理
		int Init() override;
		//! @brief 更新処理
		void Update() override;

	private:
		float lifetime = 0.0f; //!< 生存タイマー
		static constexpr float LIFE_TIME = 30.0f; //!< 最大生存時間（秒）
	};

}
