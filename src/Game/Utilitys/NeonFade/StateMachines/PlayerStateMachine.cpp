// PlayerStateMachine.cpp
//---------------------------------------------------------------------------
//! @file   PlayerStateMachine.cpp
//! @brief  PlayerStateMachineの実装。プレイヤーのステートマシン処理を行う
//---------------------------------------------------------------------------
#include "precompile.h"
#include "PlayerStateMachine.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerSpawnState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerMoveState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerIdleState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerJumpState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerFallState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerCombatComboState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerCombatComboState2.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerCombatComboState3.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerClimbingState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerDodgeState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerDamageState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerSmashChargeState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerSuperHeroLandingState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerSmashMainState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerAttack1State.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerAttack2State.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerAttack3State.h"

namespace NeonFade
{

	/// @brief プレイヤーステートマシンのコンストラクタ
	/// @param owner_ 所有者となるプレイヤーオブジェクトのポインタ
	PlayerStateMachine::PlayerStateMachine(Player* owner_) :
		IStateMachine(static_cast<GameObject*>(owner_))
	{
		// 前フレームの攻撃インデックスを初期化する
		attack_index_prev = 0;
		// プレイヤーオブジェクトのポインタを保持する
		player = owner_;
		// 移動ステートを生成する
		auto move_state = make_safe_unique<PlayerMoveState>(player);
		// ジャンプ状態への遷移条件を定義する
		std::function<bool()> move_to_jump = [this]() {
			return is_jumping;
			};
		// 待機状態への遷移条件を定義する
		std::function<bool()> move_to_idle = [this]() {

			return move_input.magnitudeSquared() < 0.01f;
			};
		// 落下状態への遷移条件を定義する
		std::function <bool()> move_to_fall = [this]() {
			return is_falling;
			};
		// 壁登り状態への遷移条件を定義する
		std::function <bool()> move_to_climb = [this]() {
			return can_climb && move_input.magnitudeSquared() > FLT_EPSILON;
			};
		// 攻撃状態への遷移条件を定義する
		std::function<bool()> move_to_attack = [this]() {
			return is_attacking;
			};
		// 回避状態への遷移条件を定義する
		std::function<bool()> move_to_dodge = [this]() {
			return is_dodging;
			};
		// ダメージ状態への遷移条件を定義する
		std::function<bool()> move_to_damage = [this]() {
			return is_damaged;
			};
		// 定義した各遷移条件を移動ステートに登録する
		move_state->RegisterChangeRequest("dodge", move_to_dodge, 0);
		move_state->RegisterChangeRequest("jump", move_to_jump, 0);
		move_state->RegisterChangeRequest("climb", move_to_climb, 0);
		move_state->RegisterChangeRequest("damage", move_to_damage, 1);
		move_state->RegisterChangeRequest("idle", move_to_idle, 1);
		move_state->RegisterChangeRequest("fall", move_to_fall, 1);
		move_state->RegisterChangeRequest("attack", move_to_attack, 1);
		// 移動ステートをステートマシンに追加する
		AddState("move", std::move(move_state));

		// ジャンプステートを生成する
		auto jump_state = make_safe_unique<PlayerJumpState>(player);

		// 落下状態への遷移条件を定義する
		std::function<bool()> jump_to_fall = [this, state = jump_state.get()]() {
			return  is_falling && state->jump_timer >= state->JUMP_TIME;
			};
		// 待機状態への遷移条件を定義する
		std::function<bool()> jump_to_idle = [this, state = jump_state.get()]() {
			return is_landed && state->jump_timer >= state->JUMP_TIME;
			};
		// 壁登り状態への遷移条件を定義する
		std::function<bool()> jump_to_climb = [this]() {
			return can_climb && move_input.magnitudeSquared() > FLT_EPSILON;
			};
		// 攻撃状態への遷移条件を定義する
		std::function<bool()> jump_to_jump_attack = [this]() {
			return is_attacking;
			};
		// 回避状態への遷移条件を定義する
		std::function<bool()> jump_to_dodge = [this]() {
			return is_dodging;
			};
		// ダメージ状態への遷移条件を定義する
		std::function<bool()> jump_to_damage = [this]() {
			return is_damaged;
			};
		// 定義した各遷移条件をジャンプステートに登録する
		jump_state->RegisterChangeRequest("damage", jump_to_damage, 1);
		jump_state->RegisterChangeRequest("fall", jump_to_fall, 1);
		jump_state->RegisterChangeRequest("idle", jump_to_idle, 1);
		jump_state->RegisterChangeRequest("dodge", jump_to_dodge, 0);
		jump_state->RegisterChangeRequest("climb", jump_to_climb, 0);
		jump_state->RegisterChangeRequest("attack", jump_to_jump_attack, 0);
		// ジャンプステートをステートマシンに追加する
		AddState("jump", std::move(jump_state));


		// 落下ステートを生成する
		auto fall_state = make_safe_unique<PlayerFallState>(player);
		// 待機状態への遷移条件を定義する
		std::function<bool()> fall_to_idle = [this]() {
			//return player->rb->velocity.y >= -5.0f;
			return is_landed;
			};
		// 壁登り状態への遷移条件を定義する
		std::function<bool()> fall_to_climb = [this]() {
			return can_climb && move_input.magnitudeSquared() > FLT_EPSILON;
			};
		// 攻撃状態への遷移条件を定義する
		std::function<bool()> fall_to_jump_attack = [this]() {return is_attacking; };
		// ダメージ状態への遷移条件を定義する
		std::function<bool()> fall_to_damage = [this]() {
			return is_damaged;
			};
		// 定義した各遷移条件を落下ステートに登録する
		fall_state->RegisterChangeRequest("damage", fall_to_damage, 1);
		fall_state->RegisterChangeRequest("idle", fall_to_idle, 1);
		fall_state->RegisterChangeRequest("climb", fall_to_climb, 0);
		fall_state->RegisterChangeRequest("attack", fall_to_jump_attack, 0);
		// 落下ステートをステートマシンに追加する
		AddState("fall", std::move(fall_state));

		// 壁登りステートを生成する
		auto climb_state = make_safe_unique<PlayerClimbingState>(player);

		// 壁登りステートをステートマシンに追加する
		AddState("climb", std::move(climb_state));

		// 待機ステートを生成する
		auto idle_state = make_safe_unique<PlayerIdleState>(player);
		// 移動状態への遷移条件を定義する
		std::function<bool()> idle_to_move = [this]() {
			return move_input.magnitudeSquared() >= 0.01f;
			};
		// ジャンプ状態への遷移条件を定義する
		std::function<bool()> idle_to_jump = [this]() {
			return is_jumping;
			};
		// 落下状態への遷移条件を定義する
		std::function<bool()> idle_to_fall = [this]() {
			//return  player->rb->velocity.y < -5.0f;
			return is_falling;
			};
		// 攻撃状態への遷移条件を定義する
		std::function<bool()> idle_to_attack = [this]() {
			return is_attacking;
			};
		// 回避状態への遷移条件を定義する
		std::function<bool()> idle_to_dodge = [this]() {
			return is_dodging;
			};
		// ダメージ状態への遷移条件を定義する
		std::function<bool()> idle_to_damage = [this]() {
			return is_damaged;
			};

		// 定義した各遷移条件を待機ステートに登録する
		idle_state->RegisterChangeRequest("dodge", idle_to_dodge, 0);
		idle_state->RegisterChangeRequest("jump", idle_to_jump, 0);
		idle_state->RegisterChangeRequest("fall", idle_to_fall, 0);
		idle_state->RegisterChangeRequest("damage", idle_to_damage, 1);
		idle_state->RegisterChangeRequest("move", idle_to_move, 1);
		idle_state->RegisterChangeRequest("attack", idle_to_attack, 1);
		// 待機ステートをステートマシンに追加する
		AddState("idle", std::move(idle_state));



		// 攻撃1ステートを生成し名前を設定する
		auto attack1_state = make_safe_unique<PlayerAttack1State>(player);
		attack1_state->SetName("attack1");
		// 攻撃1ステートを攻撃ステート配列に格納する
		//AddState("attack", std::move(attack1_state));
		attack_states_vec[3] = std::move(attack1_state);
		// 攻撃3ステートを生成し名前を設定する
		//AddState("attack", std::move(attack2_state));
		auto attack3_state = make_safe_unique<PlayerAttack3State>(player);
		attack3_state->SetName("attack3");
		// 攻撃3ステートを攻撃ステート配列に格納する
		//AddState("attack", std::move(attack3_state));
		attack_states_vec[2] = std::move(attack3_state);


		// スマッシュチャージステートを生成し名前を設定する
		auto smash_charge_state = make_safe_unique<PlayerSmashChargeState>(player);
		smash_charge_state->SetName("smash_charge");

		// スマッシュチャージステートを攻撃ステート配列に格納する
		attack_states_vec[1] = std::move(smash_charge_state);

		// スーパーヒーロー着地ステートを生成し登録する
		auto super_hero_landing_state = make_safe_unique<PlayerSuperHeroLandingState>(player);
		AddState("super_hero_landing", std::move(super_hero_landing_state));

		// スマッシュメインステートを生成し登録する
		auto smash_main_state = make_safe_unique<PlayerSmashMainState>(player);
		AddState("smash_attack", std::move(smash_main_state));
		// コンボ攻撃関連のステートを設定するブロック
		{
			// コンボ攻撃1段目のステートを生成する
			auto attack_state = make_safe_unique<PlayerCombatComboState>(player);
			// ジャンプ状態への遷移条件を定義する
			std::function<bool()> attack_to_jump = [this]() {
				return is_jumping;
				};
			// 回避状態への遷移条件を定義する
			std::function<bool()> attack_to_dodge = [this]() {
				return is_dodging;
				};
			// ダメージ状態への遷移条件を定義する
			std::function<bool()> attack_to_damage = [this]() {
				return is_damaged;
				};
			// 各遷移条件をコンボ攻撃1段目に登録する
			attack_state->RegisterChangeRequest("dodge", attack_to_dodge, 0);
			attack_state->RegisterChangeRequest("jump", attack_to_jump, 0);
			attack_state->RegisterChangeRequest("damage", attack_to_damage, 1);
			// ステート名を変更して配列に格納する
			attack_state->SetName("default_attack");
			attack_states_vec[0] = std::move(attack_state);
			// コンボ攻撃2段目のステートを生成する
			auto attack2_state = make_safe_unique<PlayerCombatComboState2>(player);
			// ジャンプ状態への遷移条件を定義する
			std::function<bool()> attack2_to_jump = [this]() {
				return is_jumping;
				};
			// 回避状態への遷移条件を定義する
			std::function<bool()> attack2_to_dodge = [this]() {
				return is_dodging;
				};
			// ダメージ状態への遷移条件を定義する
			std::function<bool()> attack2_to_damage = [this]() {
				return is_damaged;
				};
			// 各遷移条件をコンボ攻撃2段目に登録しステートマシンに追加する
			attack2_state->RegisterChangeRequest("dodge", attack2_to_dodge, 0);
			attack2_state->RegisterChangeRequest("jump", attack2_to_jump, 0);
			attack2_state->RegisterChangeRequest("damage", attack2_to_damage, 1);
			AddState("attack2", std::move(attack2_state));
			// コンボ攻撃3段目のステートを生成する
			auto attack3_state = make_safe_unique<PlayerCombatComboState3>(player);
			// ジャンプ状態への遷移条件を定義する
			std::function<bool()> attack3_to_jump = [this]() {
				return is_jumping;
				};
			// 回避状態への遷移条件を定義する
			std::function<bool()> attack3_to_dodge = [this]() {
				return is_dodging;
				};
			// ダメージ状態への遷移条件を定義する
			std::function<bool()> attack3_to_damage = [this]() {
				return is_damaged;
				};
			// 各遷移条件をコンボ攻撃3段目に登録しステートマシンに追加する
			attack3_state->RegisterChangeRequest("dodge", attack3_to_dodge, 0);
			attack3_state->RegisterChangeRequest("jump", attack3_to_jump, 0);
			attack3_state->RegisterChangeRequest("damage", attack3_to_damage, 1);
			AddState("attack3", std::move(attack3_state));

		}






		// 回避ステートを生成する
		auto dodge_state = make_safe_unique<PlayerDodgeState>(player);
		// ダメージ状態への遷移条件を定義する
		std::function<bool()> dodge_to_damage = [this, state = dodge_state.get()]() {
			return is_damaged && state->dodge_timer >= state->I_FRAME_TIME;
			};
		// 回避ステートをステートマシンに追加する
		AddState("dodge", std::move(dodge_state));

		// ダメージステートを生成しステートマシンに追加する
		auto damage_state = make_safe_unique<PlayerDamageState>(player);
		AddState("damage", std::move(damage_state));

		// スポーンステートを生成しステートマシンに追加する
		auto spawn_state = make_safe_unique<PlayerSpawnState>(player);
		AddState("spawn", std::move(spawn_state));
		// 初期状態をスポーンに設定する
		ChangeState("spawn");
	}
	/// @brief デストラクタ
	PlayerStateMachine::~PlayerStateMachine()
	{}
	/// @brief デバッグ用の描画処理を行う
	void PlayerStateMachine::DebugDraw()
	{
		__super::DebugDraw();
#if 0
		if (current_state)
			current_state->DebugDraw();
#endif
	}
	/// @brief 毎フレームの更新処理を行う
	/// @param dt 経過時間
	void PlayerStateMachine::Update(float dt)
	{
		// 攻撃中でない場合は入力に応じて使用する攻撃の種類を決定する
		if (GetCurrentStateName() != "attack") {
			u32 attack_index = 0;
			// ボタン入力に応じてattack_indexを設定

			// ボタン1が押された場合はインデックス1に設定する
			if (Input::GetPadButtonDown(0, PadButton::Button1)||Input::GetKey(KeyCode::Alpha1))
			{

				attack_index = 1;
			}

			// ボタン2が押された場合はインデックス0に設定する
			if (Input::GetPadButtonDown(0, PadButton::Button2)||Input::GetKey(KeyCode::Alpha2))
			{
				attack_index = 0;
			}
			// ボタン3が押された場合はインデックス2に設定する
			if (Input::GetPadButtonDown(0, PadButton::Button3)||Input::GetKey(KeyCode::Alpha3))
			{
				attack_index = 2;
			}
			// ボタン4が押された場合はインデックス3に設定する
			if (Input::GetPadButtonDown(0, PadButton::Button4)||Input::GetKey(KeyCode::Alpha4))
			{
				attack_index = 3;
			}
			// 空中状態であれば強制的にスマッシュチャージ用のインデックスを設定する
			if (GetCurrentStateName() == "jump" || GetCurrentStateName() == "fall") {
				attack_index = 1; //空中攻撃はsmash_chargeに固定
			}
#ifdef _DEBUG
#if 0
			int i = 0;
			printfDx("attack_index:%d\n", attack_index);
			for (auto& state : attack_states_vec) {
				if (state) {
					printfDx("index:%d,name:%s\n", i, state->GetName().c_str());
				}
				else
				{
					printfDx("index:%d,name:null\n", i);
				}
				i++;
			}
#endif
#endif // _DEBUG
			//前フレームと違う攻撃が選択されたら状態を入れ替え

			//既にある攻撃状態を元のスロットに戻す
			if (states["attack"] && attack_index != attack_index_prev)
				attack_states_vec[attack_index_prev] = std::move(states["attack"]);
			//新しい攻撃状態をセット
			if (attack_index != attack_index_prev)
				states["attack"] = std::move(attack_states_vec[attack_index]);

			//上記の操作を経た後に、もし攻撃状態が空ならデフォルトの攻撃状態をセット
			if (!states["attack"])
				states["attack"] = std::move(attack_states_vec[0]);

			//現在フレームの攻撃インデックスを保存
			attack_index_prev = attack_index;
		}

		// 親クラスの更新処理を呼び出す
		__super::Update(dt);
	}
}