#include "precompile.h"
#include "PlaneObject.h"
#include "System/Components/RigidBody.h"
#include "System/Components/ModelRenderer.h"
#include "System/Components/CapsuleCollider.h"


int PlaneObject::Init()
{
	rb = AddComponent<RigidBody>();
	auto capsule = AddComponent<CapsuleCollider>();
	capsule->height = 1.2f;
	capsule->radius = 0.1f;
	capsule->rotation = Quaternion(DEG2RAD(90), { 0,1,0 });
	capsule->position = { 0.5f,0.2f,0 };
	col = capsule;
	rb.lock()->freeze_rotation = { 1,1,1 };
	model = AddComponent<ModelRenderer>();
	model.lock()->Load("data/aircraft.mv1", "aircraft");
	model.lock()->scale = { 0.0002f,0.0002f,0.0002f };
	model.lock()->rot = Quaternion(DEG2RAD(180), { 0,1,0 });
	transform->position = { 0,5,0 };

	SetCameraNearFar(0.1f, 10000);
	return 0;
}

void PlaneObject::Update()
{
	if (Input::CheckHitKey(KEY_INPUT_W))
		transform->AddRotation({ DEG2RAD(30) ,0,0 });
	if (Input::CheckHitKey(KEY_INPUT_S))
		transform->AddRotation({ -DEG2RAD(30) ,0,0 });
	if (Input::CheckHitKey(KEY_INPUT_A))
		transform->AddRotation({ 0 ,0,DEG2RAD(30) });
	if (Input::CheckHitKey(KEY_INPUT_D))
		transform->AddRotation({ 0 ,0,-DEG2RAD(30) });
	if (Input::CheckHitKey(KEY_INPUT_UP))
		camera_rot *= Quaternion(DEG2RAD(30) * Time::DeltaTime(), { 1,0,0 });
	if (Input::CheckHitKey(KEY_INPUT_DOWN))
		camera_rot *= Quaternion(DEG2RAD(-30) * Time::DeltaTime(), { 1,0,0 });
	if (Input::CheckHitKey(KEY_INPUT_LEFT))
		camera_rot *= Quaternion(DEG2RAD(30) * Time::DeltaTime(), { 0,0,1 });
	if (Input::CheckHitKey(KEY_INPUT_RIGHT))
		camera_rot *= Quaternion(DEG2RAD(-30) * Time::DeltaTime(), { 0,0,1 });
	if (Input::CheckHitKey(KEY_INPUT_LSHIFT))
		speed += 1.0f * Time::DeltaTime();
	if (Input::CheckHitKey(KEY_INPUT_LCONTROL))
		speed -= 1.0f * Time::DeltaTime();
	if (speed<0 || speed>SPEED_MAX)
		speed = speed < 0 ? 0 : SPEED_MAX;

	Vector3 forwardDir = transform->AxisZ();
	Vector3 upDir = transform->AxisY();
	Vector3 velocity = rb.lock()->velocity;
	Vector3 velocityDir = velocity.getNormalized();
	float angleOfAttack = physx::PxAcos(velocityDir.dot(forwardDir));
	float slipAngle = physx::PxAcos(velocityDir.dot(forwardDir));
	// —g—Í‚ÌŒvŽZ
	float airDensity = 1.225f;
	float wingArea = 0.2f;
	float maxLiftCoeff = 1.0f;
	float liftCoeff = physx::PxSin(angleOfAttack) * maxLiftCoeff; // AoA‚ÉŠî‚Ã‚­—g—ÍŒW”
	float liftForce = 0.5f * airDensity * wingArea * liftCoeff * velocity.magnitude();

	// **—g—ÍƒxƒNƒgƒ‹**
	Vector3 lift = upDir * liftForce;

	// **‘O•û„i¬•ª‚ð’Šo**
	float forwardComponent = upDir.dot(forwardDir); // —g—Í‚Ì‚¤‚¿A‘O•û‚ÖŒü‚©‚¤Š„‡
	Vector3 forwardThrust = forwardDir * (liftForce * forwardComponent); // „i—Í‚Æ‚µ‚Ä“K—p

	float sideDragCoeff = 1.0f; // ‰¡ŠŠ‚è—}§‚Ì‹­‚³i’²®—pj

	// ‰¡•ûŒü‚Ì‘¬“x¬•ª‚ðŽæ“¾
	Vector3 lateralVelocity = velocity - (forwardDir * velocity.dot(forwardDir));

	// ‰¡ŠŠ‚è‚ð—}‚¦‚éR—Í‚ð‰Á‚¦‚é
	Vector3 sideDrag = -lateralVelocity * (slipAngle * sideDragCoeff);
	float dragCoeff = 1.8f;
	float angleBetween = physx::PxAcos(physx::PxClamp(forwardDir.dot(velocityDir), -1.0f, 1.0f)); // ƒ‰ƒWƒAƒ“

	// **‹ó‹C’ïR‚ÌŒvŽZ**
	float baseDrag = 0.000002f;             // ’ïR‚ÌŠî‘b’li¬‚³‚¢’lj
	float dragFactor = 0.001f;           // Šp“x‚É‚æ‚é‰e‹¿‚Ì‘å‚«‚³i’²®‰Â”\j
	float dragCoefficient = baseDrag + physx::PxSin(angleBetween) * dragFactor; // Šp“x‚ª‘å‚«‚¢‚Ù‚Ç’ïR‚ª‘‚·
	Vector3 dragForce = -velocity * dragCoefficient * velocity.magnitude(); // ‘¬“x‚É”ä—á‚µ‚½’ïR

	// —Í‚ð“K—p
	rb.lock()->AddForce(lift, Force); // —g—Í
	//rb.lock()->AddForce(sideDrag, Acceleration);
	rb.lock()->AddForce(dragForce, Acceleration);
	rb.lock()->AddForce(forwardThrust, Force); // ‘O•û„i—Í
	rb.lock()->AddForce(forwardDir * speed, Force); // ‘O•û„i—Í


}

void PlaneObject::PreDraw()
{
	float3 cam_pos = transform->position;
	cam_pos += camera_rot.rotate(transform->AxisZ()) * -3.0f;
	SetCameraPositionAndTargetAndUpVec(cam_pos, cast(transform->position + transform->AxisY() * 0.5f), cast(camera_rot.getBasisVector1()));
}

void PlaneObject::LateDraw()
{
	DrawFormatString(0, 0, RED, "ƒGƒ“ƒWƒ“„—Í: %.1f %", speed / SPEED_MAX * 100.0f);
	DrawFormatString(0, 24, RED, "‘Î’n‘¬“x: %.1f km/h", rb.lock()->velocity.magnitude());
}

void PlaneObject::OnCollisionEnter(const HitInfo& hit_info)
{
	ObjBaseWP this_ = shared_from_this();
	if (rb.lock()->velocity.magnitude() >= 500)
		if (hit_info.hit_collision->owner->tag == Object::Stage)
			SceneManager::Object::Destory(shared_from_this());
}
