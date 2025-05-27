#pragma once
#include "System/Component.h"
USING_PTR(ModelRenderer);
USING_PTR(Animator);
USING_PTR(CapsuleCollider);
USING_PTR(SampleCutIn);
class SampleAttack :
	public Component
{
public:
	USING_SUPER(SampleAttack);
	int Init() override;
	void Update() override;
	void Exit() override;
	void OnTriggerEnter(const HitInfo& hit_info);
	float timer = 0.0f;

	ModelRendererWP model;
	AnimatorWP animator;
	CapsuleColliderWP fist;
	SampleCutInWP cut_in;
};

