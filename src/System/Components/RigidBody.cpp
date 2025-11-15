#include "RigidBody.h"

using namespace physx;
void RigidBody::Construct()
{
	status.status_bit.on(CompStat::STATUS::SINGLE);
	SetPriority(0);
}
int RigidBody::Init()
{

	auto p_scene = owner->GetScene()->GetPhysicsScene();
	body
		= PhysicsManager::GetPhysicsInstance()->createRigidDynamic(physx::PxTransform(physx::PxIdentity));
	body->userData = new SafeWeakPtr<Object>(owner->shared_from_this());

	p_scene->addActor(*body);
	if (is_kinematic)
		return 0;
	if (auto rig_body = body->is<PxRigidDynamic>()) {
		rig_body->setLinearVelocity(velocity);
		rig_body->setAngularVelocity(velocity);
	}

	return 0;
}

void RigidBody::PrePhysics()
{
	auto& owner_trns = owner->transform;
	u8 cache_changed = CheckCacheChanged();
	//位置または回転が変化している場合のみ、物理演算に反映する
	if (cache_changed & 0b00000001 || cache_changed & 0b00000010) {
		PxTransform trns(owner_trns->position, owner_trns->rotation);
		body->setGlobalPose(trns);
	}
	if (body->is<PxRigidDynamic>()) {
		auto rig_body = static_cast<PxRigidDynamic*>(body);


		//各種設定反映(こちらは毎フレーム反映しても問題ない)
		rig_body->setMass(mass);
		rig_body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, freeze_position.x);
		rig_body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, freeze_position.y);
		rig_body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, freeze_position.z);
		rig_body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, freeze_rotation.x);
		rig_body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, freeze_rotation.y);
		rig_body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, freeze_rotation.z);
		rig_body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !use_gravity);
		bool  was_kinematic = rig_body->getRigidBodyFlags().isSet(PxRigidBodyFlag::eKINEMATIC);
		rig_body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, is_kinematic);
		//もし前フレームでキネマティックだったのに、今フレームでキネマティックじゃなくなった場合は起こす
		if (!is_kinematic && was_kinematic)
			rig_body->wakeUp();
		//速度が変化している場合のみ、物理演算に反映する
		if (!is_kinematic) {
			if (cache_changed & 0b00000100)
				rig_body->setLinearVelocity(velocity);
			if (cache_changed & 0b00001000)
				rig_body->setAngularVelocity(angular_velocity);
		}

	}
	CacheCurrentData();
}

void RigidBody::PostPhysics()
{
	auto& owner_trns = owner->transform;
	auto trns = body->getGlobalPose();
	u8 cache_changed = CheckCacheChanged();
	if (!(cache_changed & 0b00000001))
		owner_trns->SetPosition(trns.p);
	if (!(cache_changed & 0b00000010))
		owner_trns->SetRotation(trns.q);
	if (auto rig_body = body->is<PxRigidDynamic>()) {

		if (!(cache_changed & 0b00000100))
			velocity = rig_body->getLinearVelocity();
		if (!(cache_changed & 0b00001000))
			angular_velocity = rig_body->getAngularVelocity();
	}
	if (!cache_changed)
		CacheCurrentData();

}

void RigidBody::Update()
{
}

void RigidBody::DebugDraw()
{
	DrawSphere3D(cast(body->getGlobalPose().p), 0.1f, 8, Color::YELLOW, Color::YELLOW, true);
}

void RigidBody::Exit()
{

	owner->GetScene()->DeleteActor(body);
	body = nullptr;
}

void RigidBody::AddForce(Vector3 force, ForceMode force_mode)
{
	if (!body->is<PxRigidDynamic>())
		return;
	bool in_simulation = SceneManager::GetCurrentScene()->IsInSimulation();
	if (!in_simulation && !is_kinematic) {
		static_cast<PxRigidDynamic*>(body)->addForce(force, static_cast<PxForceMode::Enum>(force_mode));
		return;
	}
	auto lambda = [wp = SafeWeakPtr<RigidBody>(std::static_pointer_cast<RigidBody>(shared_from_this())), force, force_mode]() {
		if (!wp)
			return;
		if (wp->is_kinematic)
			return;
		if (static_cast<SafeWeakPtr<Object>*>(wp->body->userData))
			static_cast<PxRigidDynamic*>(wp->body)->addForce(force, static_cast<PxForceMode::Enum>(force_mode));
		};
	SceneManager::GetCurrentScene()->AddFunctionAfterSimulation(lambda);

}

void RigidBody::AddTorque(Vector3 torque, ForceMode force_mode)
{
	if (!body->is<PxRigidDynamic>())
		return;
	bool in_simulation = SceneManager::GetCurrentScene()->IsInSimulation();
	if (!in_simulation && !is_kinematic) {
		static_cast<PxRigidDynamic*>(body)->addTorque(torque, static_cast<PxForceMode::Enum>(force_mode));
		return;
	}
	auto lambda = [wp = SafeWeakPtr<RigidBody>(std::static_pointer_cast<RigidBody>(shared_from_this())), torque, force_mode]() {
		if (!wp)
			return;
		if (wp->is_kinematic)
			return;
		if (static_cast<SafeWeakPtr<Object>*>(wp->body->userData))
			static_cast<PxRigidDynamic*>(wp->body)->addTorque(torque, static_cast<PxForceMode::Enum>(force_mode));
		};
	SceneManager::GetCurrentScene()->AddFunctionAfterSimulation(lambda);
}

