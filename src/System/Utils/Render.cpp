//----------------------------------------------------------------------------
//!	@file	Render.cpp
//!	@brief	描画関数のDXライブラリ拡張の実装。Render 関連の関数を実装し、DirectX11 を使用した描画をサポート。
//!　@author　岩野 健太郎
//----------------------------------------------------------------------------

#include "Render.h"
#include "System/Utils/Texture.h"
#include "System/Main.h"
#include "System/Utils/Shader.h"

namespace {

	std::shared_ptr<Texture>  texture_back_buffer_;      // バックバッファ
	std::shared_ptr<Texture>  texture_depth_stencil_;    // デフォルトのデプスステンシル
	std::shared_ptr<ShaderPs> shader_ps_texture_;        // 通常コピー用のシェーダー
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> default_dss; // デフォルトのデプスステンシルステート
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> write_off_dss; // デプスステンシル無効ステート

	TargetDesc target_desc_;    // 現在のRenderTarget情報

}    // namespace

void SetWriteZBufferEnable(bool enable)
{
	if (enable) {
		GetD3DDeviceContext()->OMSetDepthStencilState(default_dss.Get(), 1);
	}
	else {
		GetD3DDeviceContext()->OMSetDepthStencilState(write_off_dss.Get(), 1);
	}
}

//----------------------------------------------------
// @brief Render初期化。
//----------------------------------------------------
bool RenderInit()
{
	// バックバッファを取得
	auto* d3d_back_buffer = static_cast<ID3D11Texture2D*>(const_cast<void*>(GetUseDirect3D11BackBufferTexture2D()));
	auto* d3d_depth_stencil = static_cast<ID3D11Texture2D*>(const_cast<void*>(GetUseDirect3D11DepthStencilTexture2D()));

	texture_back_buffer_ = std::make_shared<Texture>(d3d_back_buffer);
	texture_depth_stencil_ = std::make_shared<Texture>(d3d_depth_stencil);

	// シェーダー作成
	shader_ps_texture_ = std::make_shared<ShaderPs>("data/Shader/ps_texture");
	// デフォルトのデプスステンシルステート作成
	{
		D3D11_DEPTH_STENCIL_DESC desc{};
		desc.DepthEnable = TRUE;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS;
		desc.StencilEnable = FALSE;
		HRESULT hr = GetD3DDevice()->CreateDepthStencilState(&desc, default_dss.GetAddressOf());
		if (FAILED(hr)) {
			return false;
		}
	}
	// デプスステンシル無効ステート作成
	{
		D3D11_DEPTH_STENCIL_DESC desc{};
		desc.DepthEnable = FALSE;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		desc.StencilEnable = FALSE;
		HRESULT hr = GetD3DDevice()->CreateDepthStencilState(&desc, write_off_dss.GetAddressOf());
		if (FAILED(hr)) {
			return false;
		}
	}

	return true;
}

//----------------------------------------------------
// @brief Render終了。
//----------------------------------------------------
void RenderExit()
{
	texture_back_buffer_.reset();
	texture_depth_stencil_.reset();
	shader_ps_texture_.reset();
	default_dss.Reset();
	write_off_dss.Reset();
}

//----------------------------------------------------
// @brief D3Dデバイスを取得。
//----------------------------------------------------
ID3D11Device* GetD3DDevice()
{
	return reinterpret_cast<ID3D11Device*>(const_cast<void*>(GetUseDirect3D11Device()));
}

//----------------------------------------------------
// @brief D3Dデバイスコンテキストを取得。
//----------------------------------------------------
ID3D11DeviceContext* GetD3DDeviceContext()
{
	return reinterpret_cast<ID3D11DeviceContext*>(const_cast<void*>(GetUseDirect3D11DeviceContext()));
}

//----------------------------------------------------
// @brief 現在のカラーバッファをクリア。
//----------------------------------------------------
void ClearColor(const Color& clear_color)
{
	auto* color_texture = target_desc_.color_targets_[0];

	if (color_texture == nullptr)
		return;

	ClearColor(color_texture, clear_color);
}

//----------------------------------------------------
// @brief 現在のデプスバッファをクリア。
//----------------------------------------------------
void ClearDepth(f32 depth_value)
{
	auto* depth_stencil = target_desc_.depth_stencil_;

	if (depth_stencil == nullptr)
		return;

	ClearDepth(depth_stencil, depth_value);
}

//----------------------------------------------------
// @brief 現在のステンシルバッファをクリア。
//----------------------------------------------------
void ClearStencil(u8 stencil_value)
{
	auto* depth_stencil = target_desc_.depth_stencil_;

	if (depth_stencil == nullptr)
		return;

	ClearStencil(depth_stencil, stencil_value);
}

//----------------------------------------------------
// @brief カラーバッファをクリア。
//----------------------------------------------------
void ClearColor(const Texture* texture, const Color& clear_color)
{
	GetD3DDeviceContext()->ClearRenderTargetView(texture->Rtv(), clear_color.color());
}

