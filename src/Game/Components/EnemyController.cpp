// EnemyController.cpp
//---------------------------------------------------------------------------
//! @file   EnemyController.cpp
//! @brief  EnemyControllerコンポーネントの実装。敵キャラクターの行動制御を管理する
//---------------------------------------------------------------------------
#include "precompile.h"
#include "EnemyController.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/Player.h"

#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/BasicEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/LeaderEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/TeamMemberEnemyBrain.h"

namespace NeonFade {


	int EnemyController::Init()
	{
		// このコンポーネントを持つオーナーをEnemy型にキャストしてnullセーフなshared_ptrとして取得する
		SafeSharedPtr<Enemy> owner_enemy = SafeStaticCast<Enemy>(owner.lock());
		// 取得したEnemyオブジェクトを基に敵専用のステートマシンを構築する
		state_machine = make_safe_unique<EnemyStateMachine>(owner_enemy.get());

		// 親クラスの初期化処理を呼び出す
		return Super::Init();
	}

	void EnemyController::Update()
	{
		// AIの思考を司る脳と状態を管理するステートマシンが存在しない場合は処理を中断する
		if (!brain || !state_machine)
			return;

		// 脳の思考処理を実行し次に取るべき最適な行動を取得する
		std::string best_action = brain->Think();
		// ステートマシンから現在の状態を取得する
		IState* current_state = state_machine->GetCurrentState();
		// 現在の状態が存在しないか新しい行動への遷移が許可されているか判定する
		if (!current_state || current_state->CanTransitTo(best_action)) {
			// 条件を満たしていればステートマシンに新しい行動を指示して状態を切り替える
			state_machine->ChangeState(best_action);
		}

		// 実際の時間経過に基づいてステートマシンの状態を更新する
		state_machine->Update(Time::UnscaledDeltaTime());
	}

	void EnemyController::Exit()
	{}

	void EnemyController::DebugDraw()
	{
		// ステートマシンが存在する場合のみデバッグ用の描画処理を呼び出す
		if (state_machine)
			state_machine->DebugDraw();
	}

	void EnemyController::LateDebugDraw()
	{
		// 脳またはステートマシンが存在しない場合は描画処理を中断する
		if (!brain || !state_machine)
			return;

		// ステートマシンと脳の遅延デバッグ描画を実行する
		state_machine->DebugDraw();
		brain->DebugDraw();
	}

	void EnemyController::Damage(int damage, bool ignore_i_frame)
	{
		// 脳が存在しない場合はダメージ処理を行わない
		if (!brain)
			return;

		// 脳にダメージ量と無敵時間を無視するかどうかのフラグを渡して処理を委譲する
		brain->Damage(damage, ignore_i_frame);
	}

	void EnemyController::Down(Vector3 vec)
	{
		// 脳が存在しない場合はダウン処理を行わない
		if (!brain)
			return;
		// 脳に対してノックバック方向のベクトルを渡してダウン状態を処理する
		brain->KnockBack(vec);
	}
	bool EnemyController::IsDead() {
		// 脳が存在しない場合は死んでいるとみなす
		if (!brain)
			return true;

		// 脳から現在の生存状態を取得して返す
		return brain->IsDead();
	}
	u32 EnemyController::GetHp() {
		// 脳が存在しない場合はHPを0として返す
		if (!brain)
			return 0;

		// 脳が管理している現在のHPを取得して返す
		return brain->GetHp();
	}

	GameObjectWP EnemyController::GetPlayer()
	{
		// 保持しているプレイヤーへの弱参照を返す
		return player;
	}

	void EnemyController::SetBrain(AbstractEnemyBrainUP new_brain) {
		// 新しい脳の所有権を移動してセットする
		brain = std::move(new_brain);
		// セットした直後に一度思考処理を走らせる
		brain->Think();

		// ステートマシンが存在する場合は新しい脳のポインタを渡して連携させる
		if (state_machine)
			state_machine->SetBrain(brain.get());
	}

	void EnemyController::OnTriggerEnter(const HitInfo& hit_info)
	{
		// ステートマシンが存在しない場合は衝突イベントを処理しない
		if (!state_machine)
			return;
		// 領域への侵入イベントをステートマシンに伝達する
		state_machine->OnTriggerEnter(hit_info);
	}

}