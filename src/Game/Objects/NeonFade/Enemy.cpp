#include "precompile.h"
#include "Enemy.h"
#include "Game/Components/EnemyController.h"

namespace NeonFade {
	int Enemy::Init()
	{

		rb = AddComponent<RigidBody>();
		model = AddComponent<ModelRenderer>();
		animator = AddComponent<Animator>();

		model->SetModel("enemy_model");
		animator->SetAnimation("enemy_idle", 0);
		animator->SetAnimation("enemy_damage", 0);
		animator->SetAnimation("enemy_down", 0);


		rb->freeze_rotation = { 1,1,1 };
		auto col = AddComponent<CapsuleCollider>();
		col->height = 7.5f;
		col->radius = 1.5f;
		col->position = { 5.4f,0,0 };
		col->rotation = Quaternion(DEG2RAD(90), { 0,0,1 });
		col->SetLayer(Collider::Layer::Enemy);
		col->SetHitGroup(Collider::Layer::Player | Collider::Layer::Terrain | Collider::Layer::Wepon | Collider::Layer::Vehicle);
		enem_controller = AddComponent<EnemyController>();
		return 0;
	}
	void Enemy::Update()
	{
	}
	void Enemy::Damage(int damage)
	{
		enem_controller->Damage();
	}
	void Enemy::Down(Vector3 vec)
	{
		if (vec.magnitudeSquared() <= FLT_EPSILON)
			vec = transform->AxisZ();
		enem_controller->Down(vec);
	}
}