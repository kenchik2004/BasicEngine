#pragma once
namespace NeonFade {

	class Player;
	class Enemy;
	class MiniMapObject :
		public UIObject
	{
	public:
		USING_SUPER(MiniMapObject);
		int Init() override;
		void Update() override;
		void LateDebugDraw() override;

	private:
		SafeWeakPtr<NeonFade::Player> player = nullptr;	// ミニマップ上に表示するプレイヤーへの弱参照

		SafeWeakPtr<ImageRenderer> map_img_renderer = nullptr;	// ミニマップの背景画像レンダラーコンポーネントへの弱参照
	};
}

