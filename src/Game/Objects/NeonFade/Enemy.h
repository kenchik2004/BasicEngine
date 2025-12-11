#pragma once

#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"
namespace NeonFade {

	USING_PTR(EnemyController);
	USING_PTR(Player);
	class Enemy :
		public GameObject
	{
	public:
		USING_SUPER(Enemy);
		int Init() override;
		void Update() override;
		void Damage(int damage, bool ignore_i_frame = false);
		void Down(Vector3 vec);
		bool IsDead();
		u32 GetHp();

		void OnTriggerEnter(const HitInfo& hit_info);

		ModelRendererWP model;
		AnimatorWP animator;
		RigidBodyWP rb;
		EnemyControllerWP enem_controller;
		ColliderWP col;
		static inline Material* death_material = nullptr;
	};
	class BasicEnemyBrain;
	class TeamMemberEnemyBrain;
	class LeaderEnemyBrain;
	class EnemyFactory {
	private:
		static inline u64 counter = 0;
		Vector3 spawn_pos = { 0,0,0 };
		float spawn_radius = 5.0f;

	public:
		void SetSpawnPosition(const Vector3& pos) { spawn_pos = pos; }
		void SetSpawnRadius(float radius) { spawn_radius = radius; }
		//不完全初期化状態の敵を作成してコントローラーを返す
		//脳が作成・セットされてないので、
		//プレイヤーポインタとステートマシンで初期化する必要がある
		//なんかモダンで推奨されてるらしいからstd::tupleで構造化束縛してみよう
		std::tuple<EnemyController*, EnemyStateMachine*> MakeAbstractEnemy();

		//シンプルな単体行動を行う脳をセットされた敵を作成して、脳のポインタを返す
		BasicEnemyBrain* MakeBasicEnemy(SafeWeakPtr<Player> player);

		//リーダーに従い行動する脳がセットされた敵を作成して脳のポインタを返す
		TeamMemberEnemyBrain* MakeTeamMateEnemy(LeaderEnemyBrain* leader_ptr, SafeWeakPtr<Player> player);

		//チームの統率を取る脳をセットされた敵を作成して脳のポインタを返す
		LeaderEnemyBrain* MakeLeader(SafeWeakPtr<Player> player);

		//敵のチームを、指定した人数*指定したグループ数だけ作成する
		void MakeEnemyTeam(u32 team_count, u32 enem_per_team, SafeWeakPtr<Player> player);

	};
}

