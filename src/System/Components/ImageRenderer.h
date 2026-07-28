//---------------------------------------------------------------------------
//! @file   ImageRenderer.h
//! @brief  2D画像の描画を管理するImageRendererコンポーネント
//---------------------------------------------------------------------------
#pragma once
USING_PTR(UIObject);
//---------------------------------------------------------------------
//! @class ImageRenderer
//! @brief UIオブジェクトに2D画像を描画するコンポーネント
//---------------------------------------------------------------------
class ImageRenderer : public Component
{
public:
	USING_SUPER(ImageRenderer);
	//! @brief 初期化処理
	int Init() override;
	//! @brief 更新処理
	void Update() override;
	//! @brief 描画処理（後処理）
	void LateDraw() override;

	//! @brief メイン描画を行う
	void        DrawMain();
	//! @brief 描画に使用するマテリアルを設定する
	inline void SetMaterial(Material* mat)
	{
		material = mat;
	}
	//! @brief 画像のサイズを取得する
	Vector2                  GetImageSize();
	float                     alpha = 1.0f; //!< 透明度（0.0〜1.0）

	//! @brief 終了処理
	void Exit() override;
	//! @brief 描画方式の種別
	enum DRAW_TYPE
	{
		CLAMP,   //!< クランプ描画
		EXTEND,  //!< 拡張描画
	};

	//! @brief 描画方式への参照を返す
	inline DRAW_TYPE& DrawType() { return draw_type; }

	inline Vector2& ImageOffset() { return image_offset; } //!< 画像のオフセット座標への参照を返す

private:
	UIObjectWP ui_owner = nullptr;    //!< 所有するUIオブジェクトへのポインタ
	DRAW_TYPE   draw_type = EXTEND; //!< 描画方式
	Material* material = nullptr;    //!< 使用するマテリアル
	Vector2 image_offset = { 0.0f, 0.0f }; //!< 画像のオフセット座標
};
