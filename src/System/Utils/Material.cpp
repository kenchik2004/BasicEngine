#include "precompile.h"
#include "Material.h"
#include "System/MaterialManager.h"





Material::Material()
{
	instance_count++;

}

Material::~Material()
{
	instance_count--;

}

void Material::SetShaderPs(ShaderPs* override_ps)
{
	pixel_shader = override_ps;
}

void Material::SetShaderVs(ShaderVs* override_vs)
{
	vertex_shader = override_vs;
}

void Material::SetTexture(SafeSharedPtr<Texture> texture, TextureType texture_type)
{
	if (!texture)
		texture = MaterialManager::GetDefaultMat3D()->GetTexture(texture_type);
	textures[static_cast<u32>(texture_type)] = texture;
}





