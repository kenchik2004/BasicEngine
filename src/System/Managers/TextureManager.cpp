//---------------------------------------------------------------------------
//! @file   TextureManager.cpp
//! @brief  テクスチャ管理
//---------------------------------------------------------------------------
#include "TextureManager.h"
#include <d3d11.h>
#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")

std::vector<SafeUniquePtr<TextureSource>> TextureManager::cache = std::vector<SafeUniquePtr<TextureSource>>(0);
std::unordered_map<std::string, TextureManager::IndexAndHandle> TextureManager::names = std::unordered_map<std::string, TextureManager::IndexAndHandle>(0);
std::unordered_map<std::string, TextureManager::IndexAndHandle> TextureManager::paths = std::unordered_map<std::string, TextureManager::IndexAndHandle>(0);
ID3D11Device* TextureManager::s_device = nullptr;
ID3D11DeviceContext* TextureManager::s_context = nullptr;

//! @brief  ロード処理
//! @param  std::string_view テクスチャのパス
//! @param  std::string_view テクスチャにつける名前
//! @details	与えられたパスを基にテクスチャを非同期でロード・キャッシュする
void TextureManager::Load(std::string_view path, std::string_view name)
{
	std::string path_key(path);
	std::string name_key(name);
	if (paths.count(path_key) || names.count(name_key))
		return;
	auto texture = make_safe_unique<TextureSource>();
	texture->name = name_key;
	texture->path = path_key;
	auto call_back = [](int handle, void* data) {
		auto ptr = reinterpret_cast<PtrToCacheAndTextureData*>(data);
		ptr->texture_source->handle = handle;
		int cache_index = static_cast<int>(ptr->cache->size());
		auto name = ptr->texture_source->name;
		auto path = ptr->texture_source->path;
		ptr->cache->push_back(std::move(ptr->texture_source));
		(*(ptr->name_map))[name].index = cache_index;
		(*(ptr->path_map))[path].index = cache_index;
		loading_count--;
		delete ptr;
		};
	PtrToCacheAndTextureData* data = new PtrToCacheAndTextureData;
	data->cache = &cache;
	data->name_map = &names;
	data->path_map = &paths;
	data->texture_source = std::move(texture);
	SetUseASyncLoadFlag(true);
	//data->texture_source->handle = MV1LoadTexture(path_key.c_str());
	data->texture_source->handle = LoadGraph(path_key.c_str());

	if (data->texture_source->handle >= 0) {
		names[name_key].handle = data->texture_source->handle;
		paths[path_key].handle = data->texture_source->handle;
		SetASyncLoadFinishCallback(data->texture_source->handle, call_back, data);
		loading_count++;
	}
	else
		data->texture_source.reset();
	SetUseASyncLoadFlag(false);
	return;
}

void TextureManager::CacheLoaded(Texture* texture, std::string_view name)
{
}

SafeSharedPtr<Texture> TextureManager::Create(std::string_view name, int width, int height, DXGI_FORMAT format)
{

	auto tex = make_safe_shared<Texture>(width, height, format);
	if (tex)
		tex->name = name.empty() ? "CREATED:UNTITLED" : std::string(name);
	return tex;
}

SafeSharedPtr<Texture> TextureManager::CloneByName(std::string_view name, std::string_view new_name)
{

	std::string name_(name);
	SafeSharedPtr<Texture> texture = nullptr;
	auto resource = names.find(name_);
	if (resource != names.end() && resource->second.handle >= 0) {
		if (resource->second.index < 0)
			WaitHandleASyncLoad(resource->second.handle);
		while (resource->second.index < 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		auto& cache_tex = *(cache[resource->second.index]);
		if (!cache_tex.is_initialized)
			cache_tex.Init();
		texture = make_safe_shared<Texture>(cache_tex);
		if (!new_name.empty())
			texture->name = new_name;
		// キャッシュからクローンを作成するので、D3Dリソースはコピーする
		s_context->CopyResource(texture->texture.Get(), cache_tex.texture.Get());
	}
	return texture;
}

SafeSharedPtr<Texture> TextureManager::CloneByPath(std::string_view path, std::string_view new_name)
{
	std::string path_(path);
	SafeSharedPtr<Texture> texture = nullptr;
	auto resource = paths.find(path_);
	if (resource != paths.end() && resource->second.handle >= 0) {
		if (resource->second.index < 0)
			WaitHandleASyncLoad(resource->second.handle);
		while (resource->second.index < 0) {}
		auto& cache_tex = *(cache[resource->second.index]);
		if (!cache_tex.is_initialized)
			cache_tex.Init();
		texture = make_safe_shared<Texture>(cache_tex);
		if (!new_name.empty())
			texture->name = new_name;

	}
	return texture;
}

SafeSharedPtr<Texture> TextureManager::Get(std::string_view name, std::string_view new_name)
{
	// パスからテクスチャを探す
	std::string name_(name);
	SafeSharedPtr<Texture> texture = nullptr;
	auto resource = names.find(name_);
	if (resource != names.end() && resource->second.handle >= 0) {
		if (resource->second.index < 0)
			WaitHandleASyncLoad(resource->second.handle);
		while (resource->second.index < 0) {}
		auto& cache_tex = *(cache[resource->second.index]);
		if (!cache_tex.is_initialized)
			cache_tex.Init();
		texture = make_safe_shared<Texture>();
		if (!new_name.empty())
			texture->name = new_name;
		texture->texture = cache_tex.texture;
		texture->srv = cache_tex.srv;
		texture->dsv = cache_tex.dsv;
		texture->rtv = cache_tex.rtv;
		texture->width = cache_tex.width;
		texture->height = cache_tex.height;
		texture->handle = cache_tex.handle;
		texture->is_initialized = true;

	}
	return texture;
}
void TextureManager::Init()
{
	// Direct3D11 デバイスとコンテキストを取得
	s_device = reinterpret_cast<ID3D11Device*>(const_cast<void*>(GetUseDirect3D11Device()));
	s_context = reinterpret_cast<ID3D11DeviceContext*>(const_cast<void*>(GetUseDirect3D11DeviceContext()));

}

void TextureManager::Exit()
{
	WaitHandleASyncLoadAll();
	cache.clear();
	names.clear();
	paths.clear();

	s_device = nullptr;
	s_context = nullptr;
}
