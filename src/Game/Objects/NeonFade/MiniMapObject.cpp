#include "MiniMapObject.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Scenes/NeonFade/SceneGame.h"

namespace NeonFade {


	static constexpr float MAP_SCALE = 0.05f;	// ミニマップ上のスケール
	static constexpr float MAP_RADIUS = 125.0f;	// ミニマップの半径
	static constexpr float MAP_X_MAX = 533.0f;	// ミニマップのX方向の最大値
	static constexpr float MAP_X_MIN = -535.0f;	// ミニマップのX方向の最小値
	static constexpr float MAP_Z_MAX = 746.0f;	// ミニマップのZ方向の最大値
	static constexpr float MAP_Z_MIN = -333.0f;	// ミニマップのZ方向の最小値
	static constexpr float MAP_X_RANGE = MAP_X_MAX - MAP_X_MIN;	// ミニマップのX方向の範囲
	static constexpr float MAP_Z_RANGE = MAP_Z_MAX - MAP_Z_MIN;	// ミニマップのZ方向の範囲
	static constexpr float MAP_X_CENTER = MAP_X_MIN + MAP_X_RANGE * 0.5f;	// ミニマップのX方向の最大値
	static constexpr float MAP_Z_CENTER = MAP_Z_MIN + MAP_Z_RANGE * 0.5f;	// ミニマップのZ方向の最大値
	static constexpr float MAP_BORDER = 15.0f;	// ミニマップの境界線の幅

	class MiniMapPositionRenderer : public Component
	{
	public:
		USING_SUPER(MiniMapPositionRenderer);
		int Init() override {

			enemies = &Enemy::GetAllEnemies();
			auto scene_game = SafeStaticCast<SceneGame>(owner->GetScene());
			player = scene_game->player;
			return Super::Init();
		}
		void LateDraw() override {
			if (!player)
				return;

			Vector2 draw_pos = {
								owner->transform->position.x + owner->transform->scale.x * 0.5f,
								owner->transform->position.y - owner->transform->scale.y * 0.5f };

			if (enemies && !enemies->empty()) {
				for (const auto& enemy : *enemies) {
					Vector2 enemy_pos = { enemy->transform->position.x, enemy->transform->position.z };
					enemy_pos.x -= player->transform->position.x;
					enemy_pos.y -= player->transform->position.z;
					if (enemy_pos.magnitudeSquared() > (MAP_RADIUS * MAP_RADIUS))
						enemy_pos = enemy_pos.getNormalized() * MAP_RADIUS;
					enemy_pos += draw_pos;
					DxLib::DrawCircleAA(enemy_pos.x, -enemy_pos.y, 3.0f, 5, Color::RED, true);
				}
			}

			Vector3 pl_forward = player->transform->AxisZ();
			pl_forward.y = 0;
			pl_forward.normalize();
			float pl_forward_cos = pl_forward.dot(Vector3(0, 0, 1));
			float player_rot = std::acos(std::clamp(pl_forward_cos, -1.0f, 1.0f));
			player_rot *= (pl_forward.x > 0) ? -1.0f : 1.0f;
			std::array<Vector2, 3> triangle_vertices = {
				Vector2(0.0f, 8.0f),
				Vector2(0.0f, 5.0f),
				Vector2(0.0f, 5.0f)
			};

			for (u32 i = 0; i < triangle_vertices.size(); ++i) {
				static constexpr float angle_offset = DEG2RAD(120.0f);
				const float angle = player_rot + angle_offset * i;
				auto& vertex = triangle_vertices[i];
				Vector2 transformed = { 0.0f,0.0f };
				transformed.x = std::cos(angle) * vertex.x - std::sin(angle) * vertex.y;
				transformed.y = std::sin(angle) * vertex.x + std::cos(angle) * vertex.y;
				transformed += draw_pos;
				vertex = transformed;
			}
			DrawTriangleAA(triangle_vertices[0].x, -triangle_vertices[0].y,
				triangle_vertices[1].x, -triangle_vertices[1].y,
				triangle_vertices[2].x, -triangle_vertices[2].y,
				Color::GREEN, true);
		}

	private:

		static constexpr float MAP_RADIUS = MAP_RADIUS;	// ミニマップの半径
		const std::vector<NeonFade::Enemy*>* enemies = nullptr;	// ミニマップ上に表示する敵のリスト


		SafeWeakPtr<NeonFade::Player> player = nullptr;	// ミニマップ上に表示するプレイヤーへの弱参照

	};

	ShaderPs* ps_minimap = nullptr;
	int MiniMapObject::Init()
	{

		auto scene_game = SafeStaticCast<SceneGame>(GetScene());
		player = scene_game->player;

		auto img_renderer = AddComponent<ImageRenderer>();
		auto mat = MaterialManager::CreateMaterial("MiniMapMaterial", MaterialManager::GetDefaultMat2D());
		ps_minimap = MaterialManager::LoadPixelShader("data/shader/ps_minimap.fx", "MiniMapPS");
		auto tex = TextureManager::CloneByName("mini_map");
		mat->SetTexture(tex, Material::TextureType::Diffuse);
		mat->SetShaderPs(ps_minimap);
		img_renderer->SetMaterial(mat);
		img_renderer->DrawType() = ImageRenderer::CLAMP;

		map_img_renderer = img_renderer;

		transform->scale = { MAP_RADIUS * 2, MAP_RADIUS * 2, 1 };
		transform->position = { 30, -30, 0 };
		anchor_type = UIObject::ANCHOR_TYPE::LEFT_TOP;
		canvas_anchor_type = UIObject::ANCHOR_TYPE::LEFT_TOP;
		if (ps_minimap) {
			Vector2 minimap_screen_pos = { transform->position.x, -transform->position.y };
			minimap_screen_pos += { transform->scale.x * 0.5f, transform->scale.y * 0.5f };
			ps_minimap->SetValue<float>("CB_PS_MINIMAP.MINIMAP_RADIUS", &MAP_RADIUS);
			ps_minimap->SetValue<Vector2>("CB_PS_MINIMAP.MINIMAP_SCREEN_POS", &minimap_screen_pos);
		}

		AddComponent<MiniMapPositionRenderer>();
		return Super::Init();

	}
	void MiniMapObject::Update()
	{

		if (player) {
			Vector3 pl_pos = player->transform->position;
			pl_pos.y = 0;
			Vector2 img_offset = { pl_pos.x - MAP_X_CENTER, -(pl_pos.z - MAP_Z_CENTER) };
			img_offset.x = std::clamp(img_offset.x, -590.0f, 590.0f);
			img_offset.y = std::clamp(img_offset.y, -590.0f, 590.0f);
			map_img_renderer->ImageOffset() = img_offset;

		}
		if (ps_minimap) {
			Vector2 minimap_screen_pos = { transform->position.x, -transform->position.y };
			minimap_screen_pos += { transform->scale.x * 0.5f, transform->scale.y * 0.5f };
			ps_minimap->SetValue<float>("CB_PS_MINIMAP.MINIMAP_RADIUS", &MAP_RADIUS);
			ps_minimap->SetValue<float>("CB_PS_MINIMAP.MINIMAP_BORDER", &MAP_BORDER);
			ps_minimap->SetValue<Vector2>("CB_PS_MINIMAP.MINIMAP_SCREEN_POS", &minimap_screen_pos);
		}


	}
	void MiniMapObject::LateDebugDraw()
	{


	}
}