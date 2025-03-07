#include "precompile.h"
#include "SampleAttack.h"
#include"System/Components/ModelRenderer.h"
#include "System/Components/RigidBody.h"
#include"System/Components/CapsuleCollider.h"

int SampleAttack::Init()
{
	model = owner->GetComponent<ModelRenderer>();
	model.lock()->PlayAnimationNoSame("punch");
	return 0;
}

void SampleAttack::Update()
{
	timer += Time::DeltaTime();
	if (timer > 0.3f&& timer < 0.5f && !fist.lock()) {
		fist = owner->AddComponent<CapsuleCollider>();
		fist.lock()->is_trigger = true;
		fist.lock()->height = 0.3f;
		fist.lock()->radius = 0.1f;
		fist.lock()->AttachToModel(10);
		fist.lock()->rotation = Quaternion(DEG2RAD(90), Vector3(0, 0, 1));
		fist.lock()->position = Vector3(0, 0, 0);
	}
	if (timer > 0.5f && fist.lock())
		fist.lock()->RemoveThisComponent();

	if (!model.lock()->IsPlaying())
		RemoveThisComponent();
}

void SampleAttack::Exit()
{
	if (fist.lock())
		fist.lock()->RemoveThisComponent();
}

void SampleAttack::OnTriggerEnter(const HitInfo& hit_info)
{
	if (hit_info.hit_collision->owner)
		hit_info.hit_collision->GetRigidBody()->SetVelocity(owner->transform->AxisZ() * 10);
}
