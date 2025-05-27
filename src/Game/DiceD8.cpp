#include "precompile.h"
#include "DiceD8.h"
#include "System/Components/ModelRenderer.h"
#include "System/Components/RigidBody.h"
#include "System/Components/ConvexMeshCollider.h"

std::array<Vector3, 8> DiceD8::dice_vectors = {

Vector3(0.577f,-0.577f,0.577f),
Vector3(0.577f,0.577f,0.577f),
Vector3(-0.577f,-0.577f,-0.577f),
Vector3(-0.577f,-0.577f,0.577f),
Vector3(0.577f,0.577f,-0.577f),
Vector3(0.577f,-0.577f,-0.577f),
Vector3(-0.577f,0.577f,0.577f),
Vector3(-0.577f,0.577f,-0.577f),

};
std::array<Color, 8> color = {
	Color::RED,
	Color::ORANGE,
	Color::YELLOW,
	Color::GREEN,
	Color::DARK_GREEN,
	Color::CYAN,
	Color::BLUE,
	Color::PURPLE,
};
DiceD8::DiceD8(int texture) :Object()
{
	handle = texture;
}

int DiceD8::Init()
{
	auto mod = AddComponent<ModelRenderer>();
	mod->SetModel("D8");
	MV1SetTextureGraphHandle(mod->GetModelHandle(), 0, handle, true);
	mod->scale = { 0.01f,0.01f,0.01f };
	transform->position = Vector3((GetRand(100) - 50) * 0.1f, 1.6f, (GetRand(100) - 50) * 0.1f);
	transform->rotation = Quaternion(DEG2RAD(GetRand(360)), Vector3((GetRand(10) - 5) * 0.1f, (GetRand(10) - 5) * 0.1f, (GetRand(10) - 5) * 0.1f).getNormalized());


	auto rb = AddComponent<RigidBody>();
	//static_cast<physx::PxRigidDynamic*>(rb->GetBody())->setAngularDamping(2.0f);
	rb->AddTorque(Vector3((float)(GetRand(800) - 400), (float)(GetRand(800) - 400), (float)(GetRand(800) - 400)));
	auto col = AddComponent<ConvexMeshCollider>();
	rb->mass = 5;
	return 0;
}

void DiceD8::Update()
{
	for (int i = 0; i < dice_vectors.size(); i++) {
		if (transform->rotation.rotate(dice_vectors[i]).dot(Vector3(0, 1, 0)) > transform->rotation.rotate(dice_vectors[selected_number]).dot(Vector3(0, 1, 0)))
			selected_number = i;
	}
}

void DiceD8::DebugDraw()
{
	for (int i = 0; i < dice_vectors.size(); i++) {
		DrawLine3D(cast(transform->position), cast(transform->position + transform->rotation.rotate(dice_vectors[i]) * 2), color[i]);
	}
}

void DiceD8::LateDebugDraw()
{
	auto obj_pos = ConvWorldPosToScreenPos(cast(transform->position + Vector3(0, 0.5f, 0)));;
	auto pos = ConvWorldPosToScreenPos(cast(transform->position + Vector3(1.5f, 1, 0)));
	if (pos.z < 1) {
		DrawLineAA(obj_pos.x, obj_pos.y, pos.x, pos.y, color[selected_number]);
		DrawFormatString(pos.x, pos.y, color[selected_number], " NUMBER:%d", selected_number + 1);
	}
}
