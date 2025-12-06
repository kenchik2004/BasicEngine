//----------------------------------------------------------------------------
//!	@file	Shader.cpp
//!	@brief	シェーダー関連の実装。Shader クラスのメソッドを実装し、シェーダーのコンパイルと管理を行う。
//!　@author　岩野 健太郎
//----------------------------------------------------------------------------

#include <mutex>

#include <d3dcompiler.h>    // シェーダーコンパイラ
#include <d3d11shader.h>
#include <filesystem>
#include <fstream>
#include "System/Utils/FileWatcher.h"
#include <wrl/client.h> 
#include "Shader.h"

// DirectX関連のリンク
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

struct ConstantVariableInfo {
	std::string name;		// 変数名
	UINT offset;                  // CB内オフセット
	UINT size;                    // バイト数
	D3D_SHADER_VARIABLE_CLASS varClass;	//クラス情報(構造体の場合は中身まで解析する必要あり)
	D3D_SHADER_VARIABLE_TYPE varType;	//int floatなどの型情報(varClassと似ているが、別物のようだ。とりあえず保管しておく)
	std::vector<ConstantVariableInfo> members; // 構造体メンバ

	//----------------------------------------------------
	// @brief 定数バッファの変数を再帰的に検索。
	// @brief 構造体の場合は、中の変数まで検索をかけて、見つかった変数の情報を返す
	// @param name 定数バッファ内の変数名 (例: "MyStruct.MyVariable")
	// @retval 見つかった変数の情報を持つ構造体へのポインタ。 ない場合はnullptrが返る。
	//----------------------------------------------------
	ConstantVariableInfo* FindValue(std::string_view name_) {
		ConstantVariableInfo* variable = nullptr;

		//渡された文字列から'.'で区切られている位置を探す
		//例："MyStruct.MyVariable"の"MyStruct"部分に当たる
		auto period = name_.find('.');

		//自分の持っているメンバ情報から、区切られた部分を探す
		std::string_view perse = name_.substr(0, period);
		auto buffer = std::find_if(members.begin(), members.end(),
			[&perse](const ConstantVariableInfo& it) {return it.name == perse; });

		//メンバにいる、かつそれ以降の区切りがまだ残っている場合、
		// 見つかったメンバの中からその先の検索を行う
		if (period != name_.npos && buffer != members.end())
			variable = buffer->FindValue(name_.substr(period + 1));
		//区切りがもう残っていない場合、見つかったメンバの情報を返す
		else if (buffer != members.end())
			variable = &(*buffer);

		return variable;

	}
};

struct ConstantBuffer {
	std::string name;		//定数バッファ名
	std::vector<ConstantVariableInfo> variables; // 定数バッファメンバ
	u8 slot;		//定数バッファが紐づけられているスロット番号(register(b0))など
	u32 size;		//定数バッファ全体のサイズ(バイト単位)
	int buffer_handle = -1;		//DxLibの定数バッファハンドル

	//----------------------------------------------------
	// @brief 定数バッファの変数を検索。
	// @param name 定数バッファ名と、バッファ内での変数名 (例: "MyBuffer.MyStruct.MyVariable")
	// @retval 見つかった変数の情報を持つ構造体へのポインタ。 ない場合はnullptrが返る。
	//----------------------------------------------------
	ConstantVariableInfo* FindValue(std::string_view name_) {
		ConstantVariableInfo* variable = nullptr;

		//渡された文字列から'.'で区切られている位置を探す
		//例："MyBuffer.MyStruct.MyVariable"の"MyBuffer"部分に当たる
		auto period = name_.find('.');
		std::string_view perse = name_.substr(0, period);
		//区切られた文字列をもとに、定数バッファを検索する
		auto buffer = std::find_if(variables.begin(), variables.end(),
			[&perse](const ConstantVariableInfo& it) {return it.name == perse; });

		//定数バッファの変数が欲しい場合(中のMyStruct.MyVariableにアクセスしたい場合)は、
		//定数バッファの中から再帰的に変数を検索してくる
		if (period != name_.npos && buffer != variables.end())
			variable = buffer->FindValue(name_.substr(period + 1));

		//バッファそのものが欲しい場合、
		//変数までは検索せずに見つかったバッファへのポインタを返す。
		else if (buffer != variables.end())
			variable = &(*buffer);

		return variable;
	}
};



