#pragma once
#include "System/Scene.h"
USING_PTR(SampleObject);
class SceneSample :
	public Scene
{
public:
	USING_SUPER(SceneSample);
	void Load() override;
	int Init() override;
	void Update() override;
	void PreDraw() override;
	void Draw() override;
	void LateDraw() override;
	void DebugDraw() override;
	void LateDebugDraw() override;
	void UnLoad() override;
	SampleObjectWP obj;
	ObjectWP obj2;
};