//----------------------------------------------------
// @brief デプスバッファをクリア。
//----------------------------------------------------
void ClearDepth(const Texture* texture, f32 depth_value)
{
	GetD3DDeviceContext()->ClearDepthStencilView(texture->Dsv(), D3D11_CLEAR_DEPTH, depth_value, 0);
}

//----------------------------------------------------
// @brief ステンシルバッファをクリア。
//----------------------------------------------------
void ClearStencil(const Texture* texture, u8 stencil_value)
{
	GetD3DDeviceContext()->ClearDepthStencilView(texture->Dsv(), D3D11_CLEAR_STENCIL, stencil_value, 0);
}

//----------------------------------------------------
// @brief 現在のRenderTarget情報を取得。
//----------------------------------------------------
TargetDesc GetRenderTarget()
{
	return target_desc_;
}

//----------------------------------------------------
// @brief RenderTargetを一括設定 (TargetDesc指定)。
// 現在のレンダーターゲット情報を保存し、新しいレンダーターゲットを設定します。
// この関数を呼び出すことで、描画先のターゲットを変更できる。
//
// @param[in] desc 設定する RenderTarget の情報を持つ TargetDesc 構造体
//----------------------------------------------------
void SetRenderTarget(const TargetDesc& desc)
{
	// 保存
	target_desc_ = desc;

	// カラーターゲットを配列として取得
	std::array<ID3D11RenderTargetView*, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> rtvs;
	rtvs.fill(nullptr);
	bool depth_only_pass = true;
	for (u32 i = 0; i < desc.color_count_; ++i) {
		rtvs[i] = desc.color_targets_[i] ? desc.color_targets_[i]->Rtv() : nullptr;
		if (rtvs[i] != nullptr)
			depth_only_pass = false;
	}

	// デプスバッファ
	ID3D11DepthStencilView* dsv = desc.depth_stencil_ ? desc.depth_stencil_->Dsv() : nullptr;

	// [DxLib] DXライブラリ内部のプリミティブバッファをフラッシュさせる
	RenderVertex();

	// RenderTarget設定
	GetD3DDeviceContext()->OMSetRenderTargets(desc.color_count_, depth_only_pass ? nullptr : rtvs.data(), dsv);

	//----------------------------------------------------------
	// ビューポートとシザー領域を更新
	//----------------------------------------------------------
	{
		// RenderTargetの解像度を取得
		u32 w;
		u32 h;
		if (desc.color_targets_[0]) {
			// カラーターゲットがあるならそれを優先
			w = desc.color_targets_[0]->Width();
			h = desc.color_targets_[0]->Height();
		}
		else if (desc.depth_stencil_) {
			// デプスステンシルがあるならそれを優先
			w = desc.depth_stencil_->Width();
			h = desc.depth_stencil_->Height();
		}
		else {
			// どちらもないならバックバッファを使う
			w = texture_back_buffer_->Width();
			h = texture_back_buffer_->Height();
		}

		D3D11_VIEWPORT viewport{ 0, 0, static_cast<f32>(w), static_cast<f32>(h), 0.0f, 1.0f };
		GetD3DDeviceContext()->RSSetViewports(1, &viewport);

		D3D11_RECT rect{ 0, 0, static_cast<LONG>(w), static_cast<LONG>(h) };
		GetD3DDeviceContext()->RSSetScissorRects(1, &rect);
	}
}

//----------------------------------------------------
// @brief RenderTargetを設定。
// カラーターゲットとデプスステンシルを指定して、レンダーターゲットを設定します。
///　@param[in] color_target 設定するカラーターゲットのテクスチャ
/// @param[in] depth_stencil 設定するデプスステンシルのテクスチャ
//----------------------------------------------------
void SetRenderTarget(Texture* color_target, Texture* depth_stencil)
{
	SetRenderTarget(color_target ? 1 : 0, &color_target, depth_stencil);
}

//----------------------------------------------------
// @brief RenderTargetを設定(Multiple Render Target)。
// 複数のカラーターゲットを持つ RenderTarget を設定するための関数です。
//
// @param[in] color_count 設定するカラーターゲットの数
// @param[in] color_targets 設定するカラーターゲットのテクスチャ配列
// @param[in] depth_stencil 設定するデプスステンシルのテクスチャ
//----------------------------------------------------
void SetRenderTarget(u32 color_count, Texture** color_targets, Texture* depth_stencil)
{
	TargetDesc desc;

	desc.color_count_ = color_count;
	// テクスチャ配列をコピー
	for (u32 i = 0; i < color_count; ++i) {
		desc.color_targets_[i] = color_targets[i];
	}
	desc.depth_stencil_ = depth_stencil;

	// 設定
	SetRenderTarget(desc);
}

//----------------------------------------------------
// @brief テクスチャを設定。
// ピクセルシェーダーの指定スロットにテクスチャを設定します。
//
// @param[in] slot 設定するテクスチャのスロット番号
// @param[in] texture 設定するテクスチャ
//----------------------------------------------------
void SetTexture(u32 slot, Texture* texture)
{
	if (slot > 128)
		return;

	ID3D11ShaderResourceView* srv = texture ? texture->Srv() : nullptr;
	GetD3DDeviceContext()->PSSetShaderResources(slot, 1, &srv);
}