//----------------------------------------------------
// @brief 構造体メンバを解析して情報を取得
// @param type 解析対象の型情報
// @param outMembers 解析結果のメンバ情報を格納する配列
// @param baseOffset 構造体のベースオフセット
//----------------------------------------------------
void ParseStructMembers(ID3D11ShaderReflectionType* type, std::vector<ConstantVariableInfo>& outMembers, UINT baseOffset)
{
	// 型情報を取得
	D3D11_SHADER_TYPE_DESC typeDesc;
	type->GetDesc(&typeDesc);

	// メンバごとに情報を取得して格納
	for (UINT i = 0; i < typeDesc.Members; i++) {
		// メンバの型情報を取得
		const char* name = type->GetMemberTypeName(i);
		ID3D11ShaderReflectionType* memberType = type->GetMemberTypeByIndex(i);

		// メンバの型情報Descを取得
		D3D11_SHADER_TYPE_DESC memberDesc;
		memberType->GetDesc(&memberDesc);

		// メンバ情報を格納
		ConstantVariableInfo memberInfo;
		memberInfo.name = name;	// メンバ名
		memberInfo.offset = baseOffset + memberDesc.Offset;	// 構造体ベースオフセットを加算
		memberInfo.size = memberDesc.Rows * memberDesc.Columns * sizeof(float); // float4単位換算
		memberInfo.varClass = memberDesc.Class;	// クラス情報
		memberInfo.varType = memberDesc.Type;	// 型情報

		// 構造体の場合は再帰的にメンバ情報を解析
		if (memberDesc.Class == D3D_SVC_STRUCT) {
			//メンバごとに同じ関数を呼び出して解析
			ParseStructMembers(memberType, memberInfo.members, memberInfo.offset);
		}

		// 解析結果を配列に追加
		outMembers.push_back(std::move(memberInfo));
	}
}

//----------------------------------------------------
// @brief シェーダーの定数バッファを解析して情報を取得
// @param reflection シェーダーリフレクションインターフェース
// @param cbuffer 解析結果の定数バッファ情報を格納するマップ
//----------------------------------------------------
void ParseShaderConstants(ID3D11ShaderReflection* reflection, std::unordered_map<std::string, ConstantBuffer>& cbuffer)
{
	// シェーダーの定数バッファ情報を取得
	D3D11_SHADER_DESC shaderDesc;
	reflection->GetDesc(&shaderDesc);

	// 定数バッファごとに情報を取得して格納
	for (UINT i = 0; i < shaderDesc.ConstantBuffers; i++) {

		//コンパイルしたシェーダーから定数バッファ情報を取得
		ID3D11ShaderReflectionConstantBuffer* cb = reflection->GetConstantBufferByIndex(i);
		//定数バッファの情報Descを取得
		D3D11_SHADER_BUFFER_DESC cbDesc;
		cb->GetDesc(&cbDesc);

		//システム側で使う定数バッファ(DxLibのものやカメラや行列、ライト関連)はスキップ
		static const std::vector<std::string> skipCBufferNames = {
			"cbD3D11_CONST_BUFFER_COMMON",
			"cbD3D11_CONST_BUFFER_PS_BASE",
			"cbBE_PS_CONST_BUFFER_DEFAULT",
			"cbD3D11_CONST_BUFFER_VS_BASE",
			"cbD3D11_CONST_BUFFER_VS_OTHERMATRIX",
			"cbD3D11_CONST_BUFFER_VS_LOCALWORLDMATRIX",
			"LightInfo",
			"CameraInfo",
		};
		//スキップ対象なら次の定数バッファへ
		if (std::find(skipCBufferNames.begin(), skipCBufferNames.end(), cbDesc.Name) != skipCBufferNames.end()) {
			continue;
		}

		//定数バッファ情報を格納
		D3D11_SHADER_INPUT_BIND_DESC bindDesc;
		reflection->GetResourceBindingDescByName(cbDesc.Name, &bindDesc);

		cbuffer[cbDesc.Name].slot = bindDesc.BindPoint;	//register(b0)のb0部分
		cbuffer[cbDesc.Name].name = cbDesc.Name;	//定数バッファ名
		cbuffer[cbDesc.Name].size = cbDesc.Size;	//定数バッファ全体のサイズ(バイト単位)

		//定数バッファ内の変数情報を格納
		for (UINT v = 0; v < cbDesc.Variables; v++) {

			//定数バッファ内の変数情報を取得
			ID3D11ShaderReflectionVariable* var = cb->GetVariableByIndex(v);
			D3D11_SHADER_VARIABLE_DESC varDesc;
			var->GetDesc(&varDesc);

			//変数の型情報を取得
			ID3D11ShaderReflectionType* type = var->GetType();
			D3D11_SHADER_TYPE_DESC typeDesc;
			type->GetDesc(&typeDesc);

			//変数情報を格納
			ConstantVariableInfo info;
			info.name = varDesc.Name;
			info.offset = varDesc.StartOffset;
			info.size = varDesc.Size;
			info.varClass = typeDesc.Class;
			info.varType = typeDesc.Type;

			//構造体の場合はメンバ情報も解析
			if (typeDesc.Class == D3D_SVC_STRUCT) {
				ParseStructMembers(type, info.members, varDesc.StartOffset);
			}
			//定数バッファを登録
			cbuffer[cbDesc.Name].variables.push_back(std::move(info));
		}

		//DxLibの定数バッファハンドルを作成
		if (cbuffer[cbDesc.Name].buffer_handle < 0)	// 未作成なら作成
			//ハンドル作成
			cbuffer[cbDesc.Name].buffer_handle = CreateShaderConstantBuffer(cbDesc.Size);

		else {	//既にある場合はサイズが変わっている可能性があるので再作成
			// 古いハンドルを削除して新規作成
			DeleteShaderConstantBuffer(cbuffer[cbDesc.Name].buffer_handle);
			// 新規作成
			cbuffer[cbDesc.Name].buffer_handle = CreateShaderConstantBuffer(cbDesc.Size);
		}
	}
}


