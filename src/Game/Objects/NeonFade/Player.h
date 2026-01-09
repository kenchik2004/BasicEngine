#pragma once


USING_PTR(RigidBody);
USING_PTR(ModelRenderer);
USING_PTR(Animator);
USING_PTR(CameraObject);
namespace NeonFade
{
	USING_PTR(PlayerController);
	USING_PTR(PlayerCameraMachine);

	class Player :
		public GameObject
	{
	public:
		USING_SUPER(GameObject);
		int Init() override;
		void Update() override;
		void PreDraw() override;
		void LateDebugDraw() override;
		void Exit() override;

		void OnCollisionEnter(const HitInfo& hit_info)override;
		void OnCollisionStay(const HitInfo& hit_info) override;
		void OnCollisionExit(const HitInfo& hit_info) override;
		void OnTriggerEnter(const HitInfo& hit_info)override;
		void OnTriggerStay(const HitInfo& hit_info) override;
		void OnTriggerExit(const HitInfo& hit_info) override;

		void Damage(int damage);
		ModelRendererWP model;
		AnimatorWP animator;
		RigidBodyWP rb;
		PlayerControllerWP pl_controller;
		ColliderWP foot;
		CameraObjectWP player_camera;
		PlayerCameraMachineWP player_camera_machine;
	private:

	};

} // namespace NeonFade

