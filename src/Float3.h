#pragma once

//---------------------------------------------------------------------------------
//	float3 クラス（ x y z があるクラス）
//---------------------------------------------------------------------------------
class float3
{
public:
	float	x;
	float	y;
	float	z;

	float3();									//	コンストラクタ
	float3( float x, float y, float z );
	float3( VECTOR v );

	void clear();								//	ゼロ初期化
	void set( float x, float y, float z );		//	値のセット
	void set( float3 &v );

	float GetLength();							//	長さ取得
	void normalize();							//	正規化
	void SetLength( float length );				//	長さ設定

	float3& operator = ( const float3 &v );	//	= 演算子のオーバーロード

	float3& operator += ( const float3 &v );	//	+= 演算子のオーバーロード
	float3& operator -= ( const float3 &v );	//	-= 演算子のオーバーロード
	float3& operator *= ( const float f );		//	*= 演算子のオーバーロード
	float3& operator /= ( const float f );		//	/= 演算子のオーバーロード

	VECTOR VGet();								//	DXライブラリで使う VECTOR を返す
	void VSet( VECTOR v );						//	DXライブラリで使う VECTOR の値を取得して設定
};

//	+ 演算子のオーバーロード
float3 operator + ( const float3 &v1, const float3 &v2 );
//	- 演算子のオーバーロード
float3 operator - ( const float3 &v1, const float3 &v2 );
//	* 演算子のオーバーロード
float3 operator * ( const float3 &v, const float f );
//	/ 演算子のオーバーロード
float3 operator / ( const float3 &v, const float f );

//	２つの float3 の距離を求める
float GetFloat3Distance( float3 &pos1, float3 &pos2 );
//	２つの float3 の内積を求める
float GetFloat3Dot( float3 &v1, float3 &v2 );
//	２つの float3 の外積を求める
float3 GetFloat3Cross( float3 &v1, float3 &v2 );
//	float3 を MATRIX で変換したものを返す
float3 GetFloat3VTransform( float3 &v, MATRIX &mat );
