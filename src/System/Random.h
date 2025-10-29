#pragma once
namespace Random
{

	//初期化
	int Init();

	//内部乱数生成（PCG-XSH-RR アルゴリズム）
	unsigned int Generate();

	//乱数生成
	int Int(const int& min, const int& max);

	//乱数生成(float) (0.0f, 1.0f)
	float Float01();


	//乱数生成(float)
	float Range(const float& min, const float& max);


	//乱数生成(Vector2)
	Vector2 Position(const Vector2& min, const Vector2& max);


	//乱数生成(Vector3)
	Vector3 Position(const Vector3& min, const Vector3& max);


	//乱数生成(Color)
	::Color Color(const ::Color& min, const ::Color& max);


	//乱数生成(Quaternion)
	Quaternion Rotation(const Quaternion& min, const Quaternion& max);

};

