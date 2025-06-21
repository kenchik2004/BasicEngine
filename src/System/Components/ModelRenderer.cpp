#include "precompile.h"
#include "ModelRenderer.h"
#include "System/Components/MeshCollider.h"
#include "System/ModelManager.h"

void ModelRenderer::Construct()
{
	status.status_bit.on(CompStat::STATUS::SINGLE);
}

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
	mat4x4 mat(CastPhysXQuat(owner->transform->rotation * rot));
	mat.scale(Vector4(owner->transform->scale, 1));
	mat.scale(Vector4(scale, 1));
	mat.setPosition(owner->transform->position + rot.rotate(pos));
	MV1SetMatrix(model->handle, cast(mat));
}

void ModelRenderer::PreDraw()
{
	if (!model)
		return;
	mat4x4 mat(CastPhysXQuat(owner->transform->rotation * rot));
	mat.scale(Vector4(owner->transform->scale, 1));
	mat.scale(Vector4(scale, 1));
	mat.setPosition(owner->transform->position + owner->transform->rotation.rotate(rot.rotate(pos)));

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
	model->Draw();
}



void ModelRenderer::DebugDraw()
{
	if (!model)
		return;
	if (owner->GetComponent<MeshCollider>())
		return;
	MV1SetWireFrameDrawFlag(model->handle, true);
	MV1DrawModel(model->handle);
	MV1SetWireFrameDrawFlag(model->handle, false);
}


