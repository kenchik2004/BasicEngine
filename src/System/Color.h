#pragma once
class Color final
{
	unsigned int color = 0x000000;	//色情報
public:
	Color() :color(0x000000) {}
	Color(unsigned int color_uint) { color = color_uint; }
	Color(const Color& other) { color = other.color; }
	Color(unsigned char red, unsigned char green, unsigned char blue)
	{
		color = ((unsigned int)red) << 16;
		color |= ((unsigned int)green) << 8;
		color |= ((unsigned int)blue);
	}

	operator unsigned int() { return color; }

	//各色のGetColorを使用せずに扱うための定数(16進数 unsigned int)
	static constexpr unsigned int BLACK = 0x000000;	//黒
	static constexpr unsigned int WHITE = 0xFFFFFF;	//白
	static constexpr unsigned int RED = 0xFF0000;	//赤
	static constexpr unsigned int GREEN = 0x00FF00;	//緑
	static constexpr unsigned int BLUE = 0x0000FF;	//青
	static constexpr unsigned int YELLOW = 0xFFFF00;	//黄
	static constexpr unsigned int CYAN = 0x00FFFF;	//シアン
	static constexpr unsigned int MAGENTA = 0xFF00FF;	//マゼンタ
	static constexpr unsigned int GRAY = 0x808080;	//灰
	static constexpr unsigned int DARK_RED = 0x800000;	//ダークレッド
	static constexpr unsigned int DARK_GREEN = 0x006400;	//ダークグリーン
	static constexpr unsigned int DARK_BLUE = 0x00008B;	//ダークブルー
	static constexpr unsigned int ORANGE = 0xFFA500;	//オレンジ
	static constexpr unsigned int PINK = 0xFFC0CB;	//ピンク
	static constexpr unsigned int BROWN = 0xA52A2A;	//茶
	static constexpr unsigned int PURPLE = 0x800080;	//紫
};

