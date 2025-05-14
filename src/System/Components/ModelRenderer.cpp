#include "precompile.h"
#include "ModelRenderer.h"
#include "System/Components/MeshCollider.h"
#include "System/ModelManager.h"

int ModelRenderer::Init()
{
	return 0;
}


void ModelRenderer::SetModel(std::string_view name_, std::string_view new_name_)
{
	model_name = name_;
	new_name = new_name_;
	auto data = ModelManager::CloneModelByName(name_, new_name);
	model = data;
}

void ModelRenderer::Update()
{
	if (!model) {
		model = ModelManager::CloneModelByName(model_name, new_name);
		return;
	}
}

void ModelRenderer::LateUpdate()
{
	if (!model)
		return;
	mat4x4 mat(CastPhysXQuat(owner.lock()->transform->rotation * rot));
	mat.scale(Vector4(owner.lock()->transform->scale, 1));
	mat.scale(Vector4(scale, 1));
	mat.setPosition(owner.lock()->transform->position + rot.rotate(pos));
	MV1SetMatrix(model->handle, cast(mat));
}

void ModelRenderer::PreDraw()
{
	if (!model)
		return;
	mat4x4 mat(CastPhysXQuat(owner.lock()->transform->rotation * rot));
	mat.scale(Vector4(owner.lock()->transform->scale, 1));
	mat.scale(Vector4(scale, 1));
	mat.setPosition(owner.lock()->transform->position + owner.lock()->transform->rotation.rotate(rot.rotate(pos)));

	MV1SetMatrix(model->handle, cast(mat));

}

void ModelRenderer::Exit()
{
	model.reset();
}

void ModelRenderer::Draw()
{
	if (!model)
		return;
	int a = MV1DrawModel(model->handle);
}



void ModelRenderer::DebugDraw()
{
	if (!model)
		return;
	if (owner.lock()->GetComponent<MeshCollider>())
		return;
	MV1SetWireFrameDrawFlag(model->handle, true);
	MV1DrawModel(model->handle);
	MV1SetWireFrameDrawFlag(model->handle, false);
}


