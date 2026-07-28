#pragma once

// 古いC言語のセキュリティ警告（strcpy等の使用に対する警告）を無効化します。
#define _CRT_SECURE_NO_WARNINGS

//---------------------------------------------------------------------------
// C++標準ライブラリ（STL）のインクルード
//---------------------------------------------------------------------------
#include <windows.h>      
#include <iostream>       
#include <time.h>         
#include <math.h>         
#include <memory>         
#include <vector>         
#include <array>          
#include <functional>     
#include <algorithm>      
#include "chrono"         

// 特定のコンパイラ警告（UTF-8などの文字コードに関する警告 4819）を一時的に抑止
#pragma warning (disable : 4819)
// Windowsのマルチメディア関連APIライブラリをリンク
#pragma comment(lib, "winmm.lib")

//---------------------------------------------------------------------------
// 3D物理演算エンジン NVIDIA PhysX 関連
//---------------------------------------------------------------------------
// 初期化されていないメンバ変数に関するコンパイラ警告（26495）をPhysXヘッダー読み込み中のみ無視
#pragma warning (disable : 26495)
// C++20における古い言語仕様の廃止に伴う警告を無視
#define _SILENCE_CXX20_CISO646_REMOVED_WARNING
#include "PxPhysicsAPI.h" // PhysXの全機能にアクセスするためのメインヘッダー

// PhysXを動作させるために必要な静的・動的ライブラリ（.lib）をコンパイラにリンク指示
#pragma comment(lib, "PhysX_64.lib")
#pragma comment(lib, "PhysXCommon_64.lib")
#pragma comment(lib, "PhysXCooking_64.lib")
#pragma comment(lib, "PhysXExtensions_static_64.lib")
#pragma comment(lib, "PhysXFoundation_64.lib")
#pragma comment(lib, "PhysXPvdSDK_static_64.lib")
#pragma comment(lib, "PhysXTask_static_64.lib")
#pragma comment(lib, "SceneQuery_static_64.lib")
#pragma comment(lib, "SimulationController_static_64.lib")
// 警告設定を元の状態に戻す
#pragma warning (default : 26495)

//---------------------------------------------------------------------------
// DirectX 11 / 描画エンジン関連
//---------------------------------------------------------------------------
#include <d3d11.h>        // Microsoft DirectX 11の描画コアAPI

//---------------------------------------------------------------------------
// ゲームエンジン独自の共通定義・汎用ヘッダー
//---------------------------------------------------------------------------
#include "System/typedef.h"                     // 独自の型定義やエイリアス（マクロや短縮型名）
#include "System/SystemHeaders/Utilitys.h"      // 汎用ユーティリティ関数（数学補助、デバッグ等）
#include "System/Managers/PhysicsManager.h"     // 物理エンジン(PhysX)を管理するクラス

//---------------------------------------------------------------------------
// DxLib（ＤＸライブラリ）のインクルード
// Windowsゲーム作成を3D/2D描画、サウンド、入力など含めて強力にサポートするライブラリ
//---------------------------------------------------------------------------
#include <DxLib.h>

// 独自定義の2次元・3次元数学ベクトルクラス
#include "float2.h"
#include "float3.h"

//---------------------------------------------------------------------------
// ゲームシーン・ゲームオブジェクト・コンポーネントシステム
// UnityやUnreal Engineのように、オブジェクトにコンポーネント（機能）を貼り付ける
// 設計思想に基づいたゲームエンジンの主要ヘッダー群
//---------------------------------------------------------------------------
#include "System/SystemHeaders/Components.h"   // 各種コンポーネント（レンダラー、コライダーなど）
#include "System/SystemHeaders/Objects.h"      // ゲームオブジェクト（プレイヤー、敵、カメラ等）
#include "System/SystemHeaders/Managers.h"     // システムマネージャ群（シーン、アセット、サウンド管理など）
#include "System/DontDestroyOnLoadScene.h"     // シーン遷移しても破棄されない永続オブジェクト用シーン
