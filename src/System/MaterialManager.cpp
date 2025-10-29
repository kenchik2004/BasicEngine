#include "precompile.h"
#include "MaterialManager.h"
#include "System/Utils/Render.h"



Material* MaterialManager::CreateMaterial(std::string_view name)
{
	std::string name_key(name);
	if (materials.count(name_key))
		return materials[name_key].get();
	//新規作成
	auto new_mat = make_safe_unique<Material>();
	new_mat->name = name_key;
	new_mat->pixel_shader = default_pixel_shader.get();
	new_mat->vertex_shader = default_vertex_shader.get();
	new_mat->SetTexture(null_white, Material::TextureType::Diffuse);
	new_mat->SetTexture(null_black, Material::TextureType::Specular);
	new_mat->SetTexture(null_normal, Material::TextureType::Normal);
	new_mat->SetTexture(null_white, Material::TextureType::Roughness);
	new_mat->SetTexture(null_black, Material::TextureType::Metalic);
	auto ret = new_mat.get();
	materials[name_key] = std::move(new_mat);
	return ret;

}

Material* MaterialManager::GetMaterial(std::string_view name)
{
	std::string name_key(name);
	if (materials.count(name_key))
		return materials[name_key].get();
	return nullptr;
}

ShaderPs* MaterialManager::LoadPixelShader(std::string_view path, std::string_view name)
{
	std::string name_key(name);
	if (pixel_shaders.count(name_key))
		return pixel_shaders[name_key].get();
	//新規作成
	auto new_ps = make_safe_unique<ShaderPs>(path);

	auto ret = new_ps.get();
	pixel_shaders[name_key] = std::move(new_ps);
	return ret;
}

ShaderVs* MaterialManager::LoadVertexShader(std::string_view path, std::string_view name, int variant)
{
	std::string name_key(name);
	if (vertex_shaders.count(name_key))
		return vertex_shaders[name_key].get();
	//新規作成
	auto new_vs = make_safe_unique<ShaderVs>(path, variant);
	auto ret = new_vs.get();
	vertex_shaders[name_key] = std::move(new_vs);
	return ret;
}

Material* MaterialManager::GetDefaultMat2D()
{
	return materials["default_2d"].get();
}

Material* MaterialManager::GetDefaultMat3D()
{
	return materials["default_3d"].get();
}

Material* MaterialManager::GetDefaultMatGBuffer()
{
	return materials["default_gbuffer"].get();
}

void MaterialManager::Init()
{
	if (!default_pixel_shader)
		default_pixel_shader = make_safe_unique<ShaderPs>("data/shader/ps_model.fx");
	if (!default_vertex_shader)
		default_vertex_shader = make_safe_unique<ShaderVs>("data/shader/vs_model.fx", 8);
	if (!default_pixel_shader_2d)
		default_pixel_shader_2d = make_safe_unique<ShaderPs>("data/shader/ps_texture.fx");
	if (!default_pixel_shader_gbuffer)
		default_pixel_shader_gbuffer = make_safe_unique<ShaderPs>("data/shader/ps_model_gbuffer.fx");

	if (!null_black) {
		null_black = make_safe_shared<Texture>(LoadGraph("data/null_black.dds"));
	}
	if (!null_white) {
		null_white = make_safe_shared<Texture>(LoadGraph("data/null_white.dds"));
	}
	if (!null_normal) {
		null_normal = make_safe_shared<Texture>(LoadGraph("data/null_normal.dds"));
	}



	MaterialManager::CreateMaterial("default_3d");
	auto default_2d = MaterialManager::CreateMaterial("default_2d");
	default_2d->SetShaderPs(default_pixel_shader_2d.get());
	MaterialManager::CreateMaterial("default_gbuffer")->SetShaderPs(default_pixel_shader_gbuffer.get());

}

void MaterialManager::UpdateShaders()
{
	for (auto& [name, ps] : pixel_shaders) {
		if (ps)
			ps->updateFileWatcher();
	}
	for (auto& [name, vs] : vertex_shaders) {
		if (vs)
			vs->updateFileWatcher();
	}
}

void MaterialManager::Release()
{
	materials.clear();
	pixel_shaders.clear();
	vertex_shaders.clear();
	default_pixel_shader.reset();
	default_vertex_shader.reset();
	default_pixel_shader_2d.reset();
	default_pixel_shader_gbuffer.reset();
	null_black.reset();
	null_white.reset();
	null_normal.reset();
}






