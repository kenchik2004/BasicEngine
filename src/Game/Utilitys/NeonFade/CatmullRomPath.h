#pragma once
#include <vector>
#include <cmath>
#include <algorithm>

using namespace physx;

// ============================
// Path Class
// ============================
class CatmullRomPath {
public:
    std::vector<Vector3> controlPoints; // アンカー
    bool loop = true;

private:
    struct SegmentInfo {
        float length; // セグメント長
        std::vector<float> tSamples;  // LUT: パラメータt
        std::vector<float> sSamples;  // LUT: 累積距離
    };
    std::vector<SegmentInfo> segments;
    float totalLength = 0.0f;

public:
    void SetPoints(const std::vector<Vector3>& pts, bool looped = true) {
        controlPoints = pts;
        loop = looped;
        BuildArcLengthTable();
    }

    // ============================
    // 中心的な関数
    // ============================

    // 距離s(0～総距離)に対する座標・向きを取得
    void Evaluate(float s, Vector3& outPos, Quaternion& outRot, bool uniformSpeed = true, const Vector3& worldUp = Vector3(0, 1, 0)) const;

    float GetTotalLength() const { return totalLength; }

private:
    // ============================
    // 内部関数群
    // ============================

    static Vector3 CatmullRomPos(const Vector3& P0, const Vector3& P1, const Vector3& P2, const Vector3& P3, float t) {
        float t2 = t * t;
        float t3 = t2 * t;
        return 0.5f * (
            (2.0f * P1) +
            (-P0 + P2) * t +
            (2.0f * P0 - 5.0f * P1 + 4.0f * P2 - P3) * t2 +
            (-P0 + 3.0f * P1 - 3.0f * P2 + P3) * t3
            );
    }

    static Vector3 CatmullRomTangent(const Vector3& P0, const Vector3& P1, const Vector3& P2, const Vector3& P3, float t) {
        float t2 = t * t;
        return 0.5f * (
            (-P0 + P2) +
            (4.0f * P0 - 10.0f * P1 + 8.0f * P2 - 2.0f * P3) * t +
            (-3.0f * P0 + 9.0f * P1 - 9.0f * P2 + 3.0f * P3) * t2
            );
    }

    // 左手Y-up用 LookRotation
    static Quaternion LookRotationLH(const Vector3& forward, const Vector3& upHint);

    // centripetal parameter化
    static float Param(float prev, const Vector3& Pi, const Vector3& Pj) {
        float a = (Pi - Pj).magnitude();
        return prev + powf(a, 0.5f); // α = 0.5
    }

    void BuildArcLengthTable();

    static float GetTFromArcLength(const SegmentInfo& seg, float s);
};


