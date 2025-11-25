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
		void Down(Vector3 vec, int damage = 1);
		bool IsDead();

		void OnTriggerEnter(const HitInfo& hit_info);

		ModelRendererWP model;
		AnimatorWP animator;
		RigidBodyWP rb;
		EnemyControllerWP enem_controller;
		ColliderWP col;
		static inline Material* death_material = nullptr;
	};
}

