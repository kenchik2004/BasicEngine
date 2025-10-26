#pragma once
#include <System/Component.h>
USING_PTR(UIObject);
class ImageRenderer : public Component
{
public:
	USING_SUPER(ImageRenderer);
	int Init() override;
	void Update() override;
	void LateDraw() override;

	void        DrawMain();
	inline void SetMaterial(Material* mat)
	{
		material = mat;
	}
	float2                  GetImageSize();
	float                     alpha = 1.0f;

	void Exit() override;
	enum DRAW_TYPE
	{
		DEFAULT,
		CLAMP,
		EXTEND,
	};

	inline DRAW_TYPE& DrawType() { return draw_type; }

private:
	UIObjectP ui_owner = nullptr;
	DRAW_TYPE   draw_type = DEFAULT;
	Material* material = nullptr;
};
