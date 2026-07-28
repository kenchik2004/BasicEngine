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
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyTeamSiegeState.h"

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
			auto become_basic_state = make_safe_unique<EnemyBecomeToBasicState>(machine->enemy);
			machine->AddState("become_basic", std::move(become_basic_state));

			// 指示を受けた際にリーダーになるステートを登録する
			auto become_leader_state = make_safe_unique<EnemyBecomeToLeaderState>(machine->enemy);
			machine->AddState("become_leader", std::move(become_leader_state));


			//ドロップキック攻撃ステートを登録する
			auto atk_1_state = make_safe_unique<EnemyDropKickAttackEntryState>(state_machine->enemy);
			state_machine->AddState("drop_kick", std::move(atk_1_state));

			// チームメンバー専用の包囲攻撃ステートを登録する
			auto team_siege_state = make_safe_unique<EnemyTeamSiegeState>(state_machine->enemy);
			state_machine->AddState("team_siege", std::move(team_siege_state));
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
		{
			result = "team_siege";
			return result;
		}
#endif

		// もしチームが存在しない場合は、基本的な敵Brainをセットする
		// ここでそのままセットしてしまうと、スタックが巻き戻ってきたときに use after free が発生するので、
		// 遷移先のステートを返すことで、ステートマシン側でBrainを切り替えるようにする
		if (become_leader) {
			result = "become_leader";
			return result;
		}
		if (!my_team) {
			result = "become_basic";
			return result;
		}
		if (go_to_attack) {
			result = "drop_kick";
			go_to_attack = false;
		}


		return result;
	}

	void TeamMemberEnemyBrain::DebugDraw()
	{
		Vector3 pos = machine->enemy->transform->position;
		if (!my_team) {
			DrawSphere3D(cast(pos), 3.0f, 8, Color::RED, Color::RED, true);
			return;
		}
		auto leader = my_team->GetLeader();
		if (!leader) {
			DrawSphere3D(cast(pos), 3.0f, 8, Color::RED, Color::RED, true);
			return;
		}
		Vector3 to_leader = leader->GetMachine()->enemy->transform->position - pos;
		DrawLine3D(cast(pos), cast(pos + to_leader), Color::RED);

	}

	void TeamMemberEnemyBrain::ResetTeamRef()
	{
		my_team = nullptr;
	}

	void TeamMemberEnemyBrain::SetTeamRef(EnemyTeam* team)
	{
		my_team = team;
	}

	void TeamMemberEnemyBrain::Die()
	{
		__super::Die();

		//死亡時にチームを解散するか、新しいリーダーを選出する
		ReleaseTeamOrSelectNewLeader();
	}

	void TeamMemberEnemyBrain::ReleaseTeamOrSelectNewLeader()
	{
		//既に解散済みなら何もしない
		//また、次のリーダーになる予定がない場合も何もしない
		if (!my_team || !become_leader)
			return;

		//チームのポインタを取得する(自身を削除した後にmy_teamがnullptrになるので、先に取得しておく)
		EnemyTeam* team = my_team;

		my_team->SubMember(this); //チームから自分を削除する

		//壊滅状態(メンバーが一定数以下)の場合は、チームを解散する
		if (team->GetMemberNum() <= LeaderEnemyBrain::TEAM_RELEASE_MEMBER_NUM) {
			team->ClearTeam();
		}
		//チームがまだ成立しうる(=メンバーがまだ残っている)場合は、ランダムで新しいリーダーを選出する
		else {
			//ランダムなインデックスを生成して、メンバーの中から新しいリーダーを選出する
			auto members = team->GetMembers();
			u32 rand_idx = Random::Int(0, static_cast<int>(members.size() - 1));

			//次は君だ
			members[rand_idx]->BecomeLeader();
		}
	}
}






//どのように爽快感を感じるか(目指すところ)
//敵同士が連携し合うので、まとめて倒すのが難しい
//敵の連携を崩して、隙が出来たところにド派手な攻撃を決めてまとめて倒すことにより爽快感を感じさせる

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


//バレット様フィードバック:
//リーダーちょっと分かりづらいな。もうちょいなんか指令出すときとかに強調できる表現あるといいね				<-部分的にClear!
//敵が一気に襲ってくるように見えてるから、なんかしらでタイミングずらしたり、別の行動取れるようにしとくといいね
//操作説明が微妙やね。分かりづらい。TIPS的な感じとか、チュートリアルとか、強制的に見せる必要あるね
//ステート量産して、シンプルなゲームルールにステージデザイン的なところで肉付けできるといいね。
//敵がいい感じのフォーメーションを組んで、プレイヤーを取り囲むとかいいね。									<-Clear!
//プレイヤーが敵に埋もれちゃうから、攻撃後にちょっと離れさせるといいかも。

//あーあと、今年の応募はまだ ウン十件くらいやね。ちなみにまだ一人も取ってない。


