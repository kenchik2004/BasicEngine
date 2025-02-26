#include "precompile.h"
#include "RigidBody.h"

using namespace physx;
int RigidBody::Init()
{
	{
		auto scene = SceneManager::GetCurrentScene();
		auto p_scene = scene->GetPhysicsScene();
		body
			= PhysicsManager::GetPhysicsInstance()->createRigidDynamic(physx::PxTransform(physx::PxIdentity));
		body->userData = new std::weak_ptr<ObjBase>(owner->shared_from_this());

		p_scene->addActor(*body);
		if (body->is<PxRigidDynamic>())
			static_cast<PxRigidDynamic*>(body)->setLinearVelocity(velocity);
	}
	return 0;
}

void RigidBody::PrePhysics()
{
	auto& owner_trns = owner->transform;
	body->setGlobalPose(PxTransform(owner_trns->position + owner_trns->rotation.rotate(pos), owner_trns->rotation * rot));
	if (body->is<PxRigidDynamic>())
		static_cast<PxRigidDynamic*>(body)->setLinearVelocity(velocity);
}

void RigidBody::PostPhysics()
{
	auto& owner_trns = owner->transform;
	auto trns = body->getGlobalPose();
	owner->transform->SetPosition(trns.p - trns.q.rotate(Vector3(pos)));
	owner->transform->SetRotation(trns.q);
	if (body->is<PxRigidDynamic>()) {
		velocity = static_cast<PxRigidDynamic*>(body)->getLinearVelocity();
	}

}

void RigidBody::Update()
{
}

void RigidBody::DebugDraw()
{
	DrawSphere3D(cast(body->getGlobalPose().p), 0.1f, 8, RED, RED, true);
}

void RigidBody::Exit()
{

	SceneManager::GetCurrentScene()->DeleteActor(body);
	body = nullptr;
}
