#include "precompile.h"
#include "Transform.h"
#include "algorithm"

void Transform::AddRotation(Vector3 euler_angles)
{
	Quaternion qx(DEG2RAD(euler_angles.x), Vector3(1, 0, 0));
	Quaternion qy(DEG2RAD(euler_angles.y), Vector3(0, 1, 0));
	Quaternion qz(DEG2RAD(euler_angles.z), Vector3(0, 0, 1));
	rotation *= qx * qy * qz;

}
void Transform::AddRotation(Quaternion q)
{
	rotation *= q;
}

void Transform::SetRotation(Vector3 euler_angles)
{
	Quaternion qx(DEG2RAD(euler_angles.x), Vector3(1, 0, 0));
	Quaternion qy(DEG2RAD(euler_angles.y), Vector3(0, 1, 0));
	Quaternion qz(DEG2RAD(euler_angles.z), Vector3(0, 0, 1));
	rotation = qx * qy * qz;
}
void Transform::SetRotation(Quaternion q)
{
	rotation = q;
}

Vector3 Transform::AxisX()
{
	float3 vec = rotation.getBasisVector0();
	return vec;
}

Vector3 Transform::AxisY()
{
	float3 vec = rotation.getBasisVector1();
	return vec;
}

Vector3 Transform::AxisZ()
{
	float3 vec = rotation.getBasisVector2();
	return vec;

}

void Transform::SetAxisX(Vector3 target) {
	float3 right = { 1.0f, 0.0f, 0.0f }; // デフォルトの前方 (+Z)
	float3 targetNorm = target;
	targetNorm.normalize(); // 正規化

	float3 axis = GetFloat3Cross(right, targetNorm); // 回転軸
	float dot = GetFloat3Dot(right, targetNorm); // 内積
	float angle = acosf(dot); // 角度（ラジアン）

	// 回転が不要な場合（すでに一致している）
	if (fabs(dot - 1.0f) < FLT_EPSILON) {
		rotation *= { 1, 0, 0, 0 }; // 単位クォータニオン
	}

	axis.normalize(); // 軸を正規化
	rotation = Quaternion(angle, axis);
}

void Transform::SetAxisY(Vector3 target) {
	float3 up = { 0.0f, 1.0f, 0.0f }; // デフォルトの前方 (+Z)
	float3 targetNorm = target;
	targetNorm.normalize(); // 正規化

	float3 axis = GetFloat3Cross(up, targetNorm); // 回転軸
	float dot = GetFloat3Dot(up, targetNorm); // 内積
	float angle = acosf(dot); // 角度（ラジアン）

	// 回転が不要な場合（すでに一致している）
	if (fabs(dot - 1.0f) < FLT_EPSILON) {
		rotation *= { 1, 0, 0, 0 }; // 単位クォータニオン
	}

	axis.normalize(); // 軸を正規化
	rotation = Quaternion(angle, axis);
}
void Transform::SetAxisZ(Vector3 target) {
	float3 forward = { 0.0f, 0.0f, 1.0f }; // デフォルトの前方 (+Z)
	float3 targetNorm = target;
	targetNorm.normalize(); // 正規化

	float3 axis = GetFloat3Cross(forward, targetNorm); // 回転軸
	float dot = GetFloat3Dot(forward, targetNorm); // 内積
	float angle = acosf(dot); // 角度（ラジアン）

	// 回転が不要な場合（すでに一致している）
	if (fabs(dot - 1.0f) < FLT_EPSILON) {
		rotation *= { 1, 0, 0, 0 }; // 単位クォータニオン
	}

	axis.normalize(); // 軸を正規化
	rotation = Quaternion(angle, axis);
}