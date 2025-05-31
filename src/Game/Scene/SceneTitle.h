#pragma once
#include "System/Scene.h"
USING_PTR(DiceScene);
namespace RLyeh {

class SceneTitle :
    public Scene
{
public:
    USING_SUPER(SceneTitle);
    void Load() override;
    int Init()override;
    void Update() override;
    void LateDraw() override;
    void UnLoad() override;
    SafeSharedPtr<AudioClip> bgm = nullptr;
    DiceSceneWP dice_scene;
    int title_img = -1;
};
}

