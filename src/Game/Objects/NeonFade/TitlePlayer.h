#pragma once
namespace NeonFade {

	class TitlePlayerStateMachine;

	class TitlePlayer :
		public GameObject {
	public:
		USING_SUPER(TitlePlayer);
		TitlePlayer();
		~TitlePlayer();
		int Init() override;
		void Update() override;
		void DebugDraw() override;
		TitlePlayerStateMachine* GetStateMachine() { return state_machine.get(); }

	private:
		std::unique_ptr<TitlePlayerStateMachine> state_machine;

	};
}
