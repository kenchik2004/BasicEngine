// AbstractEnemyBrain.cpp
#include "AbstractEnemyBrain.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"

#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyIdleState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyKnockBackState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyKnockFrontState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyDamageState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyDieState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyStandUpFrontState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyStandUpBackState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyCrowlingState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyEscapeState.h"

namespace NeonFade {

	/// @brief 敵AI基底クラスのコンストラクタ
	/// @param state_machine_ 対象のステートマシン
	/// @param player_ プレイヤーへの弱参照
	AbstractEnemyBrain::AbstractEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_)
		: state_machine(state_machine_), player(player_) {

		// ステートマシンのポインタが有効な場合のみオーナーとなる敵本体のポインタを取得して保持する
		if (state_machine_)
			body = state_machine_->enemy;

		if (!state_machine||!body)
			return;

		//待機状態
		auto idle_state = make_safe_unique<EnemyIdleState>(state_machine->enemy);
		state_machine->AddState("idle", std::move(idle_state));
		//ダメージ状態
		auto damage_state = make_safe_unique<EnemyDamageState>(state_machine->enemy);
		state_machine->AddState("damage", std::move(damage_state));
		//這いずり状態でのダメージ状態
		auto damage_crowling_state = make_safe_unique<EnemyCrowlingDamageState>(state_machine->enemy);
		state_machine->AddState("damage_crowling", std::move(damage_crowling_state));

		//ノックバック状態
		auto knock_back_state = make_safe_unique<EnemyKnockBackState>(state_machine->enemy);
		state_machine->AddState("knock_back", std::move(knock_back_state));
		//前方ノックバック状態
		auto knock_front_state = make_safe_unique<EnemyKnockFrontState>(state_machine->enemy);
		state_machine->AddState("knock_front", std::move(knock_front_state));
		//死亡状態
		auto die_state = make_safe_unique<EnemyDieState>(state_machine->enemy);
		state_machine->AddState("die", std::move(die_state));
		//前方起き上がり状態
		auto stand_up_front_state = make_safe_unique<EnemyStandUpFrontState>(state_machine->enemy);
		state_machine->AddState("stand_up_front", std::move(stand_up_front_state));
		//後方起き上がり状態
		auto stand_up_back_state = make_safe_unique<EnemyStandUpBackState>(state_machine->enemy);
		state_machine->AddState("stand_up_back", std::move(stand_up_back_state));
		//這いずり状態
		auto crowling_state = make_safe_unique<EnemyCrowlingState>(state_machine->enemy);
		state_machine->AddState("crowling", std::move(crowling_state));
		//離脱状態
		auto escape_state = make_safe_unique<EnemyEscapeState>(state_machine->enemy);
		state_machine->AddState("escape", std::move(escape_state));
	}
	/// @brief 敵AI基底クラスのデストラクタ
	AbstractEnemyBrain::~AbstractEnemyBrain()
	{}


	void AbstractEnemyBrain::ResetFrameParameters()
	{
		// フレーム依存のパラメータをリセットする
		is_knock_back = false;
		is_damaged = false;
	}

	std::string AbstractEnemyBrain::Think()
	{
		std::string result = "";
		//本来なら死亡判定が最優先だが、HPが0になっても殴り続ける方がプレイヤーにとって面白いと思うので、
		// ダメージとノックバックの判定を先にする

		// ノックバックを受けた場合はノックバック状態に遷移する
		if (is_knock_back) {
			// ノックバックの方向を敵の正面ベクトルとの内積で判定して、前方ノックバックか後方ノックバックかを決定する
			float dot = state_machine->enemy->transform->AxisZ().dot(knock_back_vec);
			if (dot > 0.0f)
				result = "knock_front";
			else
				result = "knock_back";

			// ノックバック状態に遷移したら、フレーム依存パラメータをリセットする
			ResetFrameParameters();

			return result;
		}
		// ダメージを受けた場合はダメージ状態に遷移する
		if (is_damaged) {
			result = "damage";
			if(is_crowling)
				result = "damage_crowling";
			// ダメージ状態に遷移したら、フレーム依存パラメータをリセットする
			ResetFrameParameters();
			return result;
		}

		//プレイヤーの気が済んだら殺してやってもいい
		//HPが0になる or 地面を貫通したら死ぬ
		if (hp == 0 || state_machine->enemy->transform->position.y < -50.0f) {
			result = "die";

			// 死亡状態に遷移したら、フレーム依存パラメータをリセットする
			ResetFrameParameters();
			return result;
		}

		// 弱った状態の敵は逃げる
		if (is_weakened) {
			result = "escape";

			// 逃げる状態に遷移したら、フレーム依存パラメータをリセットする
			ResetFrameParameters();
			return result;
		}


		// それ以外は通常の思考処理を行う
		// Think関数は派生クラスでオーバーライドされることを想定しているため、ここでは空文字列を返す
		return result;
	}


	void AbstractEnemyBrain::Damage(u32 damage, bool ignore_i_frame) {

		// 無敵時間を無視しないなら、無敵時間中はダメージを受けない
		if (!ignore_i_frame && i_frame_timer > 0.0f)
			return;

		// ダメージを受けた瞬間のフラグを立てる
		is_damaged = true;
		// ダメージが現在のHPを超える場合は死亡処理を行う
		if (damage > hp)
		{
			Die();
			return;
		}
		// ダメージを受ける
		hp -= damage;

		// HPが弱った状態の閾値以下になった場合は、弱った状態に遷移する
		if (hp <= WEAKED_HP_THRESHOLD) {

			//自身を弱った敵として登録する
			Enemy::RegisterWeakenedEnemy(state_machine->enemy);
			is_weakened = true;
		}
		// 無敵時間をリセットする
		i_frame_timer = I_FRAME;
	}

	void AbstractEnemyBrain::Die()
	{
		// HPを0に設定して死亡フラグを立てる
		hp = 0;
		if (is_weakened)
			Enemy::UnregisterWeakenedEnemy(state_machine->enemy);
		is_weakened = false;
	}

	void AbstractEnemyBrain::KnockBack(Vector3 knock_back_vec)
	{
		// ノックバックを受けた瞬間のフラグを立てる
		is_knock_back = true;
		// ノックバックの方向と力のベクトルを保存する
		this->knock_back_vec = knock_back_vec;
	}

}