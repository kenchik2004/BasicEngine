#include "HuntedEffect.h"

namespace NeonFade {

	int HuntedEffect::Init()
	{
		auto img = AddComponent<ImageRenderer>();
		auto mat = MaterialManager::CreateMaterial(u8"mat_hunted_effect");
		auto movie = TextureManager::Get(u8"movie_hunted_effect");
		DxLib::PlayMovieToGraph(*movie, DX_PLAYTYPE_BACK);
		mat->SetTexture(movie, Material::TextureType::Diffuse);
		mat->SetShaderPs(MaterialManager::LoadPixelShader(u8"data/shader/hunted_effect_ps.fx", u8"hunted_effect_ps"));
		img->SetMaterial(mat);
		return Super::Init();
	}
	void HuntedEffect::Update()
	{
		lifetime += Time::DeltaTime();
		if (lifetime >= LIFE_TIME) {
			SceneManager::Object::Destroy(shared_from_this());
		}
	}
}