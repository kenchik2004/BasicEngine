#pragma once
#include "System/Scene.h"
class SceneSample :
	public Scene
{
public:
	USING_SUPER(SceneSample);
	void Load() override;
	int Init() override;
	void Update() override;
	void LateDraw() override;
	void DebugDraw() override;
	void UnLoad() override;
};