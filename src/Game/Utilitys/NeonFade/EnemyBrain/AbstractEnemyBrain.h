//---------------------------------------------------------------------------
//! @file   AbstractEnemyBrain.h
//! @brief  敵AI思考抽象クラスの定義
//---------------------------------------------------------------------------
#pragma once

namespace NeonFade {
	// 各種前方宣言
	class EnemyStateMachine;
	class Enemy;
	USING_PTR(Player);


	//! @brief 全ての敵AIの共通機能を提供する基底クラス
	//! @brief このクラスは純粋仮想関数を持つため、直接インスタンス化することはできない
	class AbstractEnemyBrain
	{
	public:
		//! @brief 敵AI基底クラスのコンストラクタ
		//! @param state_machine_ 状態遷移を管理するステートマシン
		//! @param player_ プレイヤーの情報を参照するためのポインタ
		AbstractEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_);

		//! @brief 敵AI基底クラスのデストラクタ
		virtual ~AbstractEnemyBrain();

		//! @brief フレーム依存のパラメータをリセットする
		//! @brief この関数はThink関数内でのリターン前に呼び出して、ダメージやノックバックの状態をリセットするために使用する
		void ResetFrameParameters();

		//! @brief 思考処理を実行する純粋仮想関数
		//! @return次に遷移すべきステートの文字列
		virtual std::string Think();

		/// @brief ダメージ処理を行う
		/// @param damage 受けるダメージ量
		/// @param ignore_i_frame 無敵フレームを無視するかどうかのフラグ
		virtual void Damage(u32 damage = 0, bool ignore_i_frame = false);

		/// @brief 死亡処理を行う
		virtual void Die();

		/// @brief ノックバック処理を行う
		/// @param knock_back_vec ノックバックさせる方向と力のベクトル
		virtual void KnockBack(Vector3 knock_back_vec);

		/// @brief デバッグ用の描画を行う
		virtual void DebugDraw() = 0;

		/// @brief 敵が弱っている状態かどうかを判定する
		/// @return 弱っていればtrueを返す
		bool IsWeakened() const { return is_weakened; }

		//! @brief 現在のHPを取得する
		//! @return 現在のHP
		u32 GetHp() { return hp; }

		//! @brief 死亡しているか判定する
		//! @return HPが0であればtrueを返す
		bool IsDead() { return hp == 0; }

		//! @brief 保持しているステートマシンを取得する
		//! @return ステートマシンのポインタ
		EnemyStateMachine* GetMachine() { return state_machine; }

		//! @brief 自身を保持している敵本体を取得する
		//! @return 敵本体のポインタ
		Enemy* GetOwnerBody() { return body; }

		//! @brief ノックバック状態か判定する
		//! @return ノックバック中であればtrueを返す
		bool IsKnockBack() { return is_knock_back; }

		//! @brief ダメージを受けた状態か判定する
		//! @return ダメージ状態であればtrueを返す
		bool IsDamaged() { return is_damaged; }

		//! @brief 敵が這いずり状態か判定する
		//! @return 這いずり状態であればtrueを返す
		bool IsCrawling() { return is_crowling; }	

		//! @brief 敵が這いずり状態か設定する
		//! @param crawling 這いずり状態にするかどうかのフラグ
		void SetIsCrawling(bool crawling) { is_crowling = crawling; }

		//! @brief ノックバックのベクトルを取得する
		//! @return ノックバックベクトルの参照
		const Vector3& GetKnockBackVec() { return knock_back_vec; }

		//! @brief 参照しているプレイヤーのポインタを取得する
		//! @return プレイヤーのポインタ
		Player* GetPlayer() { return player.lock().get(); }

	protected:
		EnemyStateMachine* state_machine; // ステートマシンへのポインタ
		PlayerWP player; // プレイヤーへの弱参照
		Enemy* body; // 敵本体へのポインタ
		static constexpr u32 MAX_HP = 100; // 敵の最大HP
		u32 hp = MAX_HP; // 敵の現在のHP

		float i_frame_timer = 0.0f; // 無敵フレームの残り時間タイマー
		bool is_damaged = false; // ダメージを受けた瞬間の判定フラグ
		bool is_knock_back = false; // ノックバックを受けた瞬間の判定フラグ
		Vector3 knock_back_vec = { 0,0,0 }; // 適用されるノックバックベクトル
		static constexpr float I_FRAME = 0.1f; // ダメージ後の無敵時間

		static constexpr u32 WEAKED_HP_THRESHOLD = 30;	//<! HPがこの値以下のとき弱っている状態とみなす閾値
		bool is_weakened = false;						//<! 敵が弱っている状態かどうかを示すフラグ

		bool is_crowling = false; //<! 敵が這いずり状態かどうかを示すフラグ

	};
}