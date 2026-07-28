#include "EnemyEscapeState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Components/EnemyController.h"

namespace NeonFade
{
	EnemyEscapeState::EnemyEscapeState(Enemy* owner_enemy_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		// コンストラクタでオーナーのEnemyオブジェクトを設定
		owner_enemy = owner_enemy_;
		animator = owner_enemy->animator.lock().get();
		rb = owner_enemy->rb.lock().get();
	}
	void EnemyEscapeState::OnEnter(IStateMachine* machine)
	{
		// 逃走状態に入ったときの処理
		// 逃走アニメーションを再生する
		animator->PlayIfNoSame("enemy_escape", true);
	}
	void EnemyEscapeState::OnExit(IStateMachine* machine)
	{}
	bool EnemyEscapeState::CanTransitTo(const std::string& state_name)
	{
		// 遷移可能なステートは、ダメージ、ノックバック、死亡のいずれかのみ
		if (state_name == "damage" || state_name == "knock_front" || state_name == "knock_back" || state_name == "die")
			return true;
		return false;
	}
	void EnemyEscapeState::Update(IStateMachine* machine, float dt)
	{
		auto player = owner_enemy->enem_controller->GetPlayer();
		Vector3 mov_dir;
		Vector3 cur_front = owner_enemy->transform->AxisZ();
		// プレイヤーが存在する場合は、プレイヤーから離れる方向に逃走する
		if (player) {
			//逃走方向をプレイヤーから離れる方向に設定
			mov_dir = owner_enemy->transform->position - player->transform->position;
		}
		// プレイヤーが存在しない場合は、現在の向きに沿って逃走する
		else {
			mov_dir = cur_front;
		}
		// Y軸方向の移動を無効化して水平に逃走する
		mov_dir.y = 0.0f;
		// 逃走方向を正規化
		mov_dir.normalize();

		// 現在の向きと逃走方向の間を補間して、滑らかに回転させる
		mov_dir = Slerp(cur_front, mov_dir, ESCAPE_ROTATION_SPEED * dt);

		// 逃走方向に向けてオブジェクトを回転させる
		owner_enemy->transform->SetAxisZ(mov_dir);
		// 逃走速度を設定
		rb->velocity = mov_dir * ESCAPE_SPEED;
	}
}