#pragma once


USING_PTR(RigidBody);
USING_PTR(ModelRenderer);
USING_PTR(Animator);
USING_PTR(CameraObject);
namespace NeonFade
{
	USING_PTR(PlayerController);

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
		float camera_distance_max = 30.0f;
	private:
		Vector2 camera_rot = { 0,0 };
		float camera_distance = 20.0f;

	};

} // namespace NeonFade

