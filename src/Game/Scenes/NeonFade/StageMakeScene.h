#pragma once
namespace NeonFade_Debug {
	class StageMakeScene :
		public Scene
	{
	public:
		USING_SUPER(StageMakeScene);
		int Init() override;
		void Update() override;
		void LateDebugDraw() override;
		void Exit() override;

	private:
		GameObjectP camera = nullptr;
		GameObjectP current_obj = nullptr;

		GameObjectP CreateNewObject(const std::string& mdl_path);
		class ObjFactory {
		public:
			GameObjectP CreateObject(const SceneP scene, const std::string& mdl_path);
		};
		static inline ObjFactory factory = ObjFactory();
	};
}

