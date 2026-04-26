#pragma once

//! @file Main.h
//! @brief アプリケーションのメインヘッダファイル

//! @brief 画面の横幅
extern int SCREEN_W;
//! @brief 画面の縦幅
extern int SCREEN_H;

//! @brief 度数法から弧度法(ラジアン)への変換
//! @param degree 度数法での角度
//! @return ラジアンでの角度
float TO_RADIAN(float degree);

//! @brief 弧度法(ラジアン)から度数法への変換
//! @param radian ラジアンでの角度
//! @return 度数法での角度
float TO_DEGREE(float radian);

//! @brief XZ平面上に3Dの円を描画する
//! @param center 円の中心座標
//! @param radius 円の半径
//! @param color 描画色
//! @param fill 塗りつぶしフラグ (true: 塗りつぶす, false: 枠のみ)
void DrawCircle3D_XZ(float3 center, float radius, int color, bool fill = false);

//! @brief XZ平面上に3Dの矩形を描画する
//! @param center 矩形の中心座標
//! @param half_w 矩形の横幅の半分
//! @param half_h 矩形の縦幅の半分
//! @param color 描画色
//! @param fill 塗りつぶしフラグ (true: 塗りつぶす, false: 枠のみ)
void DrawBox3D_XZ(float3 center, float half_w, float half_h, int color, bool fill = false);

