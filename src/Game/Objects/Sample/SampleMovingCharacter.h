#pragma once
#include "System/Object.h"

USING_PTR(RigidBody);
USING_PTR(ModelRenderer);
USING_PTR(Animator);
class SampleMovingCharacter :
	public GameObject
{
public:
	SampleMovingCharacter() = default;
	SampleMovingCharacter(bool ignore_input_, bool use_rigidbody_)
	{
		ignore_input = ignore_input_;
		use_rigidbody = use_rigidbody_;
	};
	USING_SUPER(SampleMovingCharacter);
	int Init() override;
	void Update() override;
	void Exit() override;
	u8 movement_state = 0; // 0:idle, 1:run, 2:jump
	bool ignore_input = false;
	bool use_rigidbody = true;
private:
	ModelRendererWP my_model = nullptr;
	AnimatorWP my_animator = nullptr;
	RigidBodyWP my_rb = nullptr;
	bool is_jumping = false;

};

