#pragma once
#include "System/Object.h"
#include "System/Components/ModelRenderer.h"
namespace RLyeh {
	class CardBase :
		public GameObject
	{
	public:
		using Super = Class; 
		using Class = CardBase;
		static inline const char* Name() {
			return typeid(CardBase).name();
		}; 
		static inline ClassTypeInfo<CardBase> info = ClassTypeInfo<CardBase>(Name(), &Super::info); 
		inline virtual TypeInfo* Info() {
			return static_cast<TypeInfo*>(&info);
		};;
		int Init() override;
		void Update() override;
		void Exit() override;
		void LateDraw() override;
		void SetTexture(int tex);
		ModelRendererWP model;
		int mode = 0;
		float delta = 0;
		int my_texture = -1;
		Vector3 start_pos;
	};
}

