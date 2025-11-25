#include "precompile.h"
#include "CatmullRomPath.h"

// 距離s(0～総距離)に対する座標・向きを取得
void CatmullRomPath::Evaluate(float s, Vector3& outPos, PxQuat& outRot, bool uniformSpeed, const Vector3& worldUp) const {
	if (controlPoints.size() < 4) return;
	if (loop)
		s = fmodf(s, totalLength);
	else
		s = std::clamp(s, 0.0f, totalLength);

	// セグメント探索
	int segIdx = 0;
	float segStartDist = 0.0f;
	for (int i = 0; i < (int)segments.size(); i++) {
		if (s < segStartDist + segments[i].length) {
			segIdx = i;
			break;
		}
		segStartDist += segments[i].length;
	}

	const SegmentInfo& seg = segments[segIdx];
	float localS = s - segStartDist;
	float t = localS / seg.length;

	if (uniformSpeed) {
		// Arc-Length LUT から補正
		t = GetTFromArcLength(seg, localS);
	}

	// 制御点を取得（ループ考慮）
	auto Get = [&](int i) -> Vector3 {
		int N = (int)controlPoints.size();
		return controlPoints[(i + N) % N];
		};

	Vector3 p0 = Get(segIdx - 1);
	Vector3 p1 = Get(segIdx + 0);
	Vector3 p2 = Get(segIdx + 1);
	Vector3 p3 = Get(segIdx + 2);

	outPos = CatmullRomPos(p0, p1, p2, p3, t);
	Vector3 tangent = CatmullRomTangent(p0, p1, p2, p3, t);
	tangent.normalize();

	outRot = LookRotationLH(tangent, worldUp);
}

// 左手Y-up用 LookRotation
PxQuat CatmullRomPath::LookRotationLH(const Vector3& forward, const Vector3& upHint) {
	Vector3 f = forward.getNormalized();
	Vector3 r = upHint.cross(f).getNormalized(); // left-handedなのでcross順序注意
	Vector3 u = f.cross(r); // Up再計算

	float m00 = r.x, m01 = u.x, m02 = f.x;
	float m10 = r.y, m11 = u.y, m12 = f.y;
	float m20 = r.z, m21 = u.z, m22 = f.z;

	float trace = m00 + m11 + m22;
	PxQuat q;
	if (trace > 0.0f) {
		float s = sqrtf(trace + 1.0f) * 2.0f;
		q.w = 0.25f * s;
		q.x = (m21 - m12) / s;
		q.y = (m02 - m20) / s;
		q.z = (m10 - m01) / s;
	}
	else if ((m00 > m11) && (m00 > m22)) {
		float s = sqrtf(1.0f + m00 - m11 - m22) * 2.0f;
		q.w = (m21 - m12) / s;
		q.x = 0.25f * s;
		q.y = (m01 + m10) / s;
		q.z = (m02 + m20) / s;
	}
	else if (m11 > m22) {
		float s = sqrtf(1.0f + m11 - m00 - m22) * 2.0f;
		q.w = (m02 - m20) / s;
		q.x = (m01 + m10) / s;
		q.y = 0.25f * s;
		q.z = (m12 + m21) / s;
	}
	else {
		float s = sqrtf(1.0f + m22 - m00 - m11) * 2.0f;
		q.w = (m10 - m01) / s;
		q.x = (m02 + m20) / s;
		q.y = (m12 + m21) / s;
		q.z = 0.25f * s;
	}
	q.normalize();
	return q;
}

void CatmullRomPath::BuildArcLengthTable() {
	segments.clear();
	totalLength = 0.0f;
	if (controlPoints.size() < 4) return;

	const int samplesPerSegment = 32; // LUT密度

	int N = (int)controlPoints.size();
	for (int i = 0; i < N; i++) {
		SegmentInfo seg{};
		seg.length = 0.0f;

		auto Get = [&](int k) -> Vector3 {
			return controlPoints[(k + N) % N];
			};

		Vector3 p0 = Get(i - 1);
		Vector3 p1 = Get(i);
		Vector3 p2 = Get(i + 1);
		Vector3 p3 = Get(i + 2);

		Vector3 prev = p1;
		seg.tSamples.push_back(0.0f);
		seg.sSamples.push_back(0.0f);

		for (int j = 1; j <= samplesPerSegment; j++) {
			float t = (float)j / samplesPerSegment;
			Vector3 pos = CatmullRomPos(p0, p1, p2, p3, t);
			seg.length += (pos - prev).magnitude();
			seg.tSamples.push_back(t);
			seg.sSamples.push_back(seg.length);
			prev = pos;
		}

		totalLength += seg.length;
		segments.push_back(seg);
	}
}

float CatmullRomPath::GetTFromArcLength(const SegmentInfo& seg, float s) {
	if (s <= 0) return 0;
	if (s >= seg.length) return 1;

	for (size_t i = 1; i < seg.sSamples.size(); i++) {
		if (s < seg.sSamples[i]) {
			float s0 = seg.sSamples[i - 1];
			float s1 = seg.sSamples[i];
			float t0 = seg.tSamples[i - 1];
			float t1 = seg.tSamples[i];
			float alpha = (s - s0) / (s1 - s0);
			return t0 + alpha * (t1 - t0);
		}
	}
	return 1.0f;
}
