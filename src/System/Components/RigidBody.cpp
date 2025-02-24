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
		// Œ`ó(Box)‚ðì¬
		shape
			= PhysicsManager::GetPhysicsInstance()->createShape(
				// Box‚Ì‘å‚«‚³
				physx::PxSphereGeometry(0.5f),
				// –€ŽCŒW”‚Æ”½”­ŒW”‚ÌÝ’è
				*PhysicsManager::GetPhysicsInstance()->createMaterial(0.99f, 0.99f, 0.1f)
			);
		// Œ`ó‚ð•R‚Ã‚¯
		shape->setLocalPose(PxTransform(Vector3(0, 1, 0)));
		//		shape->setLocalPose(physx::PxTransform(physx::PxIdentity));
		shape->setSimulationFilterData(physx::PxFilterData(1, 1, 0, 0));
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !is_a);
		body->attachShape(*shape);
		static_cast<PxRigidDynamic*>(body)->setCMassLocalPose(PxTransform(Vector3(0, 0.0f, 0)));
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
	DrawSphere3D(cast(body->getGlobalPose().p), 0.1f, 8, CYAN, CYAN, true);
	DrawSphere3D(cast(body->getGlobalPose().p + body->getGlobalPose().q.rotate(shape->getLocalPose().p)), 0.5f, 8, CYAN, CYAN, false);
}

void RigidBody::Exit()
{
	SceneManager::GetCurrentScene()->DeleteActor(body);
	body = nullptr;
}
