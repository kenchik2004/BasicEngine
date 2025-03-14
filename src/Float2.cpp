#include "float2.h"

//---------------------------------------------------------------------------------
//	コンストラクタ
//---------------------------------------------------------------------------------
float2::float2()
{
	x = 0.0f;
	y = 0.0f;
}
float2::float2( float x, float y )
{
	this->x = x;
	this->y = y;
}
//---------------------------------------------------------------------------------
//	ゼロ初期化
//---------------------------------------------------------------------------------
void float2::clear()
{
	x = 0.0f;
	y = 0.0f;
}
//---------------------------------------------------------------------------------
//	値のセット
//---------------------------------------------------------------------------------
void float2::set( float x, float y )
{
	this->x = x;
	this->y = y;
}
//---------------------------------------------------------------------------------
//	値のセット
//---------------------------------------------------------------------------------
void float2::set( float2 &v )
{
	x = v.x;
	y = v.y;
}
//---------------------------------------------------------------------------------
//	長さ取得
//---------------------------------------------------------------------------------
float float2::GetLength()
{
	return sqrtf( x * x + y * y );
}
//---------------------------------------------------------------------------------
//	正規化（ベクトルの長さを１にする）
//---------------------------------------------------------------------------------
void float2::normalize()
{
	float length = GetLength();
	if( length > 0.0f ){
		x = x / length;
		y = y / length;
	}
}
float2 float2::normalized()
{
	float2 vec = *this;
	vec.normalize();
	return vec;
}
//---------------------------------------------------------------------------------
//	長さ設定
//---------------------------------------------------------------------------------
void float2::SetLength( float length )
{
	normalize();
	x = x * length;
	y = y * length;
}

//---------------------------------------------------------------------------------
//	= 演算子のオーバーロード
//---------------------------------------------------------------------------------
float2& float2::operator = ( const float2 &v )
{
	x = v.x;
	y = v.y; 
	return *this; 
}
//---------------------------------------------------------------------------------
//	+= 演算子のオーバーロード
//---------------------------------------------------------------------------------
float2& float2::operator += ( const float2 &v )
{
	x += v.x;
	y += v.y;
	return *this;
}
//---------------------------------------------------------------------------------
//	-= 演算子のオーバーロード
//---------------------------------------------------------------------------------
float2& float2::operator -= ( const float2 &v )
{
	x -= v.x;
	y -= v.y;
	return *this;
}
//---------------------------------------------------------------------------------
//	*= 演算子のオーバーロード
//---------------------------------------------------------------------------------
float2& float2::operator *= ( const float f )
{
	x *= f;
	y *= f;
	return *this;
}
//---------------------------------------------------------------------------------
//	/= 演算子のオーバーロード
//---------------------------------------------------------------------------------
float2& float2::operator /= ( const float f )
{
	x /= f;
	y /= f;
	return *this;
}

//---------------------------------------------------------------------------------
//	+ 演算子のオーバーロード
//---------------------------------------------------------------------------------
float2 operator + ( const float2 &v1, const float2 &v2 )
{
	float2 v;
	v.x = v1.x + v2.x;
	v.y = v1.y + v2.y;
	return v;
}
//---------------------------------------------------------------------------------
//	- 演算子のオーバーロード
//---------------------------------------------------------------------------------
float2 operator - ( const float2 &v1, const float2 &v2 )
{
	float2 v;
	v.x = v1.x - v2.x;
	v.y = v1.y - v2.y;
	return v;
}
//---------------------------------------------------------------------------------
//	* 演算子のオーバーロード
//---------------------------------------------------------------------------------
float2 operator * ( const float2 &v, const float f )
{
	float2 r;
	r.x = v.x * f;
	r.y = v.y * f;
	return r;
}
//---------------------------------------------------------------------------------
//	/ 演算子のオーバーロード
//---------------------------------------------------------------------------------
float2 operator / ( const float2 &v, const float f )
{
	float2 r;
	r.x = v.x / f;
	r.y = v.y / f;
	return r;
}

//---------------------------------------------------------------------------------
//	２つの float2 の距離を求める
//---------------------------------------------------------------------------------
float GetFloat2Distance( float2 &pos1, float2 &pos2 )
{
	float2 v = pos1 - pos2;
	return v.GetLength();
}
//---------------------------------------------------------------------------------
//	２つの float2 の内積を求める
//---------------------------------------------------------------------------------
float GetFloat2Dot( float2 &v1, float2 &v2 )
{
	return v1.x * v2.x + v1.y * v2.y;
}
//---------------------------------------------------------------------------------
//	２つの float2 の外積を求める
//---------------------------------------------------------------------------------
float GetFloat2Cross( float2 &v1, float2 &v2 )
{
	return v1.x * v2.y - v1.y * v2.x;
}
