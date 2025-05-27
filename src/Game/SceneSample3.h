#pragma once
#include "System/Scene.h"
class SceneSample3 :
    public Scene
{
public:
    USING_SUPER(SceneSample3);
	void Load() override;
    int Init() override;
    void Update() override;
    ObjBaseWP plane;
};

