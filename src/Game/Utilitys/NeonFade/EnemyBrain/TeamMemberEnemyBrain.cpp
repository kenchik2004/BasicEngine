//---------------------------------------------------------------------------
//! @file   TeamMemberEnemyBrain.cpp
//! @brief  TeamMemberEnemyBrainの実装。チームメンバー敵のAI思考ロジックを実装する
//---------------------------------------------------------------------------
#include "TeamMemberEnemyBrain.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Components/PlayerController.h"
#include "Game/Components/EnemyController.h"
#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/LeaderEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/BasicEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/EnemyTeam.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyDropKickAttackEntryState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyBecomeToBasicState.h"

namespace NeonFade {

	TeamMemberEnemyBrain::TeamMemberEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_, EnemyTeam* team)
		:AbstractEnemyBrain(state_machine_, player_)
	{

		//ステートマシンとチームのポインタを保持する
		machine = state_machine_;
		my_team = team;

		// HPを最大値で初期化する
		hp = MAX_HP;


		// もしステートマシンが存在する場合は、チームメンバー専用のステートを登録する
		{
			// チームが存在しない場合に、基礎的な敵Brainに切り替えるステートを登録する
			auto become_leader_state = make_safe_unique<EnemyBecomeToLeaderState>(machine->enemy);
			machine->AddState("become_leader", std::move(become_leader_state));

			//ドロップキック攻撃ステートを登録する
			auto atk_1_state = make_safe_unique<EnemyDropKickAttackEntryState>(state_machine->enemy);
			state_machine->AddState("drop_kick", std::move(atk_1_state));
		}

		machine->ChangeState("idle");

	}

	TeamMemberEnemyBrain::~TeamMemberEnemyBrain()
	{

		//自身をチームから削除する
		if (my_team)
			my_team->SubMember(this);
	}

	std::string TeamMemberEnemyBrain::Think()
	{
		if (i_frame_timer > 0.0f) {
			i_frame_timer -= Time::DeltaTime();
		}

		// もし親クラスのThinkで遷移先が決まっていれば、その遷移先を返す
		// 基礎的なダメージ、死亡、ノックバックの判定は親クラスで行うので、ここでは親クラスのThinkを呼び出す
		std::string result = __super::Think();

		if (result != "")
			return result;
#if 1 
		if (Input::GetKeyDown(KeyCode::J) && my_team)
			my_team->SubMember(this);
#endif

		// もしチームが存在しない場合は、基本的な敵Brainをセットする
		// ここでそのままセットしてしまうと、スタックが巻き戻ってきたときに use after free が発生するので、
		// 遷移先のステートを返すことで、ステートマシン側でBrainを切り替えるようにする
		if (Input::GetKeyDown(KeyCode::M) && Random::Int(0, 10) == 0) {
			result = "become_leader";
			return result;
		}
		if (go_to_attack) {
			result = "drop_kick";
			go_to_attack = false;
		}


		return result;
	}

	void TeamMemberEnemyBrain::DebugDraw()
	{}

	void TeamMemberEnemyBrain::ResetTeamRef()
	{
		my_team = nullptr;
	}

	void TeamMemberEnemyBrain::SetTeamRef(EnemyTeam* team)
	{
		my_team = team;
	}


}






//どのように爽快感を感じるか
//敵同士が連携し合うので、まとめて倒すのが難しい
//敵の連携を崩して、隙が出来たところにで派手な攻撃を決めてまとめて倒すことにより爽快感を感じさせる

//どの敵を率先して倒すか、駆け引きの末に一気に倒すのが爽快感を感じる要素になる
//例えば、敵が弱った場合は他の敵が庇いに来るので、集まったところに一気に攻撃を決める
//例えば、敵がチームに所属している時、リーダーは攻撃指示を出すので、部下が攻撃に向かうタイミングを見計らって、リーダーを倒すことで部下を無力化する

//プレイヤーに提供したい体験:
//サイバーパンク×ネオンの世界観の中で、人間のような知能を持ったAIと駆け引きをして戦略的に戦う体験

//ゲームの目的:
//プレイヤーは、研究中に暴走したAIを倒すために、サイバーパンクな都市を舞台に戦う。プレイヤーはAIの行動パターンを読み取り、戦略的に戦うことで、AIの暴走から世界を防衛することが目的となる。

//世界観:
//攻殻機動隊のように、技術の進歩によって人間のような知能を持ったAIが生まれ、人間に管理されている世界


//TODO:
//現状、敵の数が増えると全部撃破することが難しくなるので、以下のような行動を取るようにする
//・弱った敵は、逃げるだけでなく、負傷した仲間同士で新たなチームを結成して、プレイヤーに反撃するようにする
//・リーダーは指示を出した後に何もしていないので、何かしらの遠距離攻撃あるいはプレイヤーの行動を妨害するような行動を取るようにするとよいかも


