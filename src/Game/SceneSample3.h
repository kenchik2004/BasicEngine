#pragma once
#include "System/Scene.h"
class SceneSample3 :
    public Scene
{
public:
    USING_SUPER(SceneSample3);
    int Init() override;
    void Update() override;
    ObjBaseWP plane;
};

