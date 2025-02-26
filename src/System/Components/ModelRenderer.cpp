#include "precompile.h"
#include "ModelRenderer.h"


std::vector<AnimationData> ModelRenderer::anim_pool(0);
std::vector<ModelData> ModelRenderer::model_pool(0);
int ModelRenderer::Init()
{
	return 0;
}

void ModelRenderer::Load(std::string_view path, std::string_view name)
{
	for (auto& ite : model_pool) {
		if (ite.path == path) {
			cloned = true;
			model = ite;
			model.handle = MV1CreateCloneModel(ite.handle);
			return;
		}
	}
	model.handle = MV1LoadModel(path.data());
	model.path = path;
	model.name = name;
	model_pool.push_back(model);
}

void ModelRenderer::SetAnimation(std::string_view path, std::string_view name, int index)
{
	for (auto& ite : anim_pool) {
		if (ite.path == path) {
			animation.push_back(ite);
			return;
		}
	}
	AnimationData data;
	data.name = name;
	data.animation_index = index;
	data.path = path;
	data.handle = MV1LoadModel(path.data());
	data.anim_total_time = MV1GetAnimTotalTime(data.handle, index);
	anim_pool.push_back(data);
	animation.push_back(data);
}

void ModelRenderer::Update()
{
	anim_time += Time::DeltaTime() * 60;
	if (current_index == -1)
		current_anim = nullptr;
	if (current_anim) {
		MV1SetAttachAnimTime(model.handle, current_index, anim_time);
		if (anim_loop && anim_time > current_anim->anim_total_time)
			anim_time = 0;
	}
}

void ModelRenderer::LateUpdate()
{
	mat4x4 mat(CastPhysXQuat(owner->transform->rotation));
	mat.scale(Vector4(owner->transform->scale, 1));
	mat.setPosition(owner->transform->position);
	mat4x4 local_mat(CastPhysXQuat(rot));
	local_mat.scale(Vector4(scale, 1));
	local_mat.setPosition(pos);
	MV1SetMatrix(model.handle, cast(mat * local_mat));
}

void ModelRenderer::PostPhysics()
{

	mat4x4 mat(CastPhysXQuat(owner->transform->rotation));
	mat.scale(Vector4(owner->transform->scale, 1));
	mat.setPosition(owner->transform->position);
	mat4x4 local_mat(CastPhysXQuat(rot));
	local_mat.scale(Vector4(scale, 1));
	local_mat.setPosition(pos);
	MV1SetMatrix(model.handle, cast(mat * local_mat));

}

void ModelRenderer::Exit()
{
	if (cloned)
		MV1DeleteModel(model.handle);
}

void ModelRenderer::Draw()
{
	MV1DrawModel(model.handle);
}

void ModelRenderer::PlayAnimation(std::string_view name, bool loop)
{
	AnimationData* select = nullptr;
	for (auto& ite : animation) {
		if (ite.name == name)
			select = &ite;
	}
	if (!select)
		return;
	if (current_anim) {
		MV1DetachAnim(model.handle, current_index);
		current_index = -1;
		MV1AttachAnim(model.handle, select->animation_index, select->handle, false);
		current_anim = select;
		anim_loop = loop;
		return;
	}
	current_index = MV1AttachAnim(model.handle, select->animation_index, select->handle, false);
	anim_loop = loop;
	if (current_index >= 0)
		current_anim = select;
}

void ModelRenderer::PlayAnimationNoSame(std::string_view name, bool loop)
{
	if (!current_anim) {
		PlayAnimation(name, loop);
		return;
	}

	if (current_anim->name != name) {
		PlayAnimation(name, loop);
	}

}

void ModelRenderer::DebugDraw()
{
	MV1SetWireFrameDrawFlag(model.handle, true);
	MV1DrawModel(model.handle);
	MV1SetWireFrameDrawFlag(model.handle, false);
}

void ModelRenderer::UnLoad()
{
	for (auto& ite : model_pool) {
		MV1DeleteModel(ite.handle);
	}
	model_pool.clear();

	for (auto& ite : anim_pool) {
		MV1DeleteModel(ite.handle);
	}
	anim_pool.clear();
}
