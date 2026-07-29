#include "EnemyTeamSiegeState.h"
#include "Game/Objects/NeonFade/Enemy.h "
#include "Game/Components/EnemyController.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/TeamMemberEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/EnemyTeam.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/EnemyRVOSystem.h"

namespace NeonFade
{

	EnemyTeamSiegeState::EnemyTeamSiegeState(Enemy* owner_enemy_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		owner_enemy = owner_enemy_;
		player = owner_enemy->enem_controller->GetPlayer().lock().get();
		rb = owner_enemy->rb.lock().get();
		animator = owner_enemy->animator.lock().get();

		// 取り囲み失敗時の遷移要求を登録する
		std::function<bool()> failed_exit_func = [this]() {
			return siege_timer >= SIEGE_DURATION || team_released;
			};

		//取り囲み完了時の遷移要求を登録する
		std::function<bool()> siege_complete_func = [this]() {
			static  constexpr float SIEGE_COMPLETE_SQR_DISTANCE = SIEGE_COMPLETE_DISTANCE * SIEGE_COMPLETE_DISTANCE; // プレイヤーを取り囲む位置に到達したとみなす距離の閾値（二乗値）
			return distance_to_siege_position_sqr <= SIEGE_COMPLETE_SQR_DISTANCE;
			};

		//遷移要求として登録
		RegisterChangeRequest("idle", failed_exit_func, 0);
		RegisterChangeRequest("focus_to_player", siege_complete_func, 1);


	}
	void EnemyTeamSiegeState::OnEnter(IStateMachine* machine)
	{
		brain = static_cast<TeamMemberEnemyBrain*>(owner_enemy->enem_controller->GetBrain());

		if (!brain || team_released)
			return;
		siege_timer = 0.0f;

		EnemyTeam* team = brain->GetTeam();
		if (!team) {
			team_released = true;
			return;
		}
		siege_position = CalculateSiegePosition();
		animator->Play("enemy_fast_run", true);


	}
	void EnemyTeamSiegeState::Update(IStateMachine* machine, float dt)
	{
		siege_timer += dt;

		// プレイヤーと自分の位置を取得する
		Vector3  player_pos = player->transform->position;
		Vector3 owner_pos = owner_enemy->transform->position;

		// プレイヤーが移動した場合に、取り囲み位置を再計算する
		Vector3 player_movement = player_pos - last_player_position;
		//軽量な比較のために、距離の二乗値を計算する
		static constexpr float RECALCULATION_DISTANCE_SQR = SIEGE_RECALCULATION_DISTANCE * SIEGE_RECALCULATION_DISTANCE;
		if (player_movement.magnitudeSquared() > RECALCULATION_DISTANCE_SQR)
		{
			siege_position = CalculateSiegePosition();
		}

		// プレイヤーを取り囲む位置までの移動方向を計算する
		Vector3 move_dir = siege_position - owner_pos;

		// プレイヤーの奥に移動する際に、プレイヤーを押し出さないように避ける方向を計算する
		CalculatePlayerAvoid(move_dir, player_pos, owner_pos);


		move_dir.y = 0.0f; // Y軸方向の移動は無視する


		// 近くの敵に引き寄せられるor押し合う方向を計算する
		EnemyRVOSystem::CalculateCohesion(move_dir, owner_enemy->transform.get(), owner_enemy);

		// 移動と回転を同時に適用する
		EnemyRVOSystem::ApplyMovementAndRotation(move_dir, owner_enemy->transform.get(), rb, ROTATION_SPEED, SIEGE_SPEED);


	}
	void EnemyTeamSiegeState::OnExit(IStateMachine* machine)
	{
		//速度をそのままにしておくと慣性で滑って行ってしまうので、速度を0にする
		//Y軸方向の速度はそのままにする
		rb->velocity = { 0.0f, rb->velocity.y, 0.0f };
	}

	void EnemyTeamSiegeState::DebugDraw()
	{
		DrawSphere3D(cast(siege_position), 0.5f, 8, Color::MAGENTA, Color::MAGENTA, false);
	}