//----------------------------------------------------
// @brief シェーダーバリエーションの情報。
//----------------------------------------------------
struct Variant {
private:
	std::unordered_map < std::string, ConstantBuffer> cbuffers;	//!<定数バッファ情報のマップ
public:
	int dxlib_shader_type;
	//----------------------------------------------------
	// @brief デフォルトコンストラクタ。
	//----------------------------------------------------
	Variant() = default;
	//----------------------------------------------------
	// @brief デストラクタ。
	//----------------------------------------------------
	~Variant()
	{
		if (handle_ != -1) {
			DxLib::DeleteShader(handle_);
		}
	}
	void SetValue(std::string_view name_, const void* value_, size_t class_size) {
		ConstantVariableInfo* info = nullptr;
		auto period = name_.find('.');
		std::string perse = std::string(name_.substr(0, period));
		if (!cbuffers.count(perse) || period == name_.npos)
			return;
		auto buffer = cbuffers[perse];
		info = buffer.FindValue(name_.substr(period + 1));
		u8* dxlib_cbuffer_ptr = reinterpret_cast<u8*>(GetBufferShaderConstantBuffer(buffer.buffer_handle));
		if (!info || !dxlib_cbuffer_ptr || class_size > buffer.size)
			return;
		memcpy(dxlib_cbuffer_ptr + info->offset, value_, class_size);
		UpdateShaderConstantBuffer(buffer.buffer_handle);
	}
	void AplyConstantBuffers() {
		for (auto& [name, buffer] : cbuffers) {
			if (buffer.buffer_handle >= 0) {
				SetShaderConstantBuffer(buffer.buffer_handle, dxlib_shader_type, buffer.slot);
			}
		}
	}

