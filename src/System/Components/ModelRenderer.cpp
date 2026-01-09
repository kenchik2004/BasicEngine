#include "ModelRenderer.h"

void ModelRenderer::Construct()
{
	status.status_bit.on(CompStat::STATUS::SINGLE);
	SetPriority(5);
}

int ModelRenderer::Init()
{
	auto shadowmap = SceneManager::Object::Get<ShadowMapObject>(owner->GetScene());
	if (shadowmap)
		shadowmap->RegisterModelRenderer(this);
	return 0;
}


void ModelRenderer::SetModel(std::string_view name_, std::string_view new_name_)
{
	model_name = name_;
	new_name = new_name_;
	auto data = ModelManager::CloneModelByName(name_, new_name);
	model = data;
}

void ModelRenderer::SetModel(SafeSharedPtr<Model> model_, std::string_view new_name_)
{
	model_name = model_->name;
	new_name = new_name_;
	model = model_;

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
	mat4x4 mat(CastPhysXQuat(owner->transform->rotation));
	mat.scale(Vector4(owner->transform->scale, 1));
	//mat.scale(Vector4(scale, 1));
	mat.setPosition(owner->transform->position);
	MV1SetMatrix(model->handle, cast(mat));
}

void ModelRenderer::PostPhysics()
{
	if (!model)
		return;
	mat4x4 mat(CastPhysXQuat(owner->transform->rotation));
	mat.scale(Vector4(owner->transform->scale, 1));
	//mat.scale(Vector4(scale, 1));
	mat.setPosition(owner->transform->position);

	MV1SetMatrix(model->handle, cast(mat));

}

void ModelRenderer::Exit()
{
	auto shadowmap = SceneManager::Object::Get<ShadowMapObject>(owner->GetScene());
	if (shadowmap)
		shadowmap->UnregisterModelRenderer(this);
	model.reset();
}

void ModelRenderer::PreDraw()
{
	//ここでシャドウマップに描画したいが、以下の問題が発生する
	//オブジェクト、コンポーネントのプライオリティは処理の順番であり、メインループはそれぞれのパスを1度ずつしか回さない前提
	//そのため、0~UINT_MAXまでフルで使い切ることを想定している
	//カスケードするなら、unsigned intのプライオリティでは足りない
	//だからと言って範囲を広げるのも違う(そもそもカスケード8枚とかのレベルなら、UINT_MAXの8倍は必要になる)
	//であれば、シャドウマップのPreDrawで全モデルコンポーネントを複数回にわたって描画するのが良いか?
	//しかし、シャドウマップのPreDrawで全モデルコンポーネントを把握する手段がない
	//		->シャドウマップというかライトマネージャにモデルコンポーネントのリストを持たせる?また参照カウントを増やすのか?
	//	
	// 	一応仮の実装法
	//	{	
	//	モデルコンポーネントの初期化時、ライトマネージャに対して、自分を登録する
	//	ライトマネージャはPreDrawで、登録されたモデルコンポーネントをすべて描画する
	//  モデルコンポーネントの終了時、ライトマネージャに対して、自分を登録解除する
	//			->これで頻繁にオブジェクトをGetすることはなくなるので、多少はマシになる...のか？
	//	コンポーネントが破棄される前に登録解除するなら、解放済みアクセスも起こらない
	//  ついでに、所有権問題はsharedを渡すのではなく生ポインタを渡すなら、sharedの参照カウントは増えない
	//  
	//	}		
	//		->ライトマネージャを実装するコストがバカ高いので、今はShadowMap(ライトが持つべき)に直接やらせる
	// 
	// !!
	// 唯一の救いは、BPと違ってSetProcでの実装をしていないため、
	// 同一プライオリティでの処理は書き換えられず、登録順に処理されること
	// あれだと既に登録されているProcを上書きしてしまうので、同一プライオリティで複数登録できない
	//

	mat4x4 mat(CastPhysXQuat(owner->transform->rotation));
	mat.scale(Vector4(owner->transform->scale, 1));
	//mat.scale(Vector4(scale, 1));
	mat.setPosition(owner->transform->position);

	MV1SetMatrix(model->handle, cast(mat));
}

void ModelRenderer::Draw()
{
	if (!model)
		return;
	auto camera = owner->GetScene()->GetCurrentCameraRef();
	if (!camera)
		return;
	model->Draw(camera->render_type == Camera::RenderType::Deferred);
}

void ModelRenderer::DrawToShadowMap()
{
	if (!model)
		return;
	model->use_lighting = false;
	model->Draw(false);
	model->use_lighting = true;
}



void ModelRenderer::DebugDraw()
{
	//if (!model)
	return;
	if (owner->GetComponent<MeshCollider>() || owner->GetComponent<ConvexMeshCollider>())
		return;
	MV1SetWireFrameDrawFlag(model->handle, true);
	MV1DrawModel(model->handle);
	MV1SetWireFrameDrawFlag(model->handle, false);
}

void ModelRenderer::Load(std::string_view path, std::string_view name) {
	ModelManager::LoadAsModel(path, name);
}

MATRIX ModelRenderer::GetFrameWorldMatDX(u32 index)
{
	if (!model)
		return DxLib::MGetIdent();
	int index_max = MV1GetFrameNum(model->handle);
	if (index_max < 0 || index >= static_cast<u32>(index_max))
		return DxLib::MGetIdent();
	return MV1GetFrameLocalWorldMatrix(model->handle, index);
}

mat4x4 ModelRenderer::GetFrameWorldMat(u32 index)
{
	if (!model)
		return mat4x4(physx::PxIdentity);
	int index_max = MV1GetFrameNum(model->handle);
	if (index_max < 0 || index >= static_cast<u32>(index_max))
		return mat4x4(physx::PxIdentity);
	MATRIX result = MV1GetFrameLocalWorldMatrix(model->handle, index);

	return reinterpret_cast<mat4x4&>(result);
}

MATRIX ModelRenderer::GetFrameWorldMatDX(std::string_view frame_name)
{
	if (!model)
		return DxLib::MGetIdent();
	int index = MV1SearchFrame(model->handle, frame_name.data());
	if (index >= 0)
		return MV1GetFrameLocalWorldMatrix(model->handle, index);
	return DxLib::MGetIdent();
}

mat4x4 ModelRenderer::GetFrameWorldMat(std::string_view frame_name)
{
	if (!model)
		return mat4x4(physx::PxIdentity);
	int index = MV1SearchFrame(model->handle, frame_name.data());
	if (index < 0)
		return mat4x4(physx::PxIdentity);
	MATRIX result = MV1GetFrameLocalWorldMatrix(model->handle, index);

	return reinterpret_cast<mat4x4&>(result);
}


void ModelRenderer::SetMaterial(Material* material, size_t index)
{
	if (!model)
		return;
	if (index >= model->materials.size())
		return;
	model->materials[index] = material;
}

Material* ModelRenderer::GetMaterial(size_t index)
{
	if (!model)
		return nullptr;
	if (index >= model->materials.size())
		return nullptr;
	return model->materials[index];
}


