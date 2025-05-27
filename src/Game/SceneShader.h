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
	void Draw() override;
	void Exit() override;
	std::shared_ptr<ShaderPs> light_shader_ps = nullptr;
	std::shared_ptr<ShaderVs> light_shader_vs = nullptr;
	std::shared_ptr<ShaderPs> shader_ps = nullptr;
	std::shared_ptr<ShaderVs> shader_vs = nullptr;
	ObjectWP obj;
	CameraWP camera;
	int model[2];
	int shadow_map = 0;
};

