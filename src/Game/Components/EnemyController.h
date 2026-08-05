// EnemyController.h
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
		//! @return 初期化が成功した場合は0を返す
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
		//! @param hit_info 衝突情報
		void OnCollisionEnter(const HitInfo& hit_info);

		//! @brief 衝突継続イベント
		//! @param hit_info 衝突情報
		void OnCollisionStay(const HitInfo& hit_info);

		//! @brief 衝突終了イベント
		//! @param hit_info 衝突情報
		void OnCollisionExit(const HitInfo& hit_info);

		//! @brief トリガー開始イベント
		//! @param hit_info 衝突情報
		void OnTriggerEnter(const HitInfo& hit_info);

		//! @brief トリガー継続イベント
		//! @param hit_info 衝突情報
		void OnTriggerStay(const HitInfo& hit_info);

		//! @brief トリガー終了イベント
		//! @param hit_info 衝突情報
		void OnTriggerExit(const HitInfo& hit_info);

		//! @brief ダメージを与える
		//! @param damage ダメージ量
		//! @param ignore_i_frame 無敵フレームを無視するかどうか
		void Damage(int damage, bool ignore_i_frame = false);

		//! @brief ダウン処理
		//! @param vec ダウンさせる方向と力のベクトル
		void Down(Vector3 vec);

		//! @brief 死亡判定を返す
		//! @return 死亡していればtrueを返す
		bool IsDead();

		//! @brief 現在のHPを返す
		//! @return 現在のHP
		u32 GetHp();

		//! @brief ステートマシンを取得する
		//! @return ステートマシンへのポインタ
		EnemyStateMachine* GetStateMachine() { return state_machine.get(); }

		//! @brief 脳（AI）を取得する
		//! @return 脳（AI）へのポインタ
		AbstractEnemyBrain* GetBrain() { return brain.get(); }

		//! @brief プレイヤーをセットする
		//! @param player_ プレイヤーへの弱参照
		void SetPlayer(PlayerWP player_) { player = player_; }

		//! @brief プレイヤーへの弱参照を取得する
		//! @return プレイヤーへの弱参照
		GameObjectWP GetPlayer();

		//! @brief 脳（AI）をセットする
		//! @param new_brain 新しい脳（AI）の所有権を持つユニークポインタ
		void SetBrain(AbstractEnemyBrainUP new_brain);

	private:
		EnemyStateMachineUP state_machine; //!< 敵のステートマシン
		AbstractEnemyBrainUP brain = nullptr; //!< 敵のAI脳
		PlayerWP player = nullptr; //!< プレイヤーへの弱参照
		bool is_leader = false; //!< リーダーフラグ
	};

}