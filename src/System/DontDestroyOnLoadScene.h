#pragma once
#include "Scene.h"
class DontDestroyOnLoadScene :
    public Scene
{
public:
   void DontDestroyOnLoad(ObjBaseP object,SceneP from_where);
};

