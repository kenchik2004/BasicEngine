#pragma once
#include "System/Scene.h"
#include "System/Shader.h"

USING_PTR(Camera);
class SceneShader :
	public Scene
{
	USING_SUPER(SceneShader);
	int Init();
	void Update();
	void PreDraw() override;
	void Draw() override;
	void Exit() override;
	std::shared_ptr<ShaderPs> light_shader_ps = nullptr;
	std::shared_ptr<ShaderVs> light_shader_vs = nullptr;
	std::shared_ptr<ShaderPs> shader_ps = nullptr;
	std::shared_ptr<ShaderVs> shader_vs = nullptr;
	ObjectWP obj;
	CameraWP camera;

	int model[2];
	SafeSharedPtr<Texture> shadow_map = nullptr;
	ID3D11SamplerState* d3d_shadow_sampler = nullptr;
};

