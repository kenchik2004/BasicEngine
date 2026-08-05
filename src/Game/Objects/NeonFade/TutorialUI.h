#pragma once


namespace NeonFade {

	//! @brief チュートリアルUIを管理するクラス
	//! @brief ムービー、テキスト、ボタン画像を一括して管理するオブジェクト
	class TutorialUI :public UIObject {
	public:
		USING_SUPER(TutorialUI);

		//! @brief コンストラクタ
		//! @param movie_name_ 再生するムービーの名前
		//! @brief text_content_ 表示するテキストの内容
		//! @brief button_type_ 表示するボタンの種類
		TutorialUI(const std::string& movie_name_, const std::string& text_content_, PadButton button_type_);

		//! @brief 初期化処理
		int Init() override;
		//! @brief 更新処理
		void Update() override;
		//! @brief 終了処理
		void Exit() override;

		//! @brief WakeUp時の処理
		void OnWakeUp() override;
		//! @brief Sleep時の処理
		void OnSleep() override;

		//! @brief UIオブジェクトを生成する
		void CreateUIObjects();

		//! @brief UIオブジェクトを破棄する
		void DestroyUIObjects();
	private:
		std::string movie_name = "";		//<! 再生するムービーの名前
		std::string text_content = "";		//<! 表示するテキストの内容
		PadButton button_type = PadButton::Button1;	//<! 表示するボタンの種類
		bool fade_in = true;		//<! フェードイン中かどうか
		bool fade_out = false;		//<! フェードアウト中かどうか
		float life_time = 0.0f;		//<! 表示されてからの経過時間
		static constexpr float MINIMUM_LIFE_TIME = 5.0f; //<! 最低表示時間（秒）
		UIObjectWP  movie;		//<! ムービーオブジェクトへの弱参照ポインタ
		UIObjectWP  text;		//<! テキストオブジェクトへの弱参照ポインタ
		UIObjectWP  button;		//<! ボタンオブジェクトへの弱参照ポインタ


	};



}

