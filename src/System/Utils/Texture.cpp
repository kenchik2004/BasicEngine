//----------------------------------------------------------------------------
//!	@file	Texture.cpp
//!	@brief	テクスチャ関連の実装。Texture クラスのメソッドを実装し、DirectX11 リソースの初期化を行う。
//!　@author　岩野 健太郎
//----------------------------------------------------------------------------

#include "Texture.h"

#include <d3d11.h>
#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")

//----------------------------------------------------
// @brief テクスチャソースからテクスチャを作成(テクスチャマネージャー専用)。
//----------------------------------------------------
Texture::Texture(const TextureSource& source)
{
	if (source.texture == nullptr)
		return; // テクスチャがない場合は何もしない
	D3D11_TEXTURE2D_DESC desc;
	reinterpret_cast<ID3D11Texture2D*>(source.texture.Get())->GetDesc(&desc); // テクスチャ記述を取得
	ID3D11Device* device = TextureManager::Device(); // デバイスを取得
	if (device) {
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2d;
		device->CreateTexture2D(&desc, nullptr, tex2d.GetAddressOf()); // テクスチャを作成

		is_initialized = Init(tex2d.Get()); // 初期化

		// DxLib のハンドルも作成
		if (handle < 0)
			handle = DxLib::CreateGraphFromID3D11Texture2D(texture.Get()); // DxLib ハンドルを作成
	}
	if (is_initialized) {
		name = "CLONED:" + source.name; // 名前を設定
	}
}

//----------------------------------------------------
// @brief TextureSource の初期化。
//----------------------------------------------------
void TextureSource::Init()
{
	ID3D11Resource* d3d_resource = reinterpret_cast<ID3D11Resource*>(const_cast<void*>(GetGraphID3D11Texture2D(handle))); // DxLib から D3D リソースを取得

	if (!d3d_resource)
		return; // リソースがない場合は何もしない
	D3D11_RESOURCE_DIMENSION dimension;
	d3d_resource->GetType(&dimension); // リソースタイプを取得

	if (dimension != D3D11_RESOURCE_DIMENSION_TEXTURE2D) {
		return; // 2Dテクスチャでない場合は何もしない
	}

	texture = d3d_resource; // テクスチャを設定
	auto* d3d_device = reinterpret_cast<ID3D11Device*>(const_cast<void*>(GetUseDirect3D11Device())); // デバイスを取得
	auto* d3d_texture_2d = reinterpret_cast<ID3D11Texture2D*>(d3d_resource); // 2Dテクスチャにキャスト

	D3D11_TEXTURE2D_DESC desc;
	d3d_texture_2d->GetDesc(&desc); // テクスチャ記述を取得

	width = desc.Width;     // 幅を設定
	height = desc.Height;    // 高さを設定

	if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE) { // シェーダーリソースとしてバインドされている場合
		if (desc.Format == DXGI_FORMAT_R32_TYPELESS) {    // デプスバッファ用の場合はR32_FLOATとして利用
			D3D11_SHADER_RESOURCE_VIEW_DESC view_desc{}; // SRV 記述
			view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			view_desc.Format = DXGI_FORMAT_R32_FLOAT;
			view_desc.Texture2D.MipLevels = 1;
			d3d_device->CreateShaderResourceView(d3d_resource, &view_desc, &srv); // SRV を作成
		}
		else {
			d3d_device->CreateShaderResourceView(d3d_resource, nullptr, &srv); // SRV を作成

			// ID3D11Texture2DからDxLibグラフィックハンドルを作成
			if (handle < 0) {
				handle = CreateGraphFromID3D11Texture2D(d3d_resource); // DxLib ハンドルを作成
			}
		}
	}

	if (desc.BindFlags & D3D11_BIND_RENDER_TARGET) { // レンダーターゲットとしてバインドされている場合
		d3d_device->CreateRenderTargetView(d3d_resource, nullptr, &rtv); // RTV を作成
	}

	if (desc.BindFlags & D3D11_BIND_DEPTH_STENCIL) { // デプスステンシルとしてバインドされている場合
		D3D11_DEPTH_STENCIL_VIEW_DESC view_desc{}; // DSV 記述
		view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		if (desc.Format == DXGI_FORMAT_R16_TYPELESS)
			view_desc.Format = DXGI_FORMAT_D16_UNORM;

		else if (desc.Format == DXGI_FORMAT_D32_FLOAT || desc.Format == DXGI_FORMAT_R32_TYPELESS)
			view_desc.Format = DXGI_FORMAT_D32_FLOAT;
		else
			view_desc.Format = DXGI_FORMAT_UNKNOWN;

		d3d_device->CreateDepthStencilView(d3d_resource, &view_desc, &dsv); // DSV を作成
	}
}

