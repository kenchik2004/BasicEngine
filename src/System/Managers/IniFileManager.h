//---------------------------------------------------------------------------
//! @file   IniFileManager.h
//! @brief  INI設定ファイルの読み書きを行うファイルシステムマネージャー
//---------------------------------------------------------------------------
#pragma once
namespace FileSystem {

	//---------------------------------------------------------------------
	//! @class IniFileManager
	//! @brief セクション・キー形式のINIファイルに対して読み書きを行うクラス
	//---------------------------------------------------------------------
	class IniFileManager final
	{
	public:
		//! @brief bool値を読み込む
		static bool GetBool(std::string_view section, std::string_view key, bool default_, std::string path);
		//! @brief int値を読み込む
		static int GetInt(std::string_view section, std::string_view key, int default_, std::string path);
		//! @brief float値を読み込む
		static float Getfloat(std::string_view section, std::string_view key, float default_, std::string path);
		//! @brief Vector2値を読み込む
		static Vector2 GetVector2(std::string_view section, std::string_view key, Vector2 default_, std::string path);
		//! @brief Vector3値を読み込む
		static Vector3 GetVector3(std::string_view section, std::string_view key, Vector3 default_, std::string path);
		//! @brief Vector4値を読み込む
		static Vector4 GetVector4(std::string_view section, std::string_view key, Vector4 default_, std::string path);
		//! @brief Quaternion値を読み込む
		static Quaternion GetQuaternion(std::string_view section, std::string_view key, Quaternion default_, std::string path);
		//! @brief 文字列値を読み込む
		static std::string GetString(std::string_view section, std::string_view key, std::string default_, std::string path);

		//! @brief bool値を書き込む
		static void SetBool(std::string_view section, std::string_view key, bool value, std::string path);
		//! @brief int値を書き込む
		static void SetInt(std::string_view section, std::string_view key, int value, std::string path);
		//! @brief float値を書き込む
		static void SetFloat(std::string_view section, std::string_view key, float value, std::string path);
		//! @brief Vector2値を書き込む
		static void SetVector2(std::string_view section, std::string_view key, Vector2 value, std::string path);
		//! @brief Vector3値を書き込む
		static void SetVector3(std::string_view section, std::string_view key, Vector3 value, std::string path);
		//! @brief Vector4値を書き込む
		static void SetVector4(std::string_view section, std::string_view key, Vector4 value, std::string path);
		//! @brief Quaternion値を書き込む
		static void SetQuaternion(std::string_view section, std::string_view key, Quaternion value, std::string path);
		//! @brief 文字列値を書き込む
		static void SetString(std::string_view section, std::string_view key, std::string value, std::string path);

	private:


	};

}
