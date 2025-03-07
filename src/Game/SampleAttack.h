#pragma once
#include "System/Component.h"
USING_PTR(ModelRenderer);
USING_PTR(CapsuleCollider);
class SampleAttack :
	public Component
{
public:
	int Init() override;
	void Update() override;
	void Exit() override;
	void OnTriggerEnter(const HitInfo& hit_info);
	float timer = 0.0f;

	ModelRendererWP model;
	CapsuleColliderWP fist;
};