	//----------------------------------------------------
	// @brief コンパイル。
	// @param source_path HLSLソースコードのファイルパス。
	// @param index インデックス。
	// @param dxlib_shader_type [DxLib] シェーダーの種類(DX_SHADERTYPE_VERTEXなど)。
	//----------------------------------------------------
	bool compile(const std::wstring& source_path, u32 index, int dxlib_shader_type)
	{
		this->dxlib_shader_type = dxlib_shader_type;

		// ファイルからソースファイルを読み込み
		std::vector<std::byte> source_code;
		{
			// ファイルから読み込み
			std::ifstream file(source_path.c_str(),
				std::ios::in | std::ios::binary | std::ios::ate);    // ateを指定すると最初からファイルポインタが末尾に移動
			if (!file.is_open()) {
				return false;
			}
			auto size = file.tellg();
			source_code.resize(static_cast<size_t>(size));

			file.seekg(0, std::ios::beg);
			file.read(reinterpret_cast<char*>(source_code.data()), size);
			file.close();
		}
		//BOMがあるなら削除
		if (source_code[0] == std::byte{ 0xEF } && source_code[1] == std::byte{ 0xBB } && source_code[2] == std::byte{ 0xBF }) {
			source_code.erase(source_code.begin(), source_code.begin() + 3);
		}

		// シェーダーをコンパイル
		u32 compile_flags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
		compile_flags |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

#if defined(_DEBUG)
		// GraphicDebuggingツール用にシェーダーデバッグ向け設定
		compile_flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		// #define SHADER_VARIANT  index の形式でプリプロセッサ定義
		auto number_string = std::to_string(index);

		auto dxlib_version = std::format("{:#x}", DXLIB_VERSION);

		// シェーダーモデル名
		static const char* target_names[]{
			"vs_5_0",    // DX_SHADERTYPE_VERTEX   // 頂点シェーダー
			"ps_5_0",    // DX_SHADERTYPE_PIXEL    // ピクセルシェーダー
			"gs_5_0",    // DX_SHADERTYPE_GEOMETRY // ジオメトリシェーダー
			"cs_5_0",    // DX_SHADERTYPE_COMPUTE  // コンピュートシェーダー
			"ds_5_0",    // DX_SHADERTYPE_DOMAIN   // ドメインシェーダー
			"hs_5_0",    // DX_SHADERTYPE_HULL     // ハルシェーダー
		};

		// シェーダー内で参照できる #define マクロ定義
		const D3D_SHADER_MACRO defines[]{
			{"SHADER_VARIANT", number_string.c_str()},
			{ "DXLIB_VERSION", dxlib_version.c_str()},
			{         nullptr,               nullptr},
		};

		Microsoft::WRL::ComPtr<ID3DBlob> byte_code = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errors;

		auto hr = D3DCompile(source_code.data(),                   // [in]  ソースコードのメモリ上のアドレス
			source_code.size(),                   // [in]  ソースコードサイズ
			WStr2Str(source_path).c_str(),       // [in]  ソースコードのファイルパス(使用しない場合はnullptr)
			defines,                              // [in]  プリプロセッサマクロ定義
			D3D_COMPILE_STANDARD_FILE_INCLUDE,    // [in]  カスタムインクルード処理
			"main",                               // [in]  関数名
			target_names[dxlib_shader_type],      // [in]  シェーダーモデル名
			compile_flags,                        // [in]  コンパイラフラグ  (D3DCOMPILE_xxxx)
			0,                                    // [in]  コンパイラフラグ2 (D3DCOMPILE_FLAGS2_xxxx)
			&byte_code,                           // [out] コンパイルされたバイトコード
			&errors);                             // [out] エラーメッセージ

		// エラー警告出力
		if (errors != nullptr) {
			// 「出力」ウィンドウに表示
			OutputDebugStringA("--------------------\n");
			OutputDebugStringA((char*)errors->GetBufferPointer());
			OutputDebugStringA("--------------------\n");

			// メッセージボックス
			auto file_name = WStr2Str(source_path);
			MessageBox(DxLib::GetMainWindowHandle(), static_cast<char*>(errors->GetBufferPointer()), file_name.c_str(), MB_ICONWARNING | MB_OK);
		}

		if (FAILED(hr)) {
			return false;
		}
		// [DxLib] シェーダーを作成
		const void* shader_bytecode = byte_code->GetBufferPointer();                   // シェーダーバイトコードの先頭アドレス
		auto        shader_size = static_cast<int>(byte_code->GetBufferSize());    // シェーダーバイトコードのサイズ

		// シェーダーバイトコードを保存
		shader_bytecode_.resize(shader_size);
		memcpy(shader_bytecode_.data(), shader_bytecode, shader_size);

		int handle = -1;

		switch (dxlib_shader_type) {
		case DX_SHADERTYPE_VERTEX:    // 頂点シェーダー
			handle = LoadVertexShaderFromMem(shader_bytecode, shader_size);
			break;
		case DX_SHADERTYPE_PIXEL:    // ピクセルシェーダー
			handle = LoadPixelShaderFromMem(shader_bytecode, shader_size);
			break;
		case DX_SHADERTYPE_GEOMETRY:    // ジオメトリシェーダー
			handle = LoadGeometryShaderFromMem(shader_bytecode, shader_size);
			break;
		default:
			break;
		}

		// 結果を出力
		handle_ = handle;
#if 1 
		//定数バッファ情報をリフレクションで取得
		Microsoft::WRL::ComPtr<ID3D11ShaderReflection> p_reflector;
		hr = D3DReflect(shader_bytecode_.data(), shader_bytecode_.size(), IID_PPV_ARGS(p_reflector.GetAddressOf()));

		if (SUCCEEDED(hr)) {
			//定数バッファ情報を解析して格納
			ParseShaderConstants(p_reflector.Get(), cbuffers);
		}
#endif
		return true;
	}

