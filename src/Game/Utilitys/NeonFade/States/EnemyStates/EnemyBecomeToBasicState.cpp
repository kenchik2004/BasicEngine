#include "EnemyBecomeToBasicState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/GameObjectWithLifeTime.h"
#include "Game/Components/EnemyController.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/BasicEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/LeaderEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/TeamMemberEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/EnemyTeam.h"

namespace NeonFade {


	EnemyBecomeToBasicState::EnemyBecomeToBasicState(Enemy* owner_enemy_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		// 所有者のEnemyオブジェクトを保持する
		owner_enemy = owner_enemy_;

		// プレイヤーへの弱参照を取得する
		auto player_wp = owner_enemy->enem_controller->GetPlayer();
		PlayerWP player_wp_cast = SafeStaticCast<Player>(player_wp.lock());
		player = player_wp_cast;

		/// モデルレンダラーへのポインタを取得する
		model = owner_enemy->model.lock().get();

		/// デフォルトの遷移要求を登録する
		auto def_exit_func = [this]() {
			return true;
			};
		RegisterChangeRequest("idle", def_exit_func, 0);
	}

	bool EnemyBecomeToBasicState::CanTransitTo(const std::string& state_name)
	{
		//基本状態への降格中は他のステートに遷移できないようにする
		return false;
	}

	void EnemyBecomeToBasicState::OnExit(IStateMachine* machine)
	{
		auto brain = owner_enemy->enem_controller->GetBrain();
		//現在のHPを取得
		u32  hp = brain->GetHp();

		//新しい脳（AI）を作成し、ステートマシンとプレイヤーへの弱参照を渡す
		auto new_brain = make_safe_unique<BasicEnemyBrain>(static_cast<EnemyStateMachine*>(machine), player);

		//HPを引き継ぐ
		new_brain->SetHp(hp);

		//新しい脳（AI）を敵コントローラーにセットする
		owner_enemy->enem_controller->SetBrain(std::move(new_brain));

		//降格した際に、下っ端の敵のモデルに戻す
		if (model)
			model->SetModel("enemy_model");
	}

	void EnemyBecomeToBasicState::OnEnter(IStateMachine* machine)
	{
		//モデルが急に変わると違和感があるので、エフェクトを出してごまかす
		auto effect_obj = SceneManager::Object::Create<GameObjectWithLifeTime>("effect", 2.0f);

		//エフェクトの位置を敵の位置に設定する
		effect_obj->transform->position = owner_enemy->transform->position;

		//エフェクトを再生するコンポーネントを追加して再生する
		auto effect_comp = effect_obj->AddComponent<EffectPlayer>(u8"data/FX/Power_Down.efkefc");
		effect_comp->Play();
	}


	EnemyBecomeToLeaderState::EnemyBecomeToLeaderState(Enemy* owner_enemy_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		// 所有者のEnemyオブジェクトを保持する
		owner_enemy = owner_enemy_;

		// プレイヤーへの弱参照を取得する
		auto player_wp = owner_enemy->enem_controller->GetPlayer();
		PlayerWP player_wp_cast = SafeStaticCast<Player>(player_wp.lock());
		player = player_wp_cast;

		// モデルレンダラーとアニメーターへのポインタを取得する
		model = owner_enemy->model.lock().get();
		animator = owner_enemy->animator.lock().get();

		/// デフォルトの遷移要求を登録する
		auto def_exit_func = [this]() {
			return timer > BECOME_TO_LEADER_TIME;
			};
		RegisterChangeRequest("idle", def_exit_func, 0);
	}
	void EnemyBecomeToLeaderState::OnExit(IStateMachine* machine)
	{

		{
			// 現在の脳（AI）を取得する
			auto brain = owner_enemy->enem_controller->GetBrain();
			//現在のHPを取得
			u32 hp = brain->GetHp();
			//弱体化状態のままリーダーに昇格する場合は、弱体化状態を解除する
			if (brain->IsWeakened())
				hp = AbstractEnemyBrain::WEAKED_HP_THRESHOLD + 1; // 弱体化状態を解除するために、HPを閾値より1多く設定する

			// TeamMemberEnemyBrainにキャストして、チーム情報を取得する
			auto team_member_brain = dynamic_cast<TeamMemberEnemyBrain*>(brain);
			//元がTeamMemberEnemyBrainであれば、チーム情報を引き継ぐ
			//そうでないならnullptrを渡す
			EnemyTeam* team = team_member_brain ? team_member_brain->GetTeam() : nullptr;
			//元がTeamMemberEnemyBrainでない場合は、リーダーが不在のチームを探して参加させる		
			if (!team) {
				const  auto& teams = EnemyTeam::GetAllTeams();
				for (auto& t : teams) {
					// リーダーがいないチームを見つけたら、そのチームに参加させる
					if (!t->GetLeader()) {
						team = t;
						break;
					}
				}

			}

			// 新しい脳（AI）を作成し、ステートマシンとプレイヤーへの弱参照、チーム情報を渡す
			leader_brain = make_safe_unique<LeaderEnemyBrain>(static_cast<EnemyStateMachine*>(machine), player, team);

			// HPを引き継ぐ
			leader_brain->SetHp(hp);
		}
		// 新しい脳（AI）を敵コントローラーにセットする
		owner_enemy->enem_controller->SetBrain(std::move(leader_brain));
	}

