#include "EnemyRandomWalkState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include <omp.h>

namespace NeonFade
{
	EnemyRandomWalkState::EnemyRandomWalkState(Enemy* owner_enemy_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		owner_enemy = owner_enemy_;
		animator = owner_enemy->animator.lock().get();
		rb = owner_enemy->rb.lock().get();

		std::function<bool()> default_exit = [this]() {
			// 目的地に近づく or 一定時間経過したら状態遷移を要求する
			static constexpr float MIN_DISTANCE_SQUARED = 5.0f * 5.0f; // 目的地に近いとみなす距離の二乗
			return (owner_enemy->transform->position - next_destination).magnitudeSquared() < MIN_DISTANCE_SQUARED || elapsed_time >= MAX_WALK_TIME;
			};
		RegisterChangeRequest("knock_back", default_exit);
	}
	void EnemyRandomWalkState::OnEnter(IStateMachine* machine)
	{
		animator->Play("enemy_walk", true);
		next_destination = Random::Position({ -100.0f, 0.0f, -100.0f }, { 100.0f, 0.0f, 100.0f }); // ランダムな目的地を生成
		next_destination += owner_enemy->transform->position; // 現在の位置を基準に目的地を設定

		elapsed_time = 0.0f; // 経過時間をリセット
		auto another_enemies_ = SceneManager::Object::GetArray<Enemy>(); // 同じシーン内の他の敵を取得
		another_enemies.clear();
		for (auto& enemy : another_enemies_) {
			if (enemy.get() != owner_enemy) {
				another_enemies.push_back(SafeStaticCast<GameObject>(enemy));
			}
		}
	}
	void EnemyRandomWalkState::Update(IStateMachine* machine, float dt)
	{
		elapsed_time += dt;

		Vector3 mov_dir = { 0,0,0 };

		CalculateBaseMovement(mov_dir);
		CalculateCohesion(mov_dir);

		mov_dir.y = 0; // Y軸の回転を無効にする
		mov_dir.normalize();
		Vector3 forward = owner_enemy->transform->AxisZ();
		static constexpr float ROTATION_SPEED = 0.05f; // 回転の補間速度
		mov_dir = Slerp(forward, mov_dir, ROTATION_SPEED);
		owner_enemy->transform->SetAxisZ(mov_dir); // 目的地への方向を向く
		mov_dir *= walk_speed;

		mov_dir.y = rb->velocity.y; // 現在のY軸の速度を保持
		rb->velocity = mov_dir;

	}
	void EnemyRandomWalkState::OnExit(IStateMachine* machine)
	{
		rb->velocity = { 0, rb->velocity.y, 0 }; // 状態を抜けるときに速度をリセットする
	}

	bool EnemyRandomWalkState::CanTransitTo(const std::string& state_name)
	{
		//自身への遷移は許可しない
		if (state_name == "random_walk")
			return false;
		return true;
	}

	void EnemyRandomWalkState::DebugDraw()
	{
		Vector3 vel = rb->velocity;
		Vector3 pos = owner_enemy->transform->position;
		DrawLine3D(cast(pos), cast(pos + vel), Color::CYAN);
		DrawSphere3D(cast(next_destination), 1.0f, 16, Color::MAGENTA, Color::MAGENTA, false);
	}

	void EnemyRandomWalkState::CalculateBaseMovement(Vector3& out_mov_dir)
	{
		// 目的地への方向を計算する
		Vector3 to_dest = next_destination - owner_enemy->transform->position;
		to_dest.normalize(); // 目的地へのベクトルを正規化する
		out_mov_dir += to_dest; // 前方ベクトルと目的地へのベクトルを補間して滑らかに方向転換する
	}

	void EnemyRandomWalkState::CalculateCohesion(Vector3& out_mov_dir)
	{

		Vector3 cohesion = Vector3(0, 0, 0);
		// 近くの敵に少し引き寄せられるようにする
#pragma omp parallel for
		for (auto& enemy_wp : another_enemies) {
			if (enemy_wp) {
				auto enemy = enemy_wp.lock();
				Vector3 to_enemy = enemy->transform->position - owner_enemy->transform->position;
				float distance_squared = to_enemy.magnitudeSquared();
				distance_squared = max(distance_squared, 0.0001f); // ゼロ除算を防ぐために最小値を設定

				static constexpr float APPROACH_DISTANCE_MAX = 50.0f; // 影響を与える最大距離
				static constexpr float APPROACH_DISTANCE_MIN = 10.0f; // 影響を与える最小距離
				static constexpr float LEAVE_DISTANCE_MIN = 5.0f; // あまりにも近い場合に離れる距離
				static constexpr float APMAX_SQRD = APPROACH_DISTANCE_MAX * APPROACH_DISTANCE_MAX; // 影響を与える最大距離の二乗
				static constexpr float APMIN_SQRD = APPROACH_DISTANCE_MIN * APPROACH_DISTANCE_MIN; // 影響を与える最小距離の二乗
				static constexpr float LEAVE_SQRD = LEAVE_DISTANCE_MIN * LEAVE_DISTANCE_MIN; // あまりにも近い場合に離れる距離の二乗
				static constexpr float APPROACH_RATIO = 0.4f; // 近くの敵に引き寄せられる割合
				static constexpr float LEAVE_RATIO = 1.5f; // あまりにも近い場合に離れる割合

				if (distance_squared < APMAX_SQRD && distance_squared > APMIN_SQRD) { // 近くの敵に対してのみ影響を与える
					float step = (APMAX_SQRD - distance_squared) / (APMAX_SQRD - APMIN_SQRD); // 影響の強さを距離に応じて変化させる
					cohesion += to_enemy.getNormalized() * step * APPROACH_RATIO; // 引き寄せる方向に力を加える
				}
				if (distance_squared < LEAVE_SQRD) { // あまりにも近い場合は少し離れるようにする
					float step = (LEAVE_SQRD - distance_squared) / LEAVE_SQRD; // 影響の強さを距離に応じて変化させる
					cohesion -= to_enemy.getNormalized() * step * LEAVE_RATIO; // 離れる方向に力を加える
				}
			}
		}
		float dot = out_mov_dir.dot(cohesion);
		float direction_factor = (dot >= 0) ? 1.0f : 0.5f; // 引き寄せる方向と同じならそのまま、逆なら半分の力にする
		out_mov_dir += cohesion * direction_factor; // 引き寄せる力を加える
	}

}