//----------------------------------------------------
// @brief D3Dリソースから初期化する。
//----------------------------------------------------
bool Texture::Init(ID3D11Resource* d3d_resource)
{
	if (!d3d_resource)
		return false; // リソースがない場合は失敗
	D3D11_RESOURCE_DIMENSION dimension;
	d3d_resource->GetType(&dimension); // リソースタイプを取得

	if (dimension != D3D11_RESOURCE_DIMENSION_TEXTURE2D) {
		return false; // 2Dテクスチャでない場合は失敗
	}

	texture = d3d_resource; // テクスチャを設定
	auto* d3d_device = reinterpret_cast<ID3D11Device*>(const_cast<void*>(GetUseDirect3D11Device())); // デバイスを取得
	auto* d3d_texture_2d = reinterpret_cast<ID3D11Texture2D*>(d3d_resource); // 2Dテクスチャにキャスト

	D3D11_TEXTURE2D_DESC desc;
	d3d_texture_2d->GetDesc(&desc); // テクスチャ記述を取得

	width = desc.Width;     // 幅を設定
	height = desc.Height;    // 高さを設定

	if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE) { // シェーダーリソースとしてバインドされている場合
		if (desc.Format == DXGI_FORMAT_R32_TYPELESS) {    // デプスバッファ用の場合はR32_FLOATとして利用
			D3D11_SHADER_RESOURCE_VIEW_DESC view_desc{}; // SRV 記述
			view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			view_desc.Format = DXGI_FORMAT_R32_FLOAT;
			view_desc.Texture2D.MipLevels = 1;
			d3d_device->CreateShaderResourceView(d3d_resource, &view_desc, &srv); // SRV を作成
		}
		else {
			d3d_device->CreateShaderResourceView(d3d_resource, nullptr, &srv); // SRV を作成

			// ID3D11Texture2DからDxLibグラフィックハンドルを作成
			if (handle < 0 && desc.Format != DXGI_FORMAT_R11G11B10_FLOAT && desc.Format != DXGI_FORMAT_R8G8B8A8_SNORM) {
				handle = CreateGraphFromID3D11Texture2D(d3d_resource); // DxLib ハンドルを作成
			}
		}
	}

	if (desc.BindFlags & D3D11_BIND_RENDER_TARGET) { // レンダーターゲットとしてバインドされている場合
		d3d_device->CreateRenderTargetView(d3d_resource, nullptr, &rtv); // RTV を作成
	}

	if (desc.BindFlags & D3D11_BIND_DEPTH_STENCIL) { // デプスステンシルとしてバインドされている場合
		D3D11_DEPTH_STENCIL_VIEW_DESC view_desc{}; // DSV 記述
		view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		if (desc.Format == DXGI_FORMAT_R16_TYPELESS || desc.Format == DXGI_FORMAT_R16_UNORM)
			view_desc.Format = DXGI_FORMAT_D16_UNORM;

		else if (desc.Format == DXGI_FORMAT_D32_FLOAT || desc.Format == DXGI_FORMAT_R32_TYPELESS)
			view_desc.Format = DXGI_FORMAT_D32_FLOAT;
		else
			view_desc.Format = DXGI_FORMAT_UNKNOWN;

		d3d_device->CreateDepthStencilView(d3d_resource, &view_desc, &dsv); // DSV を作成
	}

	return true; // 成功
}

//----------------------------------------------------
// @brief D3Dリソースからテクスチャを作成。
//----------------------------------------------------
Texture::Texture(ID3D11Resource* d3d_resource)
{
	is_initialized = Init(d3d_resource); // 初期化
}