	bool EnemyTeamSiegeState::CanTransitTo(const std::string& state_name)
	{
		// ダメージ、ノックバック、死亡状態への遷移は常に許可する
		if (state_name == "damage" || state_name == "knock_back" || state_name == "knock_front" || state_name == "die")
			return true;

		return false;
	}

	Vector3 EnemyTeamSiegeState::CalculateSiegePosition()
	{

		// ○->敵
		// □->プレイヤー
		// 
		//    ○  ○  ○	
		//  ○　	      ○
		// ○     □     ○
		//  ○　	      ○
		//    ○  ○  ○	
		// こんな感じで、プレイヤーの周りに円形状の陣形を作るように配置する
		//

		// まずは、チームのメンバー数を取得する
		EnemyTeam* team = brain->GetTeam();
		//チームが解散されている場合は、陣形計算を行わない
		if (!team) {
			team_released = true;
			return Vector3(0.0f, 0.0f, 0.0f);
		}
		const auto& members = team->GetMembers();
		// メンバーがいない場合は、陣形計算を行わない
		if (members.empty())
			return Vector3(0.0f, 0.0f, 0.0f);
		// 所属するチームのメンバーの中で、自分が何番目のメンバーかを取得する
		u32 team_member_index = std::find(members.begin(), members.end(), brain) - members.begin();

		float angle_deg = 360.0f / members.size(); // メンバー数に応じて角度を計算する

		float angle_offset = angle_deg * team_member_index; // 自分のメンバー番号に応じて角度をオフセットする


		// プレイヤーの位置を取得する
		Vector3 player_pos = player->transform->position;

		//プレイヤーの位置を中心にする
		Vector3 siege_pos = player_pos;

		//xz方向に円形上のオフセットを加算する
		siege_pos.x += SIEGE_RADIUS * std::cos(DEG2RAD(angle_offset));
		siege_pos.z += SIEGE_RADIUS * std::sin(DEG2RAD(angle_offset));

		last_player_position = player_pos;
		return siege_pos;


	}

	void EnemyTeamSiegeState::CalculatePlayerAvoid(Vector3& move_dir, const Vector3& player_pos, const Vector3& owner_pos)
	{

		// 取り囲み位置までの距離の二乗値を計算する
		distance_to_siege_position_sqr = move_dir.magnitudeSquared();

		//プレイヤーの奥に移動するとき、そのまま向かってしまうと押し出してしまうので、避けるようにする
		static constexpr float AVOID_DISTANCE = 8.0f; // プレイヤーの奥に移動する際の避ける距離
		static constexpr float AVOID_DISTANCE_SQR = AVOID_DISTANCE * AVOID_DISTANCE; // プレイヤーの奥に移動する際の避ける距離（二乗値）
		Vector3 to_player = player_pos - owner_pos;
		to_player.y = 0.0f; // Y軸方向の移動は無視する
		if (to_player.magnitudeSquared() < AVOID_DISTANCE_SQR) {

			//自身の向かう先を正規化して、プレイヤーへの方向ベクトルを引く
			//その結果を正規化して、プレイヤーの奥に移動する際の避ける方向ベクトルを計算する
			Vector3 to_siege = siege_position - owner_pos;
			to_siege.y = 0.0f; // Y軸方向の移動は無視する
			to_siege.normalize();	// 正規化して方向ベクトルにする
			to_siege -= to_player.getNormalized();// プレイヤーへの方向ベクトルを引く

			//結果がゼロベクトルになった場合は、とりあえずランダムな方向に避けるようにする
			//次フレからはズレた方向に避けるようになるので、正常に避けるようになる
			if (to_siege.magnitudeSquared() < 1e-6f) {
				to_siege = Vector3(Random::Range(-1.0f, 1.0f), 0.0f, Random::Range(-1.0f, 1.0f));
			}

			move_dir += to_siege.getNormalized() * 10; // 避ける方向ベクトルを加算する



		}
	}


}