	int                    handle_ = -1;        // [DxLib] シェーダーハンドル
	std::vector<std::byte> shader_bytecode_;    // シェーダーバイトコード

};

//----------------------------------------------------
// @brief シェーダー実体。
//----------------------------------------------------
struct Shader::Impl {
	std::wstring         path_;         // ファイルパス
	int                  type_ = -1;    // [DxLib] シェーダーの種類(DX_SHADERTYPE_VERTEXなど)
	std::vector<Variant> variant_;      // バリエーションごとの定義

	// 現在アクティブなシェーダーリスト
	static inline std::unordered_map<std::wstring, std::shared_ptr<Shader::Impl>> shaders_;

public:
	//----------------------------------------------------
	// @brief コンストラクタ。
	// @param path HLSLソースコードのファイルパス。
	// @param dxlib_shader_type [DxLib] シェーダーの種類(DX_SHADERTYPE_VERTEXなど)。
	//----------------------------------------------------
	Impl(std::wstring_view path, int dxlib_shader_type, u32 variant_count)
	{
		path_ = path;
		type_ = dxlib_shader_type;

		// 最低限1つのバリエーションを持つ
		if (variant_count == 0)
			variant_count = 1;

		variant_.resize(variant_count);

		compile();
	}

	//----------------------------------------------------
	// @brief コンパイル。
	//----------------------------------------------------
	bool compile()
	{
		// シェーダーソースの拡張子のパスを作成
		std::wstring source_path = path_ + L".fx";

		// シェーダーバリエーションコンパイル
		for (u32 i = 0; i < variant_.size(); i++) {
			// シェーダーをコンパイル
			Variant new_variant;
			bool    result = new_variant.compile(source_path, i, type_);

			// 失敗したらその先の更新をスキップ終了
			if (!result) {
				return false;
			}

			// 作成が成功したら旧シェーダーを解放して新しい結果に置換
			// 代入ではなくstd::swapで行う理由は、交換されたあとのローカル変数が自動解放されるのを利用して
			// 旧シェーダーをデストラクタで解放させることで簡潔に記述することができる。
			std::swap(variant_[i], new_variant);
		}

		return true;
	}
};

namespace {

	FileWatcher    file_watcher_;       // シェーダーホットリロード時のためのファイル監視
	std::once_flag once_initialize_;    // 初回実行用フラグ

}    // namespace