//----------------------------------------------------
// @brief デフォルトのバックバッファを取得 (R8G8B8A8_UNORM)。
// 現在のバックバッファを描画ターゲットとして使用するために取得します。
//----------------------------------------------------
Texture* GetBackBuffer()
{
	return texture_back_buffer_.get();
}

//----------------------------------------------------
// @brief デフォルトのデプスバッファを取得 (D32_FLOAT)。
// 現在のデプスステンシルを描画ターゲットとして使用するために取得します。
//----------------------------------------------------
Texture* GetDepthStencil()
{
	return texture_depth_stencil_.get();
}

//----------------------------------------------------
// @brief フルスクリーン2D三角形を描画します。
// @see GDC'14 Vertex Shader Tricks
// 通常の四角形描画では三角形2枚で描画しますが、斜めのエッジ部分が2回ラスタライズされます。
// 無駄を省くために巨大な三角形で覆うことでこの問題を解決できます。(数%の高速化効果)
//  A
//  |  ＼            本来のスクリーンを△ABCで覆うと有効ピクセル部分は
//  |      ＼        矩形ラスタライズされるようになります
//  |          ＼
//  +------+------+
//  |      |      |  ＼
//  +------+------+      ＼
//  |      |      |          ＼
//  B-------------+-------------C
//----------------------------------------------------
void FillRenderTarget(int shader_ps_handle)
{
	f32 w = static_cast<f32>(SCREEN_W);    // 解像度 幅
	f32 h = static_cast<f32>(SCREEN_H);    // 解像度 高さ

	DxLib::VERTEX2DSHADER v[3]{};

	// 頂点
	v[0].pos = { 0.0f, -h, 0.0f };                  // 2D座標
	v[0].rhw = 1.0f;                              // rhw = 1.0f 初期化は2D描画に必須
	v[0].dif = GetColorU8(255, 255, 255, 255);    // カラー
	v[0].u = 0.0f;                              // テクスチャ座標 U
	v[0].v = -1.0f;                             // テクスチャ座標 V

	v[1].pos = { 0.0f, h, 0.0f };                   // 2D座標
	v[1].rhw = 1.0f;                              // rhw = 1.0f 初期化は2D描画に必須
	v[1].dif = GetColorU8(255, 255, 255, 255);    // カラー
	v[1].u = 0.0f;                              // テクスチャ座標 U
	v[1].v = 1.0f;                              // テクスチャ座標 V

	v[2].pos = { w * 2.0f, h, 0.0f };               // 2D座標
	v[2].rhw = 1.0f;                              // rhw = 1.0f 初期化は2D描画に必須
	v[2].dif = GetColorU8(255, 255, 255, 255);    // カラー
	v[2].u = 2.0f;                              // テクスチャ座標 U
	v[2].v = 1.0f;                              // テクスチャ座標 V

	// 使用するピクセルシェーダーを設定 (2Dの場合は頂点シェーダー不要)
	int ps = (shader_ps_handle != -1) ? shader_ps_handle : *shader_ps_texture_;
	DxLib::SetUsePixelShader(ps);
	{
		// 描画
		DxLib::DrawPrimitive2DToShader(v, 3, DX_PRIMTYPE_TRIANGLELIST);
	}
	// もとに戻す
	DxLib::SetUsePixelShader(-1);
}

//----------------------------------------------------
// @brief RenderTargetにイメージをコピー。
//----------------------------------------------------
void CopyToRenderTarget(Texture* dest_render_target, Texture* source_texture, int shader_ps_handle)
{
	// 今のRenderTarget情報を保存しておく(関数終了時に復元するため)
	auto current_target_desc = GetRenderTarget();

	// 描画先を変更
	SetRenderTarget(dest_render_target);

	SetDrawMode(DX_DRAWMODE_BILINEAR);             // テクスチャをバイリニア補間
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);    // テクスチャを繰り返しなし

	// 使用するテクスチャを設定 (slot=0)
	SetUseTextureToShader(0, *source_texture);
	for (int i = 1; i < 16; ++i) {
		// 他のスロットは解除しておく
		SetUseTextureToShader(i, -1);
	}

	FillRenderTarget(shader_ps_handle);
	// 使い終わったらテクスチャ設定を解除
	// 解除しておかないとモデル描画に影響あり。
	SetUseTextureToShader(0, -1);
	SetUsePixelShader(-1);

	// 描画先を元に戻す

	//このときDxLibを通さず直接D3Dに触っているため、DxLibの内部状態とズレが生じる。
	//SetDrawScreenを呼んでDxLibに状態を勘違いさせ、テクスチャスロットもクリアしておかないと、
	//レンダーターゲットがスロットに刺さっていると思われてしまい、次回以降の描画で不具合が起きる。
	SetDrawScreen(DX_SCREEN_BACK);
	SetRenderTarget(current_target_desc);    // 元に戻す
}
