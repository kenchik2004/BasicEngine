#include "precompile.h"
#include "DiceD4.h"
#include "System/Components/ModelRenderer.h"
#include "System/Components/RigidBody.h"
#include "System/Components/ConvexMeshCollider.h"

std::array<Vector3, 4> DiceD4::dice_vectors = {

	Vector3(0.0f, 1.0f,0.0f),
	Vector3(0.0f, -0.35f,	1.0f),
	Vector3(0.86f,  -0.35f,  -0.5f),
	Vector3(-0.86f, -0.35f,  -0.5f),
};
std::array<Color, 4> color = {
	Color::RED,
	Color::ORANGE,
	Color::GREEN,
	Color::BLUE
};
DiceD4::DiceD4(int texture) :Object()
{
	handle = texture;
}

int DiceD4::Init()
{
	auto mod = AddComponent<ModelRenderer>();
	transform->scale = { 0.5f,0.5f,0.5f };
	mod->SetModel("D4");
	MV1SetTextureGraphHandle(mod->GetModelHandle(), 0, handle, true);
	mod->scale = { 0.01f,0.01f,0.01f };
	//transform->rotation = Quaternion(DEG2RAD(0), Vector3((GetRand(10) - 5) * 0.1f, (GetRand(10) - 5) * 0.1f, (GetRand(10) - 5) * 0.1f).getNormalized());


	auto rb = AddComponent<RigidBody>();
	//static_cast<physx::PxRigidDynamic*>(rb->GetBody())->setAngularDamping(2.0f);
	rb->velocity = Vector3((float)(GetRand(800) - 400)*0.01f, (float)(GetRand(800) - 800) * 0.02f, (float)(GetRand(800) - 400) * 0.01f);
	rb->AddTorque(Vector3(200, 200, 0));
	auto col = AddComponent<ConvexMeshCollider>();
	rb->mass = 5;
	return 0;
}

void DiceD4::Update()
{
	for (int i = 0; i < dice_vectors.size(); i++) {
		if (transform->rotation.rotate(dice_vectors[i]).dot(Vector3(0, 1, 0)) > transform->rotation.rotate(dice_vectors[selected_number]).dot(Vector3(0, 1, 0)))
			selected_number = i;
	}
}

void DiceD4::DebugDraw()
{
	for (int i = 0; i < dice_vectors.size(); i++) {
		DrawLine3D(cast(transform->position), cast(transform->position + transform->rotation.rotate(dice_vectors[i]) * 2), color[i]);
	}
}

void DiceD4::LateDebugDraw()
{
	auto obj_pos = ConvWorldPosToScreenPos(cast(transform->position + Vector3(0, 0.5f, 0)));;
	auto pos = ConvWorldPosToScreenPos(cast(transform->position + Vector3(1.5f, 1, 0)));
	if (pos.z < 1) {
		DrawLineAA(obj_pos.x, obj_pos.y, pos.x, pos.y, color[selected_number]);
		DrawFormatString(pos.x, pos.y, color[selected_number], " NUMBER:%d", selected_number + 1);
	}
}
