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
	float3(float x, float y, float z);
	float3(VECTOR v);
	float3(Vector3 v);

	void clear();								//	ゼロ初期化
	void set(float x, float y, float z);		//	値のセット
	void set(float3& v);

	float GetLength();							//	長さ取得
	void normalize();							//	正規化
	float3 normalized();						//	正規化されたベクトルの取得
	void SetLength(float length);				//	長さ設定

	float3& operator = (const float3& v);	//	= 演算子のオーバーロード
	float3& operator = (const VECTOR& v);	//	= 演算子のオーバーロード
	float3& operator = (const Vector3& v);	//	= 演算子のオーバーロード

	float3& operator += (const float3& v);	//	+= 演算子のオーバーロード
	float3& operator -= (const float3& v);	//	-= 演算子のオーバーロード
	float3& operator *= (const float f);		//	*= 演算子のオーバーロード
	float3& operator /= (const float f);		//	/= 演算子のオーバーロード
	operator VECTOR() { VECTOR v; v.x = this->x; v.y = this->y;	v.z = this->z;	return v; }
	operator Vector3() { Vector3 v; v.x = this->x; v.y = this->y;	v.z = this->z;	return v; }
};

//	+ 演算子のオーバーロード
float3 operator + (const float3& v1, const float3& v2);
//	- 演算子のオーバーロード
float3 operator - (const float3& v1, const float3& v2);
//	* 演算子のオーバーロード
float3 operator * (const float3& v, const float f);
//	/ 演算子のオーバーロード
float3 operator / (const float3& v, const float f);

//	２つの float3 の距離を求める
float GetFloat3Distance(float3& pos1, float3& pos2);
//	２つの float3 の内積を求める
float GetFloat3Dot(float3& v1, float3& v2);
//	２つの float3 の外積を求める
float3 GetFloat3Cross(float3& v1, float3& v2);
//	float3 を MATRIX で変換したものを返す
float3 GetFloat3VTransform(float3& v, MATRIX& mat);
//	float3のベクトルを平面上に投影
float3 ProjectOnPlane(float3 vec, float3 plane_normal);
// PhysXへの受け渡し用変換
inline Vector3 CastPhysXVec(const float3& v) {
	return Vector3(v.x, v.y, -v.z);
}

// PhysXへの受け渡し用変換
inline Quaternion CastPhysXQuat(const Quaternion& q) {
	return Quaternion(q.x, q.y, -q.z, -q.w);
}

inline Vector3 QuaternionToEuler(const Quaternion& q) {
	// Roll (X-axis rotation)
	float roll = atan2(2.0f * (q.w * q.x + q.y * q.z), 1.0f - 2.0f * (q.x * q.x + q.y * q.y));

	// Pitch (Y-axis rotation)
	float pitch = asin(2.0f * (q.w * q.y - q.z * q.x));

	// Yaw (Z-axis rotation)
	float yaw = atan2(2.0f * (q.w * q.z + q.x * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z));

	return Vector3(roll, pitch, yaw);  // X, Y, Z に対応するオイラー角を返す
}