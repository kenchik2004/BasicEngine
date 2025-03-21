#pragma once
#include "System/Component.h"

USING_PTR(ModelRenderer);

class SampleCutIn :
	public Component
{
public:
	USING_SUPER(SampleCutIn);
	int Init() override;
	void PostUpdate() override;
	void LateDraw() override;
	void Exit() override;
	int handle = -1;
	int image = -1;
	int se = -1;
	float timer = 1.0f;
	Vector3 cam_pos = { 0,0,0 };
	Vector3 cam_lookat = { 0,0,0 };
	float3 pos = { 0,0,0 };

	ModelRendererWP model;
};

