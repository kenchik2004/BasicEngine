#pragma once
#include "System/Shader.h"
class Material
{
	friend class MaterialManager;
public:
	Material();
	~Material();
	enum class TextureType :u32 {
		Diffuse,
		Normal,
		Specular,
		Roughness,
		Metalic,

		Max,
	};
private:
	ShaderPs* pixel_shader;
	ShaderVs* vertex_shader;
	std::array<SafeSharedPtr<Texture>, static_cast<u32>(TextureType::Max)> textures;
	std::string name;
	static inline u32 instance_count = 0;
public:
	void SetShaderPs(ShaderPs* override_ps);
	void SetShaderVs(ShaderVs* override_vs);
	void SetTexture(SafeSharedPtr<Texture> texture, TextureType texture_type);
	inline SafeSharedPtr<Texture> GetTexture(TextureType type) {
		if (type >= TextureType::Max)
			return nullptr;
		return textures[static_cast<u32>(type)];
	}
	ShaderPs* GetPixelShader() { return pixel_shader; }
	ShaderVs* GetVertexShader() { return vertex_shader; }
};

