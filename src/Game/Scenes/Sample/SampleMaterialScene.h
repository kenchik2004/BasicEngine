#pragma once
#include "System/Scene.h"

namespace Sample {
	class SampleMaterialScene :
		public Scene
	{
	public:
		USING_SUPER(SampleMaterialScene);
		void Load() override;
		int Init() override;
		void Update() override;
		void LateDraw() override;
		void OnDrawFinish() override;
		void Exit() override;
		bool CheckForLoading(); //!< ロード状況の確認
	private:
		Material* tv_screen_mat = nullptr;
		SafeSharedPtr<Texture> tv_screen_texture;
		Material* player_default_mat = nullptr;
		Material* player_override_mat = nullptr;
		SafeSharedPtr<Texture> player_override_texture = nullptr;
		SafeWeakPtr<ModelRenderer> player_model = nullptr;
		bool default_texture_enabled = true;
	};

}