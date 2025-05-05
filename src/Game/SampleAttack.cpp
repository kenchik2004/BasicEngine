#include "precompile.h"
#include "SampleAttack.h"
#include"System/Components/ModelRenderer.h"
#include "System/Components/RigidBody.h"
#include"System/Components/CapsuleCollider.h"
#include "Game/SampleCutIn.h"

int SampleAttack::Init()
{
	model = owner->GetComponent<ModelRenderer>();
	model.lock()->PlayAnimationNoSame("punch");
	cut_in = owner->AddComponent<SampleCutIn>();
	cut_in.lock()->cam_pos = owner->transform->AxisX() * -2 + owner->transform->AxisZ() * 2 + owner->transform->AxisY() * 1.5f;
	cut_in.lock()->cam_lookat = owner->transform->AxisY() * 1.5f + owner->transform->AxisZ() * 1.5f;
	return 0;
}

void SampleAttack::Update()
{
	timer += Time::DeltaTime();
	if (timer > 0.3f && timer < 0.5f && !fist.lock()) {
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

	if (model.lock()->GetCurrentAnimName() != "punch")
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
