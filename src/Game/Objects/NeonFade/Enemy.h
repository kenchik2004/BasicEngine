//---------------------------------------------------------------------------
//! @file   Enemy.h
//! @brief  敵ゲームオブジェクト
//---------------------------------------------------------------------------
#pragma once

#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"
namespace NeonFade {

	USING_PTR(EnemyController);
	USING_PTR(Player);
	USING_PTR(EnemyTeam);
	//---------------------------------------------------------------------
	//! @class Enemy
	//! @brief 敵キャラクターのゲームオブジェクト
	//---------------------------------------------------------------------
	class Enemy :
		public GameObject
	{
	public:
		USING_SUPER(Enemy);
		//! @brief 初期化処理
		int Init() override;
		//! @brief 更新処理
		void Update() override;
		//! @brief ダメージを与える
		void Damage(int damage, bool ignore_i_frame = false);
		//! @brief ダウン処理
		void Down(Vector3 vec);
		//! @brief 死亡判定を返す
		bool IsDead();
		//! @brief 現在のHPを返す
		u32 GetHp();

		//! @brief トリガー開始イベント
		void OnTriggerEnter(const HitInfo& hit_info);

		ModelRendererWP model; //!< モデルレンダラー
		AnimatorWP animator; //!< アニメーター
		RigidBodyWP rb; //!< 物理ボディ
		EnemyControllerWP enem_controller; //!< 敵コントローラー
		ColliderWP col; //!< コライダー
		static inline Material* death_material = nullptr; //!< 死亡時マテリアル
	};
	class BasicEnemyBrain;
	class TeamMemberEnemyBrain;
	class LeaderEnemyBrain;
	//---------------------------------------------------------------------
	//! @class EnemyFactory
	//! @brief 敵オブジェクトを生成するファクトリークラス
	//---------------------------------------------------------------------
	class EnemyFactory {
	private:
		static inline u64 counter = 0; //!< 生成カウンター
		Vector3 spawn_pos = { 0,0,0 }; //!< スポーン位置
		float spawn_radius = 5.0f; //!< スポーン半径

	public:
		//! @brief スポーン位置を設定する
		void SetSpawnPosition(const Vector3& pos) { spawn_pos = pos; }
		//! @brief スポーン半径を設定する
		void SetSpawnRadius(float radius) { spawn_radius = radius; }
		//不完全初期化状態の敵を作成してコントローラーを返す
		//脳が作成・セットされてないので、
		//プレイヤーポインタとステートマシンで初期化する必要がある
		//なんかモダンで推奨されてるらしいからstd::tupleで構造化束縛してみよう
		//! @brief 未完全初期化の敵を生成してコントローラーとステートマシンを返す
		std::tuple<EnemyController*, EnemyStateMachine*> MakeAbstractEnemy();

		//シンプルな単体行動を行う脳をセットされた敵を作成して、脳のポインタを返す
		//! @brief 単体行動用の脳を持つ敵を生成する
		BasicEnemyBrain* MakeBasicEnemy(SafeWeakPtr<Player> player);

		//リーダーに従い行動する脳がセットされた敵を作成して脳のポインタを返す
		//! @brief リーダー追従型の脳を持つ敵を生成する
		TeamMemberEnemyBrain* MakeTeamMateEnemy(LeaderEnemyBrain* leader_ptr, SafeWeakPtr<Player> player, EnemyTeam* my_team = nullptr);

		//チームの統率を取る脳をセットされた敵を作成して脳のポインタを返す
		//! @brief チームリーダーの脳を持つ敵を生成する
		LeaderEnemyBrain* MakeLeader(SafeWeakPtr<Player> player, EnemyTeam* my_team);

		//敵のチームを、指定した人数*指定したグループ数だけ作成する
		//! @brief 指定した人数・グループ数で敵チームを生成する
		std::vector<EnemyTeamUP> MakeEnemyTeam(u32 team_count, u32 enem_per_team, SafeWeakPtr<Player> player);

	};
}
