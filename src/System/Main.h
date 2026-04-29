//---------------------------------------------------------------------------
//! @file   Main.h
//! @brief  アプリケーションのエントリポイントおよびグローバルユーティリティ関数の宣言
//---------------------------------------------------------------------------
#pragma once

//! @file Main.h
//! @brief アプリケーションのエントリポイントおよびグローバルユーティリティ関数の宣言

//! @brief ウィンドウの幅
extern int SCREEN_W;
//! @brief ウィンドウの高さ
extern int SCREEN_H;

//! @brief degree角をradian角に変換する
//! @param degree degree角(0~360で表される)
//! @return float radian角(0~2πで表される)
float TO_RADIAN(float degree);

//! @brief radian角をdegree角に変換する
//! @param radian radian角(0~2πで表される)
//! @return float degree角(0~360で表される)
float TO_DEGREE(float radian);

//! @brief XZ平面における円を描画する
//! @param center 円の中心座標
//! @param radius 円の半径
//! @param color 円の色
//! @param fill 円を塗りつぶすかどうか(trueで塗りつぶす、falseで線のみ)
void DrawCircle3D_XZ(float3 center, float radius, int color, bool fill = false);

//! @brief XZ平面における矩形を描画する
//! @param center 矩形の中心座標
//! @param half_w 横幅の半分
//! @param half_h 高さの半分
//! @param color 矩形の色
//! @param fill 矩形を塗りつぶすかどうか(trueで塗りつぶす、falseで線のみ)
void DrawBox3D_XZ(float3 center, float half_w, float half_h, int color, bool fill = false);