//----------------------------------------------------
// @brief 作成。
//----------------------------------------------------
Shader::Shader(std::string_view source_path, u32 dxlib_shader_type, u32 variant_count)
{
	//----------------------------------------------------------
	// ファイルパス文字列を正規化
	//----------------------------------------------------------
	std::filesystem::path path(source_path);

	// 拡張子を除去したファイルパスを取得
	auto shader_path = path.parent_path().wstring() + L"/" + path.stem().wstring();

	auto normalize_path = [](wchar_t c) {
		// 小文字に統一することで大文字小文字の差異をなくす
		c = ::towlower(c);

		// ファイルパス記号 '\\' を '/' に統一
		if (c == '\\') {
			c = L'/';
		}
		return c;
		};

	std::transform(shader_path.begin(), shader_path.end(), shader_path.begin(), normalize_path);

	//-----------------------------------------------------------------------
	// 初回実行時にファイル監視を開始する
	//-----------------------------------------------------------------------
	// ファイル変更時のコールバック関数
	auto file_modified_callback = [=](const wchar_t* full_path) {
		// ファイルパスの要素を分解
		std::filesystem::path path(full_path);
		auto                  file_path = path.parent_path().wstring() + L"/" + path.stem().wstring();

		// シェーダーソースコードだった場合
		if (path.extension() == L".fx") {
			// ファイルパスを正規化
			std::transform(file_path.begin(), file_path.end(), file_path.begin(), normalize_path);

			// 読み込み済のシェーダーパスと照合して一致したらホットリード対象
			auto it = Shader::Impl::shaders_.find(file_path);

			if (it != std::end(Shader::Impl::shaders_)) {
				auto& shader_impl = it->second;
				shader_impl->compile();    // 対象のシェーダーをコンパイル
			}
		}
		};

	// ファイル監視開始
	std::call_once(once_initialize_, [&]() { file_watcher_.initialize(L".", file_modified_callback); });

	//----------------------------------------------------------
	// 初期設定
	//----------------------------------------------------------
	// シェーダーリストを検索して既存のシェーダーの場合は再利用
	auto it = Shader::Impl::shaders_.find(shader_path);

	if (it == std::end(Shader::Impl::shaders_)) {    // 見つからなかった場合は新規作成
		auto p = std::make_shared<Shader::Impl>(shader_path, dxlib_shader_type, variant_count);

		// シェーダーリストに登録
		Shader::Impl::shaders_[shader_path] = p;

		impl_ = std::move(p);
	}
	else {    // 見つかった場合は再利用
		impl_ = it->second;
	}
}

//----------------------------------------------------
// @brief デストラクタ。
//----------------------------------------------------
Shader::~Shader()
{
}

//----------------------------------------------------
// @brief [DxLib] シェーダーハンドルを取得。
//----------------------------------------------------
Shader::operator int() const
{
	return 0 < impl_->variant_.size() ? impl_->variant_[0].handle_ : -1;
}

//----------------------------------------------------
// @brief [DxLib] シェーダーハンドルを取得 (バリエーション指定)。
// @param variant_index バリエーションインデックス。
//----------------------------------------------------
int Shader::variant(u32 variant_index) const
{
	return variant_index < impl_->variant_.size() ? impl_->variant_[variant_index].handle_ : -1;
}

//----------------------------------------------------
// @brief [DxLib] シェーダーバイトコードを取得(バリエーション指定)。
// @param variant_index バリエーションインデックス。
//----------------------------------------------------
std::tuple<const void*, size_t> Shader::shader_bytecode(u32 variant_index) const
{
	if (impl_->variant_.size() <= variant_index) {
		return { nullptr, 0 };
	}

	auto& shader_bytecode = impl_->variant_[variant_index].shader_bytecode_;
	return { shader_bytecode.data(), shader_bytecode.size() };
}

void Shader::AplyConstantBuffers(u32 variant_index)
{
	if (impl_->variant_.size() <= variant_index) {
		return;
	}
	impl_->variant_[variant_index].AplyConstantBuffers();
}

//----------------------------------------------------
// @brief ファイルパスを取得。
//----------------------------------------------------
const std::wstring& Shader::path() const
{
	return impl_->path_;
}

//----------------------------------------------------
// @brief ファイル監視を更新。
//----------------------------------------------------
void Shader::updateFileWatcher()
{
	file_watcher_.update();
}

void Shader::SetValueToVariant(Impl* impl, std::string_view name_, const void* value_, size_t class_size, u32 variant_index)
{
	if (impl->variant_.size() <= variant_index)
		return;
	impl->variant_[variant_index].SetValue(name_, value_, class_size);
}
