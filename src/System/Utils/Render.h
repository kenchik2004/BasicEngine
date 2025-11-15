//----------------------------------------------------------------------------
//!	@file	Render.h
//!	@brief	描画関数のDXライブラリ拡張。DirectX11 オブジェクトの取得、クリア関数、RenderTarget 設定などを提供。
//!　@author　岩野 健太郎
//----------------------------------------------------------------------------

#pragma once

//----------------------------------------------------
// @brief テクスチャクラス。
//----------------------------------------------------
class Texture;

//----------------------------------------------------
// @brief D3Dデバイスを取得する。
//----------------------------------------------------
ID3D11Device* GetD3DDevice();

//----------------------------------------------------
// @brief D3Dデバイスコンテキストを取得する。
//----------------------------------------------------
ID3D11DeviceContext* GetD3DDeviceContext();

//----------------------------------------------------
// @brief 現在のカラーバッファをクリアする。
// @param clear_color クリアするカラー値。
//----------------------------------------------------
void ClearColor(const Color& clear_color);

//----------------------------------------------------
// @brief 現在のデプスバッファをクリアする。
// @param depth_value クリアするデプス値。
//----------------------------------------------------
void ClearDepth(f32 depth_value);

//----------------------------------------------------
// @brief 現在のステンシルバッファをクリアする。
// @param stencil_value クリアするステンシル値。
//----------------------------------------------------
void ClearStencil(u8 stencil_value);

//----------------------------------------------------
// @brief カラーバッファをクリアする。
// @param texture 対象のカラーバッファ。
// @param clear_color クリアするカラー値。
//----------------------------------------------------
void ClearColor(const Texture* texture, const Color& clear_color);

//----------------------------------------------------
// @brief デプスバッファをクリアする。
// @param texture 対象のデプスバッファ。
// @param depth_value クリアするデプス値。
//----------------------------------------------------
void ClearDepth(const Texture* texture, f32 depth_value);

//----------------------------------------------------
// @brief ステンシルバッファをクリアする。
// @param texture 対象のデプスバッファ。
// @param stencil_value クリアするステンシル値。
//----------------------------------------------------
void ClearStencil(const Texture* texture, u8 stencil_value);

//----------------------------------------------------
// @brief RenderTarget情報。
//----------------------------------------------------
struct TargetDesc {
	u32                                                          color_count_ = 0; // カラーターゲット数
	std::array<Texture*, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> color_targets_ = {}; // カラーターゲット配列
	Texture* depth_stencil_ = nullptr; // デプスステンシル
};

//----------------------------------------------------
// @brief 現在のRenderTarget情報を取得する。
//----------------------------------------------------
TargetDesc GetRenderTarget();

//----------------------------------------------------
// @brief RenderTargetを一括設定 (TargetDesc指定)。
		/// @param desc RenderTarget情報。
//----------------------------------------------------
void SetRenderTarget(const TargetDesc& desc);

//----------------------------------------------------
// @brief RenderTargetを設定 (単体)。
		/// @param color_target カラーターゲットの先頭アドレス。
		/// @param depth_stencil デプスバッファ(nullptr指定で無効化)。
//----------------------------------------------------
void SetRenderTarget(Texture* color_target, Texture* depth_stencil = nullptr);

//----------------------------------------------------
// @brief RenderTargetを設定 (Multiple Render Target)。
		/// @param color_count カラーターゲット数(1～8)。
		/// @param color_targets カラーターゲット配列の先頭アドレス。
		/// @param depth_stencil デプスバッファ(nullptr指定で無効化)。
//----------------------------------------------------
void SetRenderTarget(u32 color_count, Texture** color_targets, Texture* depth_stencil = nullptr);

//----------------------------------------------------
// @brief テクスチャを設定する。
// @param slot テクスチャSlot番号(t16～)。
// @param texture 設定するテクスチャ(nullptrの場合は解除)。
//----------------------------------------------------
void SetTexture(u32 slot, Texture* texture);

//----------------------------------------------------
// @brief デフォルトのバックバッファを取得(R8G8B8A8_UNORM)。
//----------------------------------------------------
Texture* GetBackBuffer();

//----------------------------------------------------
// @brief デフォルトのデプスバッファを取得(D32_FLOAT)。
//----------------------------------------------------
Texture* GetDepthStencil();

//----------------------------------------------------
// @brief ピクセルシェーダーを走らせてRenderTargetを塗りつぶす(事前にSRV設定必須)。
		/// @param shader_ps_handle カスタムのピクセルシェーダー (-1の場合は単純コピー)。
//----------------------------------------------------
void FillRenderTarget(int shader_ps_handle);

//----------------------------------------------------
// @brief RenderTargetにイメージをコピーする。
		/// @param dst_render_target 出力先RenderTarget。
		/// @param src_texture コピー元テクスチャ。
		/// @param shader_ps_handle カスタムのピクセルシェーダー (-1の場合は単純コピー)。
//----------------------------------------------------
void CopyToRenderTarget(Texture* dst_render_target, Texture* src_texture, int shader_ps_handle = -1);

//----------------------------------------------------
// @brief Render初期化。
//----------------------------------------------------
bool RenderInit();

//----------------------------------------------------
// @brief Render終了。
//----------------------------------------------------
void RenderExit();
