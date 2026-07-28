#include "TitlePlayerStartSpawnState.h"
#include "Game/Objects/NeonFade/TitlePlayer.h"
#include "Game/Utilitys/NeonFade/StateMachines/TitlePlayerStateMachine.h"


namespace NeonFade
{
	TitlePlayerStartSpawnState::TitlePlayerStartSpawnState(TitlePlayer* player_) :
		IState(player_)
	{
		//自身を所有するプレイヤーのポインタを保存する
		owner_player = player_;
		//プレイヤーのモデルレンダラーとアニメーターのポインタを取得する
		model = owner_player->GetComponent<ModelRenderer>().get();
		anim = owner_player->GetComponent<Animator>().get();

		//依存しているリソースが存在しない場合は、無効なステートとして扱う
		if (!model || !anim) {
			owner_player = nullptr;
			model = nullptr;
			anim = nullptr;
			return;
		}

		//プレイヤーのデフォルトのマテリアルを複製して、スポーン用のマテリアルを作成する
		//もともとのマテリアルはスポーンエフェクトのシェーダーをセットする前に保存しておいて、スポーンエフェクトが終わったら元に戻す
		default_mats.resize(model->GetMaterialsNum());
		spawn_mats.resize(model->GetMaterialsNum());
		for (u32 i = 0; i < default_mats.size(); i++)
			default_mats[i] = model->GetMaterial(i);

		//新規でマテリアルを作成して、テクスチャはもともとのマテリアルからコピーする
		for (u32 i = 0; i < spawn_mats.size(); i++)
		{
			spawn_mats[i] = MaterialManager::CreateMaterial(u8"TitlePlayerSpawnMat" + std::to_string(i));
			for (u32 j = 0; j < static_cast<u32>(Material::TextureType::Max); j++)
			{
				spawn_mats[i]->SetTexture(model->GetMaterial(i)->GetTexture(static_cast<Material::TextureType>(j)), static_cast<Material::TextureType>(j));
			}
		}

		//スポーンエフェクト用のシェーダーをセットする
		spawn_ps = MaterialManager::LoadPixelShader(u8"data/shader/ps_title_player.fx", u8"TitlePlayerSpawnPS");
		for (auto& spawn_mat : spawn_mats)
			spawn_mat->SetShaderPs(spawn_ps, true);
	}

	void TitlePlayerStartSpawnState::OnEnter(IStateMachine* machine)
	{

		//ステートに必要なリソースが揃っていない場合は、ステートの処理を行わない
		if (!owner_player || !model || !anim || spawn_mats.empty() || !spawn_ps)
			return;

		//スポーンエフェクト用のマテリアルをセットして、スポーンアニメーションを再生する
		for (u32 i = 0; i < spawn_mats.size(); i++)
			model->SetMaterial(spawn_mats[i], i);

		anim->Play("player_sitting", true);

		//スポーン開始位置と回転を設定する
		Vector3 title_player_pos = { 363.218f,1526.004f,148.985f };
		Quaternion title_player_rot = Quaternion(2.02f, { 0,1,0 });
		owner_player->transform->position = title_player_pos;
		owner_player->transform->rotation = title_player_rot;
		spawn_timer = 0.0f;

		//スポーンエフェクト用のシェーダーに、スポーン開始位置とキャラクターの高さを渡す

		Vector3 spawn_start_pos = owner_player->transform->position;
		spawn_ps->SetValue("TitlePlayerInfo.spawn_start_pos", &spawn_start_pos);

		float character_height = 5.0f;
		spawn_ps->SetValue("TitlePlayerInfo.character_height", &character_height);
	}

	void TitlePlayerStartSpawnState::Update(IStateMachine* machine, float dt)
	{
		if (!owner_player || !model || !anim || spawn_mats.empty() || !spawn_ps)
			return;

		//スポーンエフェクトのシェーダーに、スポーン開始からの経過時間を渡す
		spawn_timer += dt;
		spawn_ps->SetValue("TitlePlayerInfo.spawn_timer", &spawn_timer);
	}

	void TitlePlayerStartSpawnState::OnExit(IStateMachine* machine)
	{
		//スポーンエフェクト用のマテリアルを元に戻す
		if (owner_player && !default_mats.empty()) {
			for (u32 i = 0; i < default_mats.size(); i++)
				model->SetMaterial(default_mats[i], i);
		}
	}
}