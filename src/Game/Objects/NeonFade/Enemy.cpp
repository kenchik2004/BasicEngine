//---------------------------------------------------------------------------
//! @file   Enemy.cpp
//! @brief  Enemyオブジェクトの実装。敵キャラクターのゲームロジックを管理する
//---------------------------------------------------------------------------
#include "precompile.h"
#include "Enemy.h"
#include "Game/Components/EnemyController.h"


#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/BasicEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/LeaderEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/TeamMemberEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyTeam.h"

namespace NeonFade {
	int Enemy::Init()
	{


		{

			rb = AddComponent<RigidBody>();
			rb->GetBody()->is<physx::PxRigidDynamic>()->setSleepThreshold(0.0f);
			auto model_obj = SceneManager::Object::Create<GameObject>("enem_model");
			model_obj->transform->SetParent(transform);
			model_obj->transform->scale = { 0.05f,0.05f,0.05f };
			model_obj->transform->local_position = { 0,-4.5f,0 };
			model_obj->transform->local_rotation = Quaternion(DEG2RAD(180), { 0,1,0 });
			model = model_obj->AddComponent<ModelRenderer>();
			animator = model_obj->AddComponent<Animator>();
			model->SetModel("enemy_model_LOD");
			animator->SetAnimation("enemy_idle", 0);
			animator->SetAnimation("enemy_damage", 0);
			animator->SetAnimation("enemy_down", 0);
			animator->SetAnimation("enemy_down_forward", 0);
			animator->SetAnimation("enemy_walk", 0);
			animator->SetAnimation("enemy_die", 0);
			animator->SetAnimation("enemy_attack_charge", 0);
			animator->SetAnimation("enemy_attack_main", 0);
			animator->SetAnimation("enemy_escape", 0);
			animator->SetAnimation("enemy_instruct", 0);
			animator->SetAnimation("enemy_stepback", 0);





			rb->freeze_rotation = { 1,1,1 };
			auto col_ = AddComponent<CapsuleCollider>();
			col_->height = 5.7f;
			col_->radius = 1.5f;
			col_->rotation = Quaternion(DEG2RAD(90), { 0,0,-1 });
			col_->SetLayer(Collider::Layer::Enemy);
			enem_controller = AddComponent<EnemyController>();
			col = col_;
		}

		if (!death_material) {

			auto death_shader = MaterialManager::LoadPixelShader("data/shader/enemy_death_ps.fx", "neonfade_enemy_death_ps");
			auto death_texture = TextureManager::Get("fx_texture");

			death_material = MaterialManager::CreateMaterial("neonfade_enemy_death_material");
			death_material->SetShaderPs(death_shader, true);
			death_material->SetTexture(death_texture, Material::TextureType::Specular);
			auto default_diffuse = model->GetMaterial(0)->GetTexture(Material::TextureType::Diffuse);
			death_material->SetTexture(default_diffuse, Material::TextureType::Diffuse);
		}
		return 0;
	}
	void Enemy::Update()
	{}
	void Enemy::Damage(int damage, bool ignore_i_frame)
	{
		enem_controller->Damage(damage, ignore_i_frame);
	}
	void Enemy::Down(Vector3 vec)
	{
		if (vec.magnitudeSquared() <= 1e-6f * 1e-6f)
			vec = transform->AxisZ();
		enem_controller->Down(vec);
	}
	bool Enemy::IsDead()
	{
		return enem_controller->IsDead();
	}
	u32 Enemy::GetHp() {
		return enem_controller->GetHp();
	}
	void Enemy::OnTriggerEnter(const HitInfo& hit_info)
	{
		enem_controller->OnTriggerEnter(hit_info);
	}


	//不完全初期化状態の敵を作成してコントローラーを返す
	//脳が作成・セットされてないので、
	//プレイヤーポインタとステートマシンで初期化する必要がある
	//なんかモダンで推奨されてるらしいから構造化束縛してみよう
	auto EnemyFactory::MakeAbstractEnemy() {
		//オブジェクト作成軽量化のためカウンタを使い名前をユニークにする
		auto team_member = SceneManager::Object::Create<Enemy>(u8"Enemy" + std::to_string(counter));
		Vector3 pos = spawn_pos;
		pos.x += sinf(Random::Range(0.0f, 360.0f)) * Random::Range(0.0f, spawn_radius);
		pos.z += sinf(Random::Range(0.0f, 360.0f)) * Random::Range(0.0f, spawn_radius);
		team_member->transform->position = pos;
		auto controller = team_member->enem_controller.lock().get();
		auto state_machine = controller->GetStateMachine();
		counter++;
		struct { EnemyController* controller; EnemyStateMachine* state_machine; } result = { controller, state_machine };
		return result;
	}

	//シンプルな単体行動を行う脳をセットされた敵を作成して、脳のポインタを返す
	BasicEnemyBrain* EnemyFactory::MakeBasicEnemy(SafeWeakPtr<Player> player) {
		auto [controller, machine] = MakeAbstractEnemy();
		controller->SetPlayer(player);
		auto brain = make_safe_unique<BasicEnemyBrain>(machine, player);
		controller->SetBrain(std::move(brain));

		return static_cast<BasicEnemyBrain*>(controller->GetBrain());
	}

	//リーダーに従い行動する脳がセットされた敵を作成して脳のポインタを返す
	TeamMemberEnemyBrain* EnemyFactory::MakeTeamMateEnemy(LeaderEnemyBrain* leader_ptr, SafeWeakPtr<Player> player, EnemyTeam* my_team) {
		auto [controller, machine] = MakeAbstractEnemy();
		controller->SetPlayer(player);
		auto brain = make_safe_unique<TeamMemberEnemyBrain>(machine, player, my_team);
		brain->AddLeader(leader_ptr);
		controller->SetBrain(std::move(brain));

		//moveした後は、コントローラーからアクセスする
		//作成した脳はチームメンバーのものなので、そのままキャストして返す
		return static_cast<TeamMemberEnemyBrain*>(controller->GetBrain());
	}

	//チームの統率を取る脳をセットされた敵を作成して脳のポインタを返す
	LeaderEnemyBrain* EnemyFactory::MakeLeader(SafeWeakPtr<Player> player, EnemyTeam* my_team) {
		auto [controller, machine] = MakeAbstractEnemy();
		controller->SetPlayer(player);
		controller->SetBrain(make_safe_unique<LeaderEnemyBrain>(machine, player, my_team));

		//moveした後は、コントローラーからアクセスする
		//作成した敵の脳はリーダーのものなので、そのままキャストして返す
		return static_cast<LeaderEnemyBrain*>(controller->GetBrain());
	}
	std::vector<EnemyTeamUP> EnemyFactory::MakeEnemyTeam(u32 team_count, u32 enem_per_team, SafeWeakPtr<Player> player) {
		auto teams = std::vector<SafeUniquePtr<EnemyTeam>>();
		for (u32 i = 0; i < team_count; i++) {
			auto enemy_team = make_safe_unique<EnemyTeam>();
			auto leader_brain = MakeLeader(player, enemy_team.get());
			enemy_team->SetLeader(leader_brain);
			for (u32 i = 1; i < enem_per_team; i++) {
				auto member = MakeTeamMateEnemy(leader_brain, player, enemy_team.get());
				enemy_team->AddMember(member);
			}
			teams.push_back(std::move(enemy_team));
		}
		return teams;
	}
}