void RigidBody::AddForceAtPosition(Vector3 force, Vector3 world_position, ForceMode force_mode)
{
	if (!body->is<PxRigidDynamic>())
		return;
	bool in_simulation = SceneManager::GetCurrentScene()->IsInSimulation();
	if (!in_simulation && !is_kinematic) {
		static_cast<PxRigidDynamic*>(body)->addForce(force, static_cast<PxForceMode::Enum>(force_mode));
		auto cross = (world_position - body->getGlobalPose().p).cross(force);
		static_cast<PxRigidDynamic*>(body)->addTorque(cross, static_cast<PxForceMode::Enum>(force_mode));
		return;
	}
	auto lambda = [wp = SafeWeakPtr<RigidBody>(std::static_pointer_cast<RigidBody>(shared_from_this())), force, world_position, force_mode]() {
		if (!wp)
			return;
		if (wp->is_kinematic)
			return;
		if (static_cast<SafeWeakPtr<Object>*>(wp->body->userData)) {
			static_cast<PxRigidDynamic*>(wp->body)->addForce(force, static_cast<PxForceMode::Enum>(force_mode));
			static_cast<PxRigidDynamic*>(wp->body)->addTorque((world_position - wp->body->getGlobalPose().p).cross(force), static_cast<PxForceMode::Enum>(force_mode));
		}
		};
	SceneManager::GetCurrentScene()->AddFunctionAfterSimulation(lambda);
}

void RigidBody::SetMassCenter(Vector3 center)
{
	if (body->is<PxRigidDynamic>())
		static_cast<PxRigidDynamic*>(body)->setCMassLocalPose(PxTransform(center));
}

void RigidBody::SetVelocity(Vector3 velocity_)
{
	velocity = velocity_;
	bool in_simulation = SceneManager::GetCurrentScene()->IsInSimulation();
	if (!in_simulation && !is_kinematic) {
		if (body->is<PxRigidDynamic>())
			static_cast<PxRigidDynamic*>(body)->setLinearVelocity(velocity_);
		return;
	}
	auto lambda = [wp = SafeWeakPtr<RigidBody>(std::static_pointer_cast<RigidBody>(shared_from_this())), velocity_]() {
		if (!wp)
			return;
		if (wp->is_kinematic)
			return;
		if (auto owner_obj = static_cast<SafeWeakPtr<Object>*>(wp->body->userData))
			if (wp->body->is<PxRigidDynamic>())
				static_cast<PxRigidDynamic*>(wp->body)->setLinearVelocity(velocity_);
		};
	SceneManager::GetCurrentScene()->AddFunctionAfterSimulation(lambda);
}

void RigidBody::ChangeToStatic()
{
	owner->GetScene()->DeleteActor(body);
	auto scene = owner->GetScene();
	auto p_scene = scene->GetPhysicsScene();
	body
		= PhysicsManager::GetPhysicsInstance()->createRigidStatic(physx::PxTransform(physx::PxIdentity));
	body->userData = new SafeWeakPtr<Object>(owner->shared_from_this());

	p_scene->addActor(*body);
}

u8 RigidBody::CheckCacheChanged()
{
	//PhysX側では、連続したsetGlobalPose呼び出しは
	// たとえ値が変わっていなくとも「ワープ」扱いになるため、キャッシュを持って変更があった場合のみ更新する
	//その際も、変更があった項目のみ更新したいので、戻り値はビットフラグにする
	//ビット0:位置変更、ビット1:回転変更、ビット2:速度変更、ビット3:角速度変更
	//また、floatはバイナリチェックするべからずなので、Vector3とQuaternionの比較は内積<FLT_EPSILONで行う

	u8 flags = 0;
	auto& owner_trns = owner->transform;

	// 位置変更チェック
	if ((owner_trns->position - cache_pos_gl).magnitude() >= 1e-6f || (owner_trns->local_position - cache_pos_lc).magnitude() >= 1e-6f) {
		flags |= (1 << 0);
	}

	// 回転変更チェック
	if (1.0f - fabs(owner_trns->rotation.dot(cache_rot_gl)) >= 1e-6f || 1.0f - fabs(owner_trns->local_rotation.dot(cache_rot_lc)) >= 1e-6f) {
		flags |= (1 << 1);
	}

	// 速度変更チェック
	if ((velocity - cache_vel_l).magnitude() >= 1e-6f) {
		flags |= (1 << 2);
	}

	// 角速度変更チェック
	if ((angular_velocity - cache_vel_a).magnitude() >= 1e-6f) {
		flags |= (1 << 3);
	}

	return flags;
}

void RigidBody::CacheCurrentData()
{
	auto& owner_trns = owner->transform;
	cache_pos_gl = owner_trns->position;
	cache_pos_lc = owner_trns->local_position;
	cache_rot_gl = owner_trns->rotation;
	cache_rot_lc = owner_trns->local_rotation;
	cache_vel_l = velocity;
	cache_vel_a = angular_velocity;
}
