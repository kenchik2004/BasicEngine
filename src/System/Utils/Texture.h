//----------------------------------------------------------------------------
//!	@file	Texture.h
//!	@brief	テクスチャ関連のクラス定義。テクスチャの読み込み、管理、DirectX11 リソースの提供。
//!　@author　岩野 健太郎
//----------------------------------------------------------------------------

#pragma once

#include "wrl.h"

//----------------------------------------------------
// @brief テクスチャの元データ。
//----------------------------------------------------
class TextureSource {
	friend class TextureManager;
	friend class Texture;

	std::string path;		// テクスチャデータの存在するパス
	std::string name;		// テクスチャのデータ名
	int handle = -1;		// テクスチャハンドル
	bool is_initialized = false;	// 初期化完了しているかどうか

	u32 width = 0;			// テクスチャの幅
	u32 height = 0;			// テクスチャの高さ
	Microsoft::WRL::ComPtr<ID3D11Resource> texture = nullptr;			// DirectX11のテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv = nullptr;	// DirectX11のシェーダリソースビュー
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv = nullptr;	// DirectX11のレンダーターゲットビュー
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv = nullptr;	// DirectX11のデプスステンシルビュー
public:
	//----------------------------------------------------
	// @brief 初期化する。
	//----------------------------------------------------
	void Init();

	//----------------------------------------------------
	// @brief デストラクタ。削除と同時にハンドルを開放。
	//----------------------------------------------------
	~TextureSource() {
		if (handle >= 0 && !is_initialized)
			// もし非同期ロード中に削除された場合、LoadGraphの戻り値を待たずにDeleteGraphを呼ぶと落ちるので、ロード完了後に自動で削除するフラグを立てる
			SetASyncLoadFinishDeleteFlag(handle);

		//D3Dリソースの解放
		texture.Reset();
		rtv.Reset();
		srv.Reset();
		dsv.Reset();
	}
};

//----------------------------------------------------
// @brief テクスチャデータを基にクローンされたデータ。
//----------------------------------------------------
class Texture {
	friend class TextureManager;
	friend class TextureRenderer;

	std::string name;		// テクスチャ名
	int handle = -1;		// テクスチャハンドル
	bool is_initialized = false;
	u32 width = 0;			// テクスチャの幅
	u32 height = 0;			// テクスチャの高さ
	Microsoft::WRL::ComPtr<ID3D11Resource> texture = nullptr;		// DirectX11のテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv = nullptr;	// DirectX11のシェーダリソースビュー
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv = nullptr;	// DirectX11のレンダーターゲットビュー
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv = nullptr;	// DirectX11のデプスステンシルビュー

private:
	//----------------------------------------------------
	// @brief D3Dリソースから初期化する。
	// @param d3d_resource DirectX11のリソース。
	//----------------------------------------------------
	bool Init(ID3D11Resource* d3d_resource);
public:
	//----------------------------------------------------
	// @brief デフォルトコンストラクタ。
	//----------------------------------------------------
	Texture() = default;

	//----------------------------------------------------
	// @brief コピーは禁止。
	//----------------------------------------------------
	Texture(const Texture& other) = delete;

	//----------------------------------------------------
	// @brief テクスチャソースからテクスチャを作成(テクスチャマネージャー専用)。
	// @param source テクスチャソース。
	//----------------------------------------------------
	Texture(const TextureSource& source);

	//----------------------------------------------------
	// @brief D3Dリソースからテクスチャを作成。
	// @param d3d_resource DirectX11のリソース。
	//----------------------------------------------------
	Texture(ID3D11Resource* d3d_resource);

	//----------------------------------------------------
	// @brief DXLibのテクスチャハンドルからテクスチャを作成。
	// @param dxlib_handle DXLibのテクスチャハンドル。
	//----------------------------------------------------
	Texture(int dxlib_handle);

	//----------------------------------------------------
	// @brief 指定した幅・高さ・フォーマットでテクスチャを作成。
	// @param width_ 幅。
	// @param height_ 高さ。
	// @param format フォーマット。
	//----------------------------------------------------
	Texture(u32 width_, u32 height_, DXGI_FORMAT format);

	//ゲッターセッター

	//----------------------------------------------------
	// @brief テクスチャの幅を取得。
	//----------------------------------------------------
	u32 Width() const { return width; }
	//----------------------------------------------------
	// @brief テクスチャの高さを取得。
	//----------------------------------------------------
	u32 Height() const { return height; }

	//----------------------------------------------------
	// @brief D3Dリソースを取得。
	//----------------------------------------------------
	inline ID3D11Resource* D3dResource() const { return texture.Get(); }

	//----------------------------------------------------
	// @brief ShaderResourceViewを取得。
	//----------------------------------------------------
	inline ID3D11ShaderResourceView* Srv() const { return srv.Get(); }

	//----------------------------------------------------
	// @brief RenderTargetViewを取得。
	//----------------------------------------------------
	inline ID3D11RenderTargetView* Rtv() const { return rtv.Get(); }

	//----------------------------------------------------
	// @brief DepthStencilViewを取得。
	//----------------------------------------------------
	inline ID3D11DepthStencilView* Dsv() const { return dsv.Get(); }

	//----------------------------------------------------
	// @brief int への変換演算子。
	//----------------------------------------------------
	operator int();

	//----------------------------------------------------
	// @brief 削除と同時にハンドルも開放。
	//----------------------------------------------------
	virtual ~Texture() {
		if (handle >= 0) {
			DeleteGraph(handle);			// 有効なら、テクスチャハンドルを削除
			srv.Reset();					// DxLibはこちらで作ったSRVまでは解放してくれないので、自前で解放
			return;

		}
		texture.Reset();						// D3Dリソースの解放
		rtv.Reset();
		dsv.Reset();
	}
};
