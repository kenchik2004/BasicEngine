#include "PlayerSuperHeroLandingState.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Components/PlayerCameraMachine.h"
#include "Game/Objects/NeonFade/CraterObject.h"

namespace NeonFade {
	PlayerSuperHeroLandingState::PlayerSuperHeroLandingState(Player* owner_)
		:IState(static_cast<GameObject*>(owner_))
	{
		owner_player = owner_;
		rb = owner_->rb.lock().get();
		animator = owner_->animator.lock().get();
		std::function<bool()> to_idle = [this]() {
			return landing_timer >= LANDING_DURATION;
			};
		RegisterChangeRequest("idle", to_idle, 0);
	}
	void PlayerSuperHeroLandingState::OnEnter(IStateMachine* machine)
	{
		landing_timer = 0.0f;
		// 着地時にプレイヤーのY軸を上方向に向ける
		owner_player->transform->SetAxisY({ 0,1,0 }, owner_player->transform->AxisX());

		//アニメーションの再生速度を変更して着地アニメーションを再生
		// 着地前のアニメーション再生速度を保持しておく
		anim_speed_before_landing = animator->anim_speed;
		// 着地アニメーションの再生速度を設定
		animator->anim_speed = LANDING_ANIM_SPEED;
		// 着地アニメーションを再生（ループなし、ブレンド時間0.2秒、腰のY軸固定なし）
		animator->PlayIfNoSame("player_superhero_landing", false, 0.0f, 0.2f, false);

		rb->velocity = { 0, -LANDING_FORCE, 0 }; // 超高速で着地するようにY軸方向の速度を設定


		// カメラがCINEMATICモードの場合、MANIPULATEモードに切り替える
		owner_player->player_camera_machine->SetTransitionTime(0.2f);
		if (owner_player->player_camera_machine->GetCameraMode() == PlayerCameraMachine::CINEMATIC)
		{
			owner_player->player_camera_machine->SetCameraMode(PlayerCameraMachine::CAMERA_MODE::MANIPULATE);
		}





	}
	void PlayerSuperHeroLandingState::OnExit(IStateMachine* machine)
	{
		owner_player->player_camera_machine->camera_distance_max = 30.0f; // カメラ距離を元に戻す
		if (atk_hit_box) {
			atk_hit_box->RemoveThisComponent();
		}
		if (crater) {
			//クレーターは時間経過で勝手に消えるようにするので、
			//ここでは参照を消すだけに留める。
			crater = nullptr;
		}

		// 着地アニメーションの再生速度の後始末をする
		animator->anim_speed = anim_speed_before_landing; // 着地前のアニメーション再生速度に戻す
		// カメラモードがCINEMATICの場合、MANIPULATEに切り替える
		if (owner_player->player_camera_machine->GetCameraMode() == PlayerCameraMachine::CINEMATIC)
		{
			owner_player->player_camera_machine->SetTransitionTime(0.0f);
			owner_player->player_camera_machine->SetCameraMode(PlayerCameraMachine::CAMERA_MODE::MANIPULATE);
		}
	}
	void PlayerSuperHeroLandingState::Update(IStateMachine* machine, float dt)
	{
		landing_timer += dt;
		float t = (LANDING_DURATION - landing_timer) / LANDING_DURATION;
		t = 1.0f - std::clamp(t, 0.0f, 1.0f); // tを0から1の範囲に正規化
		if (t < 0.2f) {

			owner_player->player_camera_machine->camera_distance_max = std::lerp(17.0f, 90.0f, t * 5.0f);

		}
		if (t > 0.8f)
			owner_player->player_camera_machine->camera_distance_max = std::lerp( 30.0f, 90.0f, (1.0f - t) * 5.0f);

		if (landing_timer > 0.25f) {

			SummonCraterObject();
		}
		if (landing_timer > 0.1f && !atk_hit_box) {
			atk_hit_box = owner_player->AddComponent<SphereCollider>(Vector3(0, 0, 0),
				Quaternion(0, 0, 0, 1),
				COLIDER_RADIUS,
				true, Collider::Layer::Player, Collider::Layer::Enemy);


		}
	}
	void PlayerSuperHeroLandingState::OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info)
	{
		if (hit_info.hit_collision->GetLayer() == Collider::Layer::Enemy)
		{
			// 敵に当たった場合の処理
			auto enem = SafeStaticCast<Enemy>(hit_info.hit_collision->owner.lock());
			Vector3 dir = enem->transform->position - owner_player->transform->position;
			dir.y = 0.0f; // 水平方向のベクトルにする
			dir.normalize();
			dir.y = KNOCKBACK_UPWARD_FORCE;
			dir *= KNOCKBACK_FORCE;
			enem->Damage(DAMAGE);
			enem->Down(dir);
			auto cam_machine = owner_player->player_camera_machine;
			cam_machine->ShakeCamera(CAMERA_SHAKE_INTENSITY, CAMERA_SHAKE_DURATION);

		}

	}
	void PlayerSuperHeroLandingState::SummonCraterObject()
	{
		// すでにクレーターオブジェクトが生成されている場合は何もしない
		if (crater)
			return;

		Vector3 summon_position;
		Vector3 summon_norm;
		// クレーターオブジェクトを生成
		//生成する前に、プレイヤーが地面に近い位置にいるかどうかを確認する
		{

			physx::PxRaycastHit hit[2];

			//位置検出にはRayCastを使う
			RayCastInfo ray_info{ hit,2 };

			Ray ray;

			//RayCastのフィルタリング設定を行う
			physx::PxQueryFilterData filter_data;
			//地形のレイヤーのみを対象にする
			filter_data.data.word0 = Collider::Layer::Terrain;
			filter_data.flags = physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::ePREFILTER;

			ray.direction = { 0, -1, 0 };
			ray.position = owner_player->transform->position;
			ray.length = 20.0f; // 20m下方向にRayを飛ばす

			//RayCastを実行して地面が見つかるか確認する
			auto scene = owner_player->GetScene();
			scene->RayCast(ray, ray_info, filter_data);

			if (!ray_info.hasBlock)
				return; // 地面が見つからなかった場合はクレーターを生成しない
			summon_position = ray_info.block.position;
			summon_norm = ray_info.block.normal;
		}

		crater = SceneManager::Object::Create<CraterObject>("crater", 5.0f);
		crater->transform->position = summon_position;
		crater->transform->SetAxisY(summon_norm);
		crater->transform->scale = { 0.5f,0.5f,0.5f };
		{
			auto fx_obj = SceneManager::Object::Create<GameObjectWithLifeTime>(u8"effect_superhero_landing", 5.0f);
			fx_obj->transform->position = summon_position + Vector3(0, 0.5f, 0);//Zファイトングを避けるために少し上にずらす
			fx_obj->AddComponent<EffectPlayer>(u8"data/FX/smash_down.efk")->Play();

		}


	}
}