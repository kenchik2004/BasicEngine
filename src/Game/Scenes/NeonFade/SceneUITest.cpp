#include "SceneUITest.h"

#include "Game/Objects/NeonFade/TutorialUI.h"
#include "Game/Objects/NeonFade/TutorialUIButtonImage.h"

namespace NeonFade {



	static const std::string dummy_text = u8"親譲りの無鉄砲で小供の時から損ばかりしている。小学校に居る時分学校の二階から飛び降りて一週間ほど腰を抜かした事がある。\nなぜそんな無闇をしたと聞く人があるかも知れぬ。別段深い理由でもない。新築の二階から首を出していたら、\n同級生の一人が冗談に、いくら威張っても、そこから飛び降りる事は出来まい。弱虫やーい。と囃したからである。";

	void SceneUITest::Load()
	{
		for (u8 i = 0; i < static_cast<u8>(PadButton::Button4) + 1; ++i)
		{
			TextureManager::Load("data/Textures/PadButtons/switch_released_" + std::to_string(i) + ".png", "switch_button_released_" + std::to_string(i));
			TextureManager::Load("data/Textures/PadButtons/switch_pushed_" + std::to_string(i) + ".png", "switch_button_pushed_" + std::to_string(i));
		}
		for (u8 i = 1; i < 10; ++i)
		{
			TextureManager::Load("data/movie/tutorial_" + std::to_string(i) + ".mp4", "mov" + std::to_string(i));
		}
	}

	bool initialized = false;
	int SceneUITest::Init() {

		SceneManager::Object::Create<CameraObject>();

		if (TextureManager::GetLoadingCount() || initialized)
			return -1;

		if constexpr (true) {
			TutorialUIButtonImage::LoadPadButtonImages();
			auto ui = SceneManager::Object::Create<TutorialUI>("TutorialUI", "mov0", dummy_text, PadButton::Button1);
			ui->WakeUp();
		}

		initialized = true;
		return Super::Init();
	}
	void SceneUITest::Update() {
		if (!initialized) {
			Init();
			return;
		}
		static u32 counter = 0;
		if (Input::GetKeyDown(KeyCode::Space)) {
			auto ui = SceneManager::Object::Get<TutorialUI>();
			if (ui) {
				counter++;
				SceneManager::Object::Destroy(ui);
			}
			SceneManager::Object::Create<TutorialUI>("TutorialUI", "mov" + std::to_string(counter % 10), dummy_text, static_cast<PadButton>(counter % PAD_BUTTON_COUNT))->WakeUp();
		}
	}
	void SceneUITest::UnLoad()
	{
		TutorialUIButtonImage::UnloadPadButtonImages();
		Super::UnLoad();

	}
}