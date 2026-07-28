//---------------------------------------------------------------------
// 	@brief クレーターオブジェクト
// 	@details クレーターのモデルを保持するだけのオブジェクト
//				時間が経過すると地面に沈んでいき、最終的に削除される
//---------------------------------------------------------------------
#pragma once
#include "Game/Objects/NeonFade/GameObjectWithLifeTime.h"

namespace NeonFade {

	class CraterObject :
		public GameObjectWithLifeTime
	{
	public:
		USING_SUPER(GameObjectWithLifeTime);
		CraterObject(const float lifetime_) :GameObjectWithLifeTime(lifetime_) {};
		int Init() override;
		void Update() override;

		static inline const Vector3 SINK_VIBRATION_AMPLITUDE = { 0.05f,0.0f,0.05f }; //!< 沈むときの振動の振幅
		static constexpr float SINK_THRESHOLD = 4.0f; //!< 沈む速度が変化する閾値(単位: 秒) 
		static constexpr float SINK_SPEED = 3.0f; //!< 沈む速度(単位: m/s)
		static constexpr float SINK_SPEED_VIBRATION = 0.5f; //!< 沈む速度に少しだけ振動を加える(単位: m/s)

	};

}

