#include "TutorialUI.h"

#include "Game/Objects/NeonFade/TutorialUIMovie.h"
#include "Game/Objects/NeonFade/TutorialUIText.h"
#include "Game/Objects/NeonFade/TutorialUIButtonImage.h"

namespace NeonFade {

	TutorialUI::TutorialUI(const std::string& movie_name_, const std::string& text_content_, PadButton button_type_)
		:UIObject()
	{
		// UIの初期化処理
		//子オブジェクトに渡すデータを保持する
		movie_name = movie_name_;
		text_content = text_content_;
		button_type = button_type_;

	}

	int TutorialUI::Init() {

		{
			// 背景を作成する
			auto bg_obj = SceneManager::Object::Create<UIObject>("bg");
			// 背景の設定
			bg_obj->UseBackGround() = true;
			// 背景の色を設定する
			bg_obj->BackGroundColor() = Color::GRAY;
			bg_obj->BackGroundColor().a = 0.5f;

			// 背景のサイズを設定する
			bg_obj->transform->scale = { 1920 * 0.7,1080 * 0.85,1 };
			// 背景をUIの子オブジェクトとして追加する
			transform->SetChild(bg_obj->transform);
		}



		//デフォルトではUIは非表示にする
		Sleep();

		return Super::Init();
	}

	void TutorialUI::Update() {
		// フェードイン処理
		if (fade_in) {
			// フェードイン中はスケールを徐々に大きくする
			float dt = Time::UnscaledDeltaTime();
			static constexpr float FADE_IN_SPEED = 5.0f; // フェードインの速度
			transform->scale += Vector3(dt * FADE_IN_SPEED, dt * FADE_IN_SPEED, dt * FADE_IN_SPEED);

			// スケールが1.0を超えないように制限する
			if (transform->scale.x >= 1.0f) {
				// スケールを1.0に固定する
				transform->scale = { 1.0f, 1.0f, 1.0f };
				// フェードインが完了したらフラグをへし折る
				fade_in = false;

				//もしUI要素がまだ作成されていなければ作成する
				CreateUIObjects();

				//各UI要素を表示する
				movie->WakeUp();
				text->WakeUp();
				button->WakeUp();

			}
		}
		if (fade_out) {

			// フェードアウト中はスケールを徐々に小さくする
			float dt = Time::UnscaledDeltaTime();
			static constexpr float FADE_OUT_SPEED = 5.0f; // フェードアウトの速度
			transform->scale -= Vector3(dt * FADE_OUT_SPEED, dt * FADE_OUT_SPEED, dt * FADE_OUT_SPEED);

			// スケールが0.0を下回らないように制限する
			if (transform->scale.x <= 0.0f) {
				// スケールを0.0に固定する
				transform->scale = { 0.0f, 0.0f, 0.0f };
				// フェードアウトが完了したらフラグをへし折る
				fade_out = false;
				// UIを閉じる
				SceneManager::Object::Destroy(shared_from_this());
			}

		}
		// UIが表示されてから一定時間経過後にボタン入力を受け付ける
		life_time += Time::UnscaledDeltaTime();



		// 一定時間経過後にボタン入力を受け付ける
		if (life_time >= MINIMUM_LIFE_TIME && Input::GetPadButtonDown(0, PadButton::Button2)) {
			//ボタンが押されたらUIを閉じる
			fade_out = true;
			//UI要素は先に破棄する
			DestroyUIObjects();
		}
		//まだ最低表示時間に達していない場合は、ボタン入力を受け付けない
		//ただし、文字送りはスキップ(文字送り速度を最大)する
		else if (Input::GetPadButtonDown(0, PadButton::Button2) && text) {
			text->GetComponent<Text>()->text_speed = 100000;
			life_time = MINIMUM_LIFE_TIME;
		}
		Super::Update();
	}

	void TutorialUI::Exit()
	{
		DestroyUIObjects();
	}

	void TutorialUI::OnWakeUp() {
		// UIが表示されるときに呼ばれる処理
		// フェードインを開始する
		fade_in = true;
		// UIのスケールを小さくしておく
		transform->scale = { 0.001f, 0.001f, 0.001f };
	}

	void TutorialUI::OnSleep()
	{
		if (movie)
			movie->Sleep();
		if (text)
			text->Sleep();
		if (button)
			button->Sleep();
	}

	void TutorialUI::CreateUIObjects()
	{
		//ムービー再生、テキスト表示、ボタン表示のオブジェクトを作成し、キャッシュする
		if (!movie) {
			movie = SceneManager::Object::Create<TutorialUIMovie>("TutorialUI", movie_name);
			movie->SetPriority(transform->GetChild(0)->owner->GetPriority() + 1); // 自身より後に描画されるように優先度を設定する
		}

		if (!text) {
			text = SceneManager::Object::Create<TutorialUIText>("TutorialUI", text_content);
			text->SetPriority(transform->GetChild(0)->owner->GetPriority() + 1); // 自身より後に描画されるように優先度を設定する
		}

		if (!button) {
			button = SceneManager::Object::Create<TutorialUIButtonImage>("TutorialUI", button_type);
			button->SetPriority(transform->GetChild(0)->owner->GetPriority() + 1); // 自身より後に描画されるように優先度を設定する
		}

	}
	void TutorialUI::DestroyUIObjects()
	{

		if (movie)
			SceneManager::Object::Destroy(movie.lock());
		if (text)
			SceneManager::Object::Destroy(text.lock());
		if (button)
			SceneManager::Object::Destroy(button.lock());
	}
}