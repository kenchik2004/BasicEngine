#pragma once
#include "System/Utils/Material.h"

class MaterialManager
{
public:
	static inline std::unordered_map<std::string, SafeUniquePtr<Material>> materials;
	static inline std::unordered_map<std::string, SafeUniquePtr<ShaderPs>> pixel_shaders;
	static inline std::unordered_map<std::string, SafeUniquePtr<ShaderVs>> vertex_shaders;
	static Material* CreateMaterial(std::string_view name);
	static Material* GetMaterial(std::string_view name);
	static ShaderPs* LoadPixelShader(std::string_view path, std::string_view name);
	static ShaderVs* LoadVertexShader(std::string_view path, std::string_view name, int variant);
	static Material* GetDefaultMat2D();
	static Material* GetDefaultMat3D();

	static void Init();
	static void UpdateShaders();
	static void Release();
private:
	static inline SafeUniquePtr<ShaderPs> default_pixel_shader;
	static inline SafeUniquePtr<ShaderPs> default_pixel_shader_2d;
	static inline SafeUniquePtr<ShaderVs> default_vertex_shader;
	static inline SafeSharedPtr<Texture> null_white;
	static inline SafeSharedPtr<Texture> null_black;
	static inline SafeSharedPtr<Texture> null_normal;
};
