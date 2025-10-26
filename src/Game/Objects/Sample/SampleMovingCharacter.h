#pragma once
#include "System/Object.h"

USING_PTR(RigidBody);
USING_PTR(ModelRenderer);
USING_PTR(Animator);
class SampleMovingCharacter :
	public GameObject
{
public:
	USING_SUPER(SampleMovingCharacter);
	int Init() override;
	void Update() override;
	void Exit() override;
private:
	ModelRendererWP my_model = nullptr;
	AnimatorWP my_animator = nullptr;
	RigidBodyWP my_rb = nullptr;
	bool is_jumping = false;

};

