//----------------------------------------------------------------------------
//!	@file	Color.cpp
//!	@brief	Color クラスの静的定数定義。様々な色の定数を初期化。
//!　@author　岩野 健太郎
//----------------------------------------------------------------------------

#include "Color.h"

const Color Color::BLACK = { 0.0f,0.0f,0.0f };			//< @brief 黒色の定義。RGB 全て 0.0f。
const Color Color::WHITE = { 1.0f,1.0f,1.0f };			//< @brief 白色の定義。RGB 全て 1.0f。 
const Color Color::RED = { 1.0f,0.0f,0.0f };			//< @brief 赤色の定義。赤 1.0f、他 0.0f。 
const Color Color::GREEN = { 0.0f,1.0f,0.0f };			//< @brief 緑色の定義。緑 1.0f、他 0.0f。 
const Color Color::BLUE = { 0.0f,0.0f,1.0f };			//< @brief 青色の定義。青 1.0f、他 0.0f。 
const Color Color::YELLOW = { 1.0f,1.0f,0.0f };			//< @brief 黄色の定義。赤と緑 1.0f、青 0.0f。 
const Color Color::CYAN = { 0.0f,1.0f,1.0f };			//< @brief シアンの定義。緑と青 1.0f、赤 0.0f。 
const Color Color::MAGENTA = { 1.0f,0.0f,1.0f };		//< @brief マゼンタの定義。赤と青 1.0f、緑 0.0f。 
const Color Color::GRAY = { 0.5f,0.5f,0.5f };			//< @brief 灰色の定義。RGB 全て 0.5f。 
const Color Color::DARK_RED = { 0.5f,0.0f,0.0f };		//< @brief ダークレッドの定義。赤 0.5f、他 0.0f。 
const Color Color::DARK_GREEN = { 0.0f,0.4f ,0.0f };	//< @brief ダークグリーンの定義。緑 0.4f、他 0.0f。 
const Color Color::DARK_BLUE = { 0.0f,0.0f,0.545f };	//< @brief ダークブルーの定義。青 0.545f、他 0.0f。 
const Color Color::ORANGE = { 1.0f,0.65f,0.0f };		//< @brief オレンジの定義。赤 1.0f、緑 0.65f、青 0.0f。 
const Color Color::PINK = { 1.0f,0.75f,0.8f };			//< @brief ピンクの定義。赤 1.0f、緑 0.75f、青 0.8f。 
const Color Color::BROWN = { 0.65f,0.165f,0.165f };		//< @brief 茶色の定義。赤 0.65f、緑 0.165f、青 0.165f。 
const Color Color::PURPLE = { 0.5f,0.0f,0.5f };			//< @brief 紫色の定義。赤と青 0.5f、緑 0.0f。 