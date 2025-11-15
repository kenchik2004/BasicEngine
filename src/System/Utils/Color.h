//----------------------------------------------------------------------------
//!	@file	Color.h
//!	@brief	色を表す Color クラスの定義。RGBA 値を管理し、様々な色変換と演算を提供。
//!　@author　岩野 健太郎
//----------------------------------------------------------------------------

#pragma once

//----------------------------------------------------
// @brief 色を表すクラス。RGBA の float 値で色を管理し、様々な変換と演算をサポート。
// final クラスなので継承不可。
//----------------------------------------------------
class Color final
{
public:
	float r; /**< @brief 赤成分 (0.0f から 1.0f)。 */
	float g; /**< @brief 緑成分 (0.0f から 1.0f)。 */
	float b; /**< @brief 青成分 (0.0f から 1.0f)。 */
	float a; /**< @brief アルファ成分 (0.0f から 1.0f)。 */

	//----------------------------------------------------
	// @brief 色の配列ポインタを返す。
	// @return 赤成分のポインタ。連続したメモリとして r, g, b, a にアクセス可能。
	//----------------------------------------------------
	const float* color() const { return &r; }

	//----------------------------------------------------
	// @brief デフォルトコンストラクタ。黒色 (r=0, g=0, b=0, a=1) で初期化。
	//----------------------------------------------------
	Color() :r(0), g(0), b(0), a(1) {}

	//----------------------------------------------------
	// @brief unsigned int から色を初期化するコンストラクタ。
	// @param uint_color 0xRRGGBB 形式の unsigned int 値。
	//----------------------------------------------------
	Color(const unsigned int uint_color) {
		r = float((uint_color & 0xff0000) >> 16) / 255; /**< @brief 赤成分を抽出して 0-1 に正規化。 */
		g = float((uint_color & 0x00ff00) >> 8) / 255; /**< @brief 緑成分を抽出して 0-1 に正規化。 */
		b = float(uint_color & 0x0000ff) / 255; /**< @brief 青成分を抽出して 0-1 に正規化。 */
		a = 1.0f; /**< @brief アルファは 1.0f に固定。 */
	}

	//----------------------------------------------------
	// @brief コピーコンストラクタ。他の Color から値をコピー。
	// @param other コピー元の Color オブジェクト。
	//----------------------------------------------------
	Color(const Color& other) { r = other.r; g = other.g; b = other.b; a = other.a; }

	//----------------------------------------------------
	// @brief Vector4 から色を初期化するコンストラクタ。
	// @param other Vector4 (x=r, y=g, z=b, w=a)。
	//----------------------------------------------------
	Color(const Vector4& other) { r = other.x; g = other.y; b = other.z; a = other.w; }

	//----------------------------------------------------
	// @brief RGBA 値から色を初期化するコンストラクタ。
	// @param red 赤成分。
	// @param green 緑成分。
	// @param blue 青成分。
	// @param alpha アルファ成分 (デフォルト 1.0f)。
	//----------------------------------------------------
	Color(float red, float green, float blue, float alpha = 1.0f) { r = red; g = green; b = blue; a = alpha; }

	//----------------------------------------------------
	// @brief Vector4 への変換演算子。
	// @return Vector4 (x=r, y=g, z=b, w=a)。
	//----------------------------------------------------
	operator Vector4() const { return Vector4(r, g, b, a); }

	//----------------------------------------------------
	// @brief DXLib の COLOR_F への変換演算子。
	// @return DXLib の COLOR_F 構造体。
	//----------------------------------------------------
	operator COLOR_F() const { COLOR_F dx_color; dx_color.r = r; dx_color.g = g; dx_color.b = b; dx_color.a = a; return dx_color; }

	//----------------------------------------------------
	// @brief unsigned int への変換演算子。
	// @return 0xRRGGBB 形式の unsigned int 値。
	//----------------------------------------------------
	operator unsigned int() const {
		unsigned int color;
		color = (std::clamp((unsigned int)(r * 255), (unsigned int)0, (unsigned int)255)) << 16; /**< @brief 赤成分を 0-255 にクランプしてシフト。 */
		color |= (std::clamp((unsigned int)(g * 255), (unsigned int)0, (unsigned int)255)) << 8; /**< @brief 緑成分を 0-255 にクランプしてシフト。 */
		color |= (std::clamp((unsigned int)(b * 255), (unsigned int)0, (unsigned int)255)); /**< @brief 青成分を 0-255 にクランプ。 */
		return color;
	}

	//----------------------------------------------------
	// @brief スカラー乗算演算子。
	// @param bias 乗算するスカラー値。
	// @return 各成分に bias を乗算した新しい Color。
	//----------------------------------------------------
	Color operator* (float bias) { return { r * bias,g * bias,b * bias,a * bias }; }

	//----------------------------------------------------
	// @brief Color 乗算演算子。
	// @param bias 乗算する Color。
	// @return 各成分を対応する成分で乗算した新しい Color。
	//----------------------------------------------------
	Color operator* (Color bias) { return { r * bias.r,g * bias.g,b * bias.b,a * bias.a }; }

	//----------------------------------------------------
	// @brief Vector4 乗算演算子。
	// @param bias 乗算する Vector4 (x=r, y=g, z=b, w=a)。
	// @return 各成分を対応する成分で乗算した新しい Color。
	//----------------------------------------------------
	Color operator* (Vector4 bias) { return { r * bias.x,g * bias.y,b * bias.z,a * bias.w }; }

	//各色のGetColorやGetColorFを使用せずに扱うための定数
	static const Color BLACK;		//< @brief 黒色定数。 
	static const Color WHITE;		//< @brief 白色定数。 
	static const Color RED;			//< @brief 赤色定数。 
	static const Color GREEN;		//< @brief 緑色定数。 
	static const Color BLUE;		//< @brief 青色定数。 
	static const Color YELLOW;		//< @brief 黄色定数。 
	static const Color CYAN;		//< @brief シアン色定数。
	static const Color MAGENTA;		//< @brief マゼンタ色定数。
	static const Color GRAY;		//< @brief 灰色定数。
	static const Color DARK_RED;	//< @brief ダークレッド色定数。
	static const Color DARK_GREEN;	//< @brief ダークグリーン色定数。
	static const Color DARK_BLUE;	//< @brief ダークブルー色定数。 
	static const Color ORANGE;		//< @brief オレンジ色定数。 
	static const Color PINK;		//< @brief ピンク色定数。 
	static const Color BROWN;		//< @brief 茶色定数。 
	static const Color PURPLE;		//< @brief 紫色定数。 
};


