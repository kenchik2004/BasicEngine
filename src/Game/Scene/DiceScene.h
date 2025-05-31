#pragma once
#include "System/DontDestroyOnLoadScene.h"
class DiceScene :
    public DontDestroyOnLoadScene
{
public:
    USING_SUPER(DiceScene);
    int Init();
    void Load() override;
    void Roll(int first, int second);

    void Draw() override;
    void OnDrawFinish() override;
    void UnLoad() override;
    int texture[6] = { 0 };
    int screen = -1;
};

