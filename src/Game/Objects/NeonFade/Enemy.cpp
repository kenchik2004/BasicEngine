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
		animator->SetAnimation("enemy_die", 0);
		animator->SetAnimation("enemy_attack_charge", 0);
		animator->SetAnimation("enemy_attack_main", 0);


		rb->freeze_rotation = { 1,1,1 };
		auto col_ = AddComponent<CapsuleCollider>();
		col_->SetMaterial(PhysicMaterial::ZeroFriction);
		col_->height = 7.5f;
		col_->radius = 1.5f;
		col_->position = { 5.4f,0,0 };
		col_->rotation = Quaternion(DEG2RAD(90), { 0,0,1 });
		col_->SetLayer(Collider::Layer::Enemy);
		col_->SetHitGroup(Collider::Layer::Player | Collider::Layer::Terrain | Collider::Layer::Wepon | Collider::Layer::Vehicle);
		col = col_;
		enem_controller = AddComponent<EnemyController>();
		if (!death_material) {

			auto death_shader = MaterialManager::LoadPixelShader("data/shader/enemy_death_ps.fx", "neonfade_enemy_death_ps");
			auto death_texture = TextureManager::Get("fx_texture");

			death_material = MaterialManager::CreateMaterial("neonfade_enemy_death_material");
			death_material->SetShaderPs(death_shader, true);
			death_material->SetTexture(death_texture, Material::TextureType::Specular);
		}
		return 0;
	}
	void Enemy::Update()
	{
		if (Input::GetKeyDown(KeyCode::Y)) {
			Down({ 0,0,1 }, 50);
		}
	}
	void Enemy::Damage(int damage)
	{
		enem_controller->Damage(damage);
	}
	void Enemy::Down(Vector3 vec, int damage)
	{
		if (vec.magnitudeSquared() <= FLT_EPSILON)
			vec = transform->AxisZ();
		enem_controller->Down(vec, damage);
	}
	bool Enemy::IsDead()
	{
		return enem_controller->hp <= 0;
	}
	void Enemy::OnTriggerEnter(const HitInfo& hit_info)
	{
		enem_controller->OnTriggerEnter(hit_info);
	}
}