//---------------------------------------------------------------------------
//! @file   DamageUIObject.h
//! @brief  ダメージUI表示オブジェクト
//---------------------------------------------------------------------------
#pragma once
namespace NeonFade {
	//---------------------------------------------------------------------
	//! @class DamageUIObject
	//! @brief ダメージ量をUI上に表示するオブジェクト
	//---------------------------------------------------------------------
	class DamageUIObject :
		public UIObject
	{
	public:
		USING_SUPER(DamageUIObject);
		//! @brief 初期化処理
		int Init() override;
		//! @brief 更新処理
		void Update() override;
	private:
		float life_timer = 0.0f; //!< 生存タイマー
		static constexpr float LIFE_TIME = 1.0f; //!< 生存時間（秒）
	};
}
