//---------------------------------------------------------------------------
//! @file   EnemyController.h
//! @brief  敵コントローラーコンポーネント（敵の行動・AI制御）
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/EnemyBrain/AbstractEnemyBrain.h"
namespace NeonFade {
	USING_PTR(EnemyStateMachine);
	USING_PTR(Player);
	USING_PTR(AbstractEnemyBrain);
	//---------------------------------------------------------------------
	//! @class EnemyController
	//! @brief 敵の行動・AI制御を担うコンポーネント
	//---------------------------------------------------------------------
	class EnemyController :
		public Component
	{
	public:
		USING_SUPER(EnemyController);
		//! @brief 初期化処理
		int Init() override;
		//! @brief 更新処理
		void Update() override;
		//! @brief 終了処理
		void Exit() override;
		//! @brief デバッグ描画
		void DebugDraw() override;
		//! @brief デバッグ描画（遅延）
		void LateDebugDraw() override;

		//! @brief 衝突開始イベント
		void OnCollisionEnter(const HitInfo& hit_info);
		//! @brief 衝突継続イベント
		void OnCollisionStay(const HitInfo& hit_info);
		//! @brief 衝突終了イベント
		void OnCollisionExit(const HitInfo& hit_info);
		//! @brief トリガー開始イベント
		void OnTriggerEnter(const HitInfo& hit_info);
		//! @brief トリガー継続イベント
		void OnTriggerStay(const HitInfo& hit_info);
		//! @brief トリガー終了イベント
		void OnTriggerExit(const HitInfo& hit_info);

		//! @brief ダメージを与える
		void Damage(int damage, bool ignore_i_frame = false);
		//! @brief ダウン処理
		void Down(Vector3 vec);
		//! @brief 死亡判定を返す
		bool IsDead();
		//! @brief 現在のHPを返す
		u32 GetHp();
		//! @brief ステートマシンを取得する
		EnemyStateMachine* GetStateMachine() { return state_machine.get(); }
		//! @brief 脳（AI）を取得する
		AbstractEnemyBrain* GetBrain() { return brain.get(); }
		//! @brief プレイヤーをセットする
		void SetPlayer(PlayerWP player_) { player = player_; }
		//! @brief 脳（AI）をセットする
		void SetBrain(AbstractEnemyBrainUP new_brain);
	private:
		EnemyStateMachineUP state_machine; //!< 敵のステートマシン
		AbstractEnemyBrainUP brain = nullptr; //!< 敵のAI脳
		PlayerWP player = nullptr; //!< プレイヤーへの弱参照
		bool is_leader = false; //!< リーダーフラグ
	};

}