//----------------------------------------------------
// @brief DXLibのテクスチャハンドルからテクスチャを作成。
//----------------------------------------------------
Texture::Texture(int dxlib_handle)
{
	DxLib::GetGraphSize(dxlib_handle, reinterpret_cast<int*>(&width), reinterpret_cast<int*>(&height)); // サイズを取得

	auto* d3d_device = reinterpret_cast<ID3D11Device*>(const_cast<void*>(GetUseDirect3D11Device())); // デバイスを取得
	ID3D11Resource* d3d_resource = reinterpret_cast<ID3D11Resource*>(const_cast<void*>(GetGraphID3D11Texture2D(dxlib_handle))); // D3D リソースを取得
	texture = d3d_resource; // テクスチャを設定
	D3D11_TEXTURE2D_DESC desc;
	reinterpret_cast<ID3D11Texture2D*>(d3d_resource)->GetDesc(&desc); // テクスチャ記述を取得
	if (desc.BindFlags & D3D11_BIND_RENDER_TARGET)
		rtv = reinterpret_cast<ID3D11RenderTargetView*>(const_cast<void*>(GetGraphID3D11RenderTargetView(dxlib_handle))); // RTV を取得
	if (desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
		dsv = reinterpret_cast<ID3D11DepthStencilView*>(const_cast<void*>(GetGraphID3D11DepthStencilView(dxlib_handle))); // DSV を取得
	if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		if (desc.Format == DXGI_FORMAT_R32_TYPELESS) {    // デプスバッファ用の場合はR32_FLOATとして利用
			D3D11_SHADER_RESOURCE_VIEW_DESC view_desc{}; // SRV 記述
			view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			view_desc.Format = DXGI_FORMAT_R32_FLOAT;
			view_desc.Texture2D.MipLevels = 1;
			d3d_device->CreateShaderResourceView(d3d_resource, &view_desc, &srv); // SRV を作成
		}
		else {
			d3d_device->CreateShaderResourceView(d3d_resource, nullptr, &srv); // SRV を作成
		}
	}
	handle = dxlib_handle; // ハンドルを設定

	is_initialized = true; // 初期化完了
}

//----------------------------------------------------
// @brief 指定した幅・高さ・フォーマットでテクスチャを作成。
//----------------------------------------------------
Texture::Texture(u32 width_, u32 height_, DXGI_FORMAT format)
{
	width = width_; // 幅を設定
	height = height_; // 高さを設定

	D3D11_TEXTURE2D_DESC desc = {}; // テクスチャ記述
	desc.Width = static_cast<UINT>(width_);
	desc.Height = static_cast<UINT>(height_);
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc = { 1,0 };
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	bool is_depth_stencil = format == DXGI_FORMAT_D24_UNORM_S8_UINT || format == DXGI_FORMAT_D32_FLOAT; // デプスステンシルかどうか
	if (is_depth_stencil)
	{
		// 深度ステンシルなら DSV も作成
		// フォーマットが D24S8 なら R24G8_TYPELESS、D32 なら R32_TYPELESS にする
		desc.Format = (format == DXGI_FORMAT_D24_UNORM_S8_UINT) ? DXGI_FORMAT_R24G8_TYPELESS : DXGI_FORMAT_R32_TYPELESS;
		desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL; // デプスステンシルバインド
	}
	else
	{
		desc.BindFlags |= D3D11_BIND_RENDER_TARGET; // レンダーターゲットバインド
	}

	ID3D11Device* device = TextureManager::Device(); // デバイスを取得
	if (device) {
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2d;
		device->CreateTexture2D(&desc, nullptr, tex2d.GetAddressOf()); // テクスチャを作成

		is_initialized = Init(tex2d.Get()); // 初期化
	}
}

//----------------------------------------------------
// @brief int への変換演算子。
//----------------------------------------------------
Texture::operator int()
{
	if (!is_initialized)
		return DX_NONE_GRAPH; // 初期化されていない場合は無効
	return handle >= 0 ? handle : DX_NONE_GRAPH; // ハンドルを返す
}
