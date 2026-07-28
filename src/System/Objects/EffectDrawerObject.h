#pragma once
#include "Object.h"
class EffectDrawerObject :
	public GameObject
{
public:
	USING_SUPER(GameObject);
	EffectDrawerObject() :
		GameObject(1999) {}
	void LateDraw() override;

};