	void EnemyBecomeToLeaderState::OnEnter(IStateMachine* machine)
	{
		// ステートに入ったときの初期化処理
		//経過時間をリセットする
		timer = 0.0f;

		// エフェクトを生成する
		//新しいリーダーが生まれたときに分かりやすいよう、進撃の巨人の落雷のようなエフェクトを出す
		auto effect_obj = SceneManager::Object::Create<GameObjectWithLifeTime>("effect", 2.0f);

		//エフェクトの位置を敵の位置に設定する
		effect_obj->transform->position = owner_enemy->transform->position;
		//サイズ調整
		effect_obj->transform->scale = { 0.6f, 0.6f, 0.6f };

		//エフェクトを再生するコンポーネントを追加して再生する
		auto effect_comp = effect_obj->AddComponent<EffectPlayer>(u8"data/FX/Power_Up.efkefc");
		effect_comp->Play();


	}

	void EnemyBecomeToLeaderState::Update(IStateMachine* machine, float dt)
	{
		//経過時間を更新する
		timer += dt;

		//経過時間が半分を超えたら、モデルをリーダー用のモデルに切り替える
		if (timer > BECOME_TO_LEADER_TIME * 0.5f)
		{
			model->SetModel("enemy_leader_model");
			//初期状態だとTポーズのままなので、アニメーションを再生する
			animator->Play("idle");

		}
	}

	bool EnemyBecomeToLeaderState::CanTransitTo(const std::string& state_name)
	{
		//リーダーへの昇格中は他のステートに遷移できないようにする
		return false;
	}


	EnemyBecomeToMemberState::EnemyBecomeToMemberState(Enemy* owner_enemy_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		// 所有者のEnemyオブジェクトを保持する
		owner_enemy = owner_enemy_;

		// プレイヤーへの弱参照を取得する
		auto player_wp = owner_enemy->enem_controller->GetPlayer();
		PlayerWP player_wp_cast = SafeStaticCast<Player>(player_wp.lock());
		player = player_wp_cast;

		// モデルレンダラーとアニメーターへのポインタを取得する
		model = owner_enemy->model.lock().get();
		animator = owner_enemy->animator.lock().get();

		/// デフォルトの遷移要求を登録する
		auto def_exit_func = [this]() {
			return timer > BECOME_TO_MEMBER_TIME;
			};
		RegisterChangeRequest("idle", def_exit_func, 0);
	}

	bool EnemyBecomeToMemberState::CanTransitTo(const std::string& state_name)
	{
		//チームメンバーへの降格中は他のステートに遷移できないようにする
		return false;
	}

	void EnemyBecomeToMemberState::OnExit(IStateMachine* machine)
	{

		{
			// 現在の脳（AI）を取得する
			auto brain = owner_enemy->enem_controller->GetBrain();
			//現在のHPを取得
			u32 hp = brain->GetHp();
			//弱体化状態のままメンバーに昇格する場合は、弱体化状態を解除する
			if (brain->IsWeakened())
				hp = AbstractEnemyBrain::WEAKED_HP_THRESHOLD + 1; // 弱体化状態を解除するために、HPを閾値より1多く設定する

			// TeamMemberEnemyBrainにキャストして、チーム情報を取得する
			auto team_member_brain = dynamic_cast<TeamMemberEnemyBrain*>(brain);
			//元がTeamMemberEnemyBrainであれば、チーム情報を引き継ぐ
			//そうでないならnullptrを渡す
			EnemyTeam* team = team_member_brain ? team_member_brain->GetTeam() : nullptr;
			//元がTeamMemberEnemyBrainでない場合は、メンバーが少ないチームに参加させる		
			if (!team) {
				const  auto& teams = EnemyTeam::GetAllTeams();

				// メンバーが最も少ないチームを見つける
				EnemyTeam* least_member_team = nullptr;
				for (auto& t : teams) {
					//見つかっている中で最もメンバーが少ないチームを更新する
					if (!least_member_team || t->GetMemberNum() < least_member_team->GetMemberNum()) {
						least_member_team = t;
					}
				}
				// メンバーが最も少ないチームに参加させる
				team = least_member_team;
			}

			// 新しい脳（AI）を作成し、ステートマシンとプレイヤーへの弱参照、チーム情報を渡す
			member_brain = make_safe_unique<TeamMemberEnemyBrain>(static_cast<EnemyStateMachine*>(machine), player, team);

			// HPを引き継ぐ
			member_brain->SetHp(hp);
		}
		// 新しい脳（AI）を敵コントローラーにセットする
		owner_enemy->enem_controller->SetBrain(std::move(member_brain));
	}
	void EnemyBecomeToMemberState::OnEnter(IStateMachine* machine)
	{
		timer = 0.0f;
	}
	void EnemyBecomeToMemberState::Update(IStateMachine* machine, float dt)
	{
		timer += dt;
	}
}