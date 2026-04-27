//---------------------------------------------------------------------------
//! @file   Button.h
//! @brief  UIボタンのクリックイベントを管理するButtonコンポーネント
//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------
//! @class Button
//! @brief UIボタンの表示とクリックイベントを管理するコンポーネント
//---------------------------------------------------------------------
class Button : public Component
{
	const Vector3 DEFAULT_BUTTON_SIZE = { 100, 50,1 }; //!< ボタンのデフォルトサイズ

public:
	USING_SUPER(Button);
	//! @brief 初期化処理
	int          Init() override;
	//! @brief 更新処理（入力判定）
	void          Update() override;
	//! @brief 描画処理（後処理）
	void          LateDraw() override;
	//! @brief ボタン押下時に呼ばれるコールバック関数を設定する
	void          SetFunc(std::function<void()> func) { BtnFunc = func; }
	//! @brief ボタンに表示する画像を設定する
	void          SetImage(std::string_view file_path);
	//! @brief 通常時のボタン色への参照を返す
	unsigned int& Color() { return button_color; }
	//! @brief マウスオーバー時の色への参照を返す
	unsigned int& OnButtonColor() { return mouse_on_color; }
	//! @brief 押下時の色への参照を返す
	unsigned int& PressedColor() { return pressed_color; }

	//! @brief ボタンのテクスチャ種別
	enum BUTTON_TEXTURE_TYPE
	{
		COLOR, //!< 単色塗りつぶし
		IMAGE, //!< 画像テクスチャ
		NONE,  //!< テクスチャなし
	};

	//! @brief テクスチャ種別への参照を返す
	BUTTON_TEXTURE_TYPE& TextureType() { return texture_type; }

private:
	unsigned int              button_color = Color::WHITE;      //!< 通常時のボタン色
	unsigned int              pressed_color = Color::RED;       //!< 押下時のボタン色
	unsigned int              mouse_on_color = Color::GRAY;     //!< マウスオーバー時のボタン色
	unsigned int              current_color = button_color;     //!< 現在表示中の色
	Vector3                    button_size = DEFAULT_BUTTON_SIZE; //!< ボタンのサイズ
	bool                      is_pressed = false;               //!< 現フレームで押されているか
	bool                      was_pressed = false;              //!< 前フレームで押されていたか
	int                       image = -1;                       //!< ボタン画像ハンドル
	std::string               image_path = "";                  //!< ボタン画像のファイルパス
	int                       null_image = -1;                  //!< 無効画像ハンドル
	BUTTON_TEXTURE_TYPE       texture_type = COLOR;             //!< テクスチャ種別
	std::function<void(void)> BtnFunc = nullptr;                //!< ボタン押下コールバック
};
