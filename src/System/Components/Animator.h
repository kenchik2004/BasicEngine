//---------------------------------------------------------------------------
//! @file   Animator.h
//! @brief  3Dモデルのアニメーション再生・ブレンドを管理するAnimatorコンポーネント
//---------------------------------------------------------------------------
#pragma once


#define CUR_ANIMTIME_MAX FLT_MAX

USING_PTR(Animator);
//---------------------------------------------------------------------
//! @class Animator
//! @brief 3Dモデルのアニメーション再生・ブレンドを管理するコンポーネント
//---------------------------------------------------------------------
class Animator :
	public Component
{
public:
	USING_SUPER(Animator);
	//! @brief コンストラクト処理
	void Construct() override;
	//! @brief 初期化処理
	int Init() override;
	//! @brief 更新処理
	void Update() override;
	//! @brief デバッグ描画処理（後処理）
	void LateDebugDraw() override;
	//! @brief 終了処理
	void Exit() override;

	void OnModelChanged();
	//----------------------------------------------------------------------------------
	//アニメーションコンポーネントが管理するべきもの
	static inline float static_anim_speed = 1.0f; //!< 全体共有のアニメーション再生速度
	float anim_speed = 1.0f;                       //!< このコンポーネントのアニメーション再生速度
	float anim_blend_time = 0.5f;                  //!< アニメーションブレンドにかける時間（秒）
	bool anim_loop = false;                        //!< アニメーションをループ再生するか
	bool anim_paused = false;                      //!< アニメーションが一時停止中か
	bool freeze_hip_y = false;                     //!< 腰のY軸移動を固定するか
	SafeWeakPtr<ModelRenderer> model;              //!< 対象のModelRendererへの参照
	std::vector<SafeSharedPtr<Animation>> animation; //!< 登録済みアニメーションリスト
	SafeSharedPtr<Animation> current_anim = nullptr; //!< 現在再生中のアニメーション

	std::array<SafeSharedPtr<Animation>, 8> old_anims;	//!< ブレンド用に古いアニメーションを2つまで保持しておく

	//! @brief アニメーションを再生する
	//! @param name       アニメーション名
	//! @param loop       ループ再生するか
	//! @param start_time 開始フレーム時間
	//! @param blend_time ブレンド時間（秒）
	//! @param freeze_y   腰のY軸を固定するか
	void Play(std::string_view name, bool loop = false, float start_time = 0.0f, float blend_time = 0.2f, bool freeze_y = false);
	//! @brief アニメーションを一時停止する
	void Pause();
	//! @brief アニメーションの一時停止を解除する
	void Resume() { anim_paused = false; }
	//! @brief アニメーションを停止する
	void Stop();
	//! @brief 同名アニメーション再生中でなければ再生する
	void PlayIfNoSame(std::string_view name, bool loop = false, float start_time = 0.0f, float blend_time = 0.2f, bool freeze_y = false);
	//! @brief 現在再生中のアニメーション名を取得する
	std::string_view GetCurrentAnimName();
	//! @brief 現在のアニメーション再生時間を取得する
	float GetCurrentAnimTime();
	//! @brief アニメーションが再生中かどうか返す
	bool IsPlaying();
	//! @brief アニメーションが一時停止中かどうか返す
	bool IsPaused();
	//! @brief アニメーションをスロットに設定する（ファイル名指定）
	void SetAnimation(std::string_view name, int index = 0, std::string_view new_name = "");
	//! @brief アニメーションをスロットに設定する（ポインタ指定）
	void SetAnimation(SafeSharedPtr<Animation> anim);
	//! @brief アニメーションのコールバックを登録する
	void SetAnimationCallBack(std::string_view anim_name, std::function<void()>& call_back, float execute_frame, std::string_view method_name);
	//! @brief アニメーションのコールバックを削除する
	void ResetAnimationCallBack(std::string_view anim_name, std::string_view method_name);
	//! @brief アニメーションファイルをロードする
	static void Load(std::string_view path, std::string_view name);

	//----------------------------------------------------------------------------------

};

