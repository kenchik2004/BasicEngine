#pragma once

#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"
namespace NeonFade {

	USING_PTR(EnemyController);
	class Enemy :
		public GameObject
	{
	public:
		USING_SUPER(Enemy);
		int Init() override;
		void Update() override;
		void Damage(int damage);
		void Down(Vector3 vec);

		ModelRendererWP model;
		AnimatorWP animator;
		RigidBodyWP rb;
		EnemyControllerWP enem_controller;
	};
}

