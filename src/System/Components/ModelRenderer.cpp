#include "precompile.h"
#include "ModelRenderer.h"
#include "System/Components/MeshCollider.h"


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
	anim_time += Time::DeltaTime() * 60 * anim_speed;
	if (current_index == -1)
		current_anim = nullptr;
	if (current_anim) {
		MV1SetAttachAnimTime(model.handle, current_index, anim_time);
		if (anim_loop && !IsPlaying())
			anim_time = anim_time > current_anim->anim_total_time ? 0 : current_anim->anim_total_time;
	}
}

void ModelRenderer::LateUpdate()
{
	mat4x4 mat(CastPhysXQuat(owner.lock()->transform->rotation));
	mat.scale(Vector4(owner.lock()->transform->scale, 1));
	mat.setPosition(owner.lock()->transform->position);
	mat4x4 local_mat(CastPhysXQuat(rot));
	local_mat.scale(Vector4(scale, 1));
	local_mat.setPosition(pos);
	MV1SetMatrix(model.handle, cast(mat * local_mat));
}

void ModelRenderer::PreDraw()
{

	mat4x4 mat(CastPhysXQuat(owner.lock()->transform->rotation));
	mat.scale(Vector4(owner.lock()->transform->scale, 1));
	mat.setPosition(owner.lock()->transform->position);
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

void ModelRenderer::PlayAnimation(std::string_view name, bool loop, float start_time)
{
	AnimationData* select = nullptr;
	for (auto& ite : animation) {
		if (ite.name == name) {
			select = &ite;
			break;
		}
	}
	if (!select)
		return;
	if (current_anim) {
		MV1DetachAnim(model.handle, current_index);
		current_index = -1;
		current_index = MV1AttachAnim(model.handle, select->animation_index, select->handle, false);
		if (current_index >= 0) {
			current_anim = select;
			anim_loop = loop;
			if (start_time == CUR_ANIMTIME_MAX)
				start_time = select->anim_total_time / 60.0f;
			anim_time = start_time * 60;
		}
		return;
	}
	current_index = MV1AttachAnim(model.handle, select->animation_index, select->handle, false);
	anim_loop = loop;
	if (current_index >= 0) {
		current_anim = select;
		if (start_time == CUR_ANIMTIME_MAX)
			start_time = select->anim_total_time / 60.0f;
		anim_time = start_time * 60;
	}
}

void ModelRenderer::PlayAnimationNoSame(std::string_view name, bool loop, float start_time)
{
	if (!current_anim) {
		PlayAnimation(name, loop, start_time);
		return;
	}

	if (current_anim->name != name) {
		PlayAnimation(name, loop, start_time);
	}

}

std::string_view ModelRenderer::GetCurrentAnimName()
{
	if (current_anim && IsPlaying())
		return current_anim->name;
	return "";
}

void ModelRenderer::DebugDraw()
{
	if (owner.lock()->GetComponent<MeshCollider>())
		return;
	MV1SetWireFrameDrawFlag(model.handle, true);
	MV1DrawModel(model.handle);
	MV1SetWireFrameDrawFlag(model.handle, false);
}

bool ModelRenderer::IsPlaying()
{
	if (!current_anim)
		return false;
	return (anim_time < current_anim->anim_total_time && anim_time >= 0);
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
