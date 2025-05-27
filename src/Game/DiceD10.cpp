#include "precompile.h"
#include "DiceD10.h"

#include "System/Components/ModelRenderer.h"
#include "System/Components/RigidBody.h"
#include "System/Components/ConvexMeshCollider.h"


std::array<Vector3, 10> DiceD10::dice_vectors = {
	 Vector3(-0.672f, -0.707f, -0.220f),
 Vector3(0.673f,  0.707f,  0.217f)	,
 Vector3(0.417f,  0.707f, -0.571f)	,
 Vector3(-0.672f,  0.707f,  0.220f),
 Vector3(0.673f, -0.707f, -0.217f)	,
 Vector3(-0.416f,  0.707f, -0.572f),
 Vector3(0.001f,  0.707f,  0.707f)	,
 Vector3(0.001f, -0.707f, -0.707f)	,
 Vector3(-0.417f, -0.707f,  0.571f),
 Vector3(0.415f, -0.707f,  0.573f),

};
std::array<Color, 10> color = {
	Color::RED,
	Color::ORANGE,
	Color::YELLOW,
	Color::GREEN,
	Color::DARK_GREEN,
	Color::CYAN,
	Color::BLUE,
	Color::DARK_BLUE,
	Color::PURPLE,
	Color::PINK,
};
DiceD10::DiceD10(int texture) :Object()
{
	handle = texture;
}

int DiceD10::Init()
{
	auto mod = AddComponent<ModelRenderer>();
	transform->scale = { 0.5f,0.5f,0.5f };
	mod->SetModel("D10");
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

void DiceD10::Update()
{
	for (int i = 0; i < dice_vectors.size(); i++) {
		if (transform->rotation.rotate(dice_vectors[i]).dot(Vector3(0, 1, 0)) > transform->rotation.rotate(dice_vectors[selected_number]).dot(Vector3(0, 1, 0)))
			selected_number = i;
	}
}

void DiceD10::DebugDraw()
{
	for (int i = 0; i < dice_vectors.size(); i++) {
		DrawLine3D(cast(transform->position), cast(transform->position + transform->rotation.rotate(dice_vectors[i]) * 0.5f), color[i]);
	}
}

void DiceD10::LateDebugDraw()
{
	auto obj_pos = ConvWorldPosToScreenPos(cast(transform->position + Vector3(0, 0.5f, 0)));;
	auto pos = ConvWorldPosToScreenPos(cast(transform->position + Vector3(1.5f, 1, 0)));
	if (pos.z < 1) {
		DrawLineAA(obj_pos.x, obj_pos.y, pos.x, pos.y, color[selected_number]);
		DrawFormatString(pos.x, pos.y, color[selected_number], " NUMBER:%d", selected_number + 1);
	}
}
