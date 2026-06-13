#pragma once

namespace NeonFade {

	USING_PTR(TitleLogo);

	class TitleLogo : public UIObject {
	public:
		USING_SUPER(TitleLogo);
		int Init() override;
	private:
		TextWP logo_text;
	};
}
