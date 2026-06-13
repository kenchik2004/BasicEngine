//---------------------------------------------------------------------------
//! @file   EnemyDieState.cpp
//! @brief  EnemyDieStateの実装。敵の死亡状態の処理を行う
//---------------------------------------------------------------------------
#include "EnemyDieState.h"
#include "Game/Objects/NeonFade/Enemy.h"	
#include "Game/Scenes/NeonFade/SceneGame.h"
namespace NeonFade
{
	//! @brief 死亡状態のインスタンス数を管理するカウンタ（シーン上の死亡中敵の数に相当）。
	u32 instance_count = 0;
	//! @brief 死亡状態を構築し、必要なコンポーネント参照を取得する。
	//! @param owner_ この状態を所有する敵オブジェクト。
	EnemyDieState::EnemyDieState(Enemy* owner_)
		:IState(static_cast<GameObject*>(owner_))
	{
		// 生成時にインスタンス数をインクリメントして生存中の死亡状態数を追跡する。
		instance_count++;
		// 状態所有者の敵本体を保持する。
		owner_enemy = owner_;
		// 死亡モーション制御に使用するアニメータを取得する。
		animator = owner_enemy->animator.lock().get();
		// 死亡マテリアル適用に使用するモデルレンダラを取得する。
		model = owner_enemy->model.lock().get();

	}
	//! @brief デストラクタ。インスタンス数をデクリメントする。
	EnemyDieState::~EnemyDieState()
	{
		// 破棄時にカウンタを減算して整合性を保つ。
		instance_count--;
	}
	//! @brief 死亡状態へ入った瞬間の初期化処理。
	//! @param machine ステートマシン本体。
	void EnemyDieState::OnEnter(IStateMachine* machine)
	{
		// 死亡後は物理演算が不要になるため剛体コンポーネントを除去する。
		if (auto rb = owner_enemy->rb.lock())
			rb->RemoveThisComponent();
		// 死亡後は当たり判定が不要になるためコライダコンポーネントを除去する。
		if (auto col = owner_enemy->col.lock())
			col->RemoveThisComponent();
		// モデルの全メッシュに死亡専用マテリアルを適用してビジュアルを切り替える。
		for (u8 i = 0; i < 2; i++) {
			model->SetMaterial(Enemy::death_material, i);
		}
		// 演出タイマを初期化して死亡エフェクトの進行を開始する。
		exit_timer = 0;
	}
	//! @brief 死亡状態を抜ける際の終了処理（現在は空実装）。
	//! @param machine ステートマシン本体。
	void EnemyDieState::OnExit(IStateMachine* machine)
	{
	}
	//! @brief 死亡シェーダパラメータを更新し、演出終了後にオブジェクトを破棄する更新処理。
	//! @param machine ステートマシン本体。
	//! @param dt 前フレームからの経過時間。
	void EnemyDieState::Update(IStateMachine* machine, float dt)
	{
		// 死亡演出の経過時間を加算する。
		exit_timer += dt;
		// 死亡マテリアルが有効でピクセルシェーダが存在する場合にパラメータを更新する。
		if (Enemy::death_material && Enemy::death_material->GetGbufferPixelShader()) {
			auto death_shader = Enemy::death_material->GetGbufferPixelShader();
			// 経過時間を [0, 1] に正規化して死亡エフェクトの進行率を算出する。
			float death_value = physx::PxClamp(exit_timer / EXIT_TIME, 0.0f, 1.0f);
			// 死亡エフェクトの速度パラメータを固定値で設定する。
			float death_speed = 3.0f;
			// シェーダの進行率パラメータへ書き込む。
			death_shader->SetValue("SampleCB.sampleparam1", &death_value);
			// シェーダの速度パラメータへ書き込む。
			death_shader->SetValue("SampleCB.sampleparam3", &death_speed);
		}
		// 演出時間を超えたらオブジェクトを破棄してシーンから取り除く。
		if (exit_timer > EXIT_TIME) {
			SafeSharedPtr<Enemy> owner_enemy_ = std::static_pointer_cast<Enemy>(owner_enemy->shared_from_this());

			//死んだら敵の数を減らす
			static_cast<SceneGame*>(owner_enemy->GetScene().get())->SubtractEnemyCount();
			SceneManager::Object::Destroy(owner_enemy_);
			return;
		}
	}
}