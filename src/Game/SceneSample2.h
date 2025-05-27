#pragma once
#include "System/Scene.h"
class SceneSample2 :
    public Scene
{
    USING_SUPER(SceneSample2);
    int Init() override;
    void Update() override;
    void PreDraw() override;
    void LateDebugDraw() override;
    void Exit() override;
    ObjBaseWP obj;
    ObjBaseWP obj2;
};

