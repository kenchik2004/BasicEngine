#include "precompile.h"
#include "Random.h"

namespace Random
{
	// 変数の実体定義（全翻訳単位で共有）
	static u64 seed = 0;
	static u64 inc = 1ULL;

	//初期化
	int Init()
	{
		u64 initstate = (u64)(SEC2MICRO(Time::GetOSTimeD())); //!<シード値を初期化
		inc = (initstate << 1u) | 1ULL; // 奇数であることを保証（より分散）
		// PCG初期化: 状態を適切に混ぜる
		seed = initstate;
		seed = seed * 6364136223846793005ULL + inc;
		return 0;
	}

	//内部乱数生成（PCG-XSH-RR アルゴリズム）
	unsigned int Generate()
	{
		unsigned long long oldstate = seed;
		// LCG部分: Newlib parameters
		seed = oldstate * 6364136223846793005ULL + inc;
		// XOR-shift and rotate (PCG-XSH-RR)
		unsigned int xorshifted = (unsigned int)(((oldstate >> 18u) ^ oldstate) >> 27u);
		unsigned int rot = (unsigned int)(oldstate >> 59u);
		return (xorshifted >> rot) | (xorshifted << ((~rot + 1u) & 31));
	}

	//乱数生成
	int Int(const int& min, const int& max)
	{
		if (min >= max) return min; //範囲が無効な場合は最小値を返す
		unsigned int range = (unsigned int)(max - min + 1);
		unsigned int scaled = (unsigned int)((unsigned long long)Generate() * range >> 32);
		return min + (int)scaled; //指定された範囲の乱数を返す
	}

	//乱数生成(float) (0.0f, 1.0f)
	float Float01()
	{
		return (Generate() >> 8) * 0x1.0p-24f; // 高精度な正規化
	}

	//乱数生成(float)
	float Range(const float& min, const float& max)
	{
		if (min >= max) return min; //範囲が無効な場合は最小値を返す
		return min + Float01() * (max - min); //指定された範囲の乱数を返す
	}

	//乱数生成(Vector2)
	Vector2 Position(const Vector2& min, const Vector2& max)
	{
		return Vector2(
			Range(min.x, max.x),
			Range(min.y, max.y)
		);
	}

	//乱数生成(Vector3)
	Vector3 Position(const Vector3& min, const Vector3& max)
	{
		return Vector3(
			Range(min.x, max.x),
			Range(min.y, max.y),
			Range(min.z, max.z)
		);
	}

	//乱数生成(Color)
	::Color Color(const::Color& min, const::Color& max)
	{
		return ::Color(
			Range(min.r, max.r),
			Range(min.g, max.g),
			Range(min.b, max.b),
			Range(min.a, max.a)
		);
	}

	//乱数生成(Quaternion)
	Quaternion Rotation(const Quaternion& min, const Quaternion& max)
	{
		return Quaternion(
			Range(min.x, max.x),
			Range(min.y, max.y),
			Range(min.z, max.z),
			Range(min.w, max.w)
		);
	}

}
