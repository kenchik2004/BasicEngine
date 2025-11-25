#pragma once
USING_PTR(ModelRenderer);
USING_PTR(Animator);
namespace Sample
{

	class SampleAnimationObject :
		public GameObject
	{
	public:
		USING_SUPER(SampleAnimationObject);
		int Init() override;
		void Update() override;
		void Exit() override;
	private:
		ModelRendererWP my_model = nullptr;
		AnimatorWP my_animator = nullptr;
	};
}

