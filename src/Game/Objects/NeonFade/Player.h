//---------------------------------------------------------------------------
//! @file   Player.h
//! @brief  プレイヤーゲームオブジェクト
//---------------------------------------------------------------------------
#pragma once


USING_PTR(RigidBody);
USING_PTR(ModelRenderer);
USING_PTR(Animator);
USING_PTR(CameraObject);
namespace NeonFade
{
	USING_PTR(PlayerController);
	USING_PTR(PlayerCameraMachine);

	//---------------------------------------------------------------------
	//! @class Player
	//! @brief プレイヤーキャラクターのゲームオブジェクト
	//---------------------------------------------------------------------
	class Player :
		public GameObject
	{
	public:
		USING_SUPER(GameObject);
		//! @brief 初期化処理
		int Init() override;
		//! @brief 更新処理
		void Update() override;
		//! @brief 描画前処理
		void PreDraw() override;
		//! @brief デバッグ描画（遅延）
		void LateDebugDraw() override;
		//! @brief 終了処理
		void Exit() override;

		//! @brief 衝突開始イベント
		void OnCollisionEnter(const HitInfo& hit_info)override;
		//! @brief 衝突継続イベント
		void OnCollisionStay(const HitInfo& hit_info) override;
		//! @brief 衝突終了イベント
		void OnCollisionExit(const HitInfo& hit_info) override;
		//! @brief トリガー開始イベント
		void OnTriggerEnter(const HitInfo& hit_info)override;
		//! @brief トリガー継続イベント
		void OnTriggerStay(const HitInfo& hit_info) override;
		//! @brief トリガー終了イベント
		void OnTriggerExit(const HitInfo& hit_info) override;

		//! @brief ダメージを受ける
		void Damage(int damage);
		//! @brief 電撃エフェクト用テクスチャをマテリアルに適用する
		void SetElectroEffectTextureToMaterials();
		//! @brief マテリアルをデフォルトに戻す
		void ResetMaterialsToDefault();
		ModelRendererWP model; //!< モデルレンダラー
		AnimatorWP animator; //!< アニメーター
		RigidBodyWP rb; //!< 物理ボディ
		PlayerControllerWP pl_controller; //!< プレイヤーコントローラー
		ColliderWP foot; //!< 足元コライダー
		CameraObjectWP player_camera; //!< カメラオブジェクト
		PlayerCameraMachineWP player_camera_machine; //!< カメラマシンコンポーネント
	private:
		SafeSharedPtr<Texture> mov_tex = nullptr; //!< 移動用テクスチャ

	};

} // namespace NeonFade
