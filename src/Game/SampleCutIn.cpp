#include "precompile.h"
#include "SampleCutIn.h"
#include "System/Components/ModelRenderer.h"
#include "System/Components/Animator.h"

// 初期化処理
int SampleCutIn::Init()
{
	// モデルレンダラーを取得
	model = owner.lock()->GetComponent<ModelRenderer>();
	animator = owner.lock()->GetComponent<Animator>();
	// 描画用のスクリーンを作成
	handle = MakeScreen(SCREEN_W, SCREEN_H, true);

	// アニメーション速度を設定
	animator.lock()->anim_speed = 0.7f;

	// 透過色を設定
	SetTransColor(0, 0, 0);

	// 動画を読み込む
	image = LoadGraph("data/cutin.mp4");

	// サウンドの再生速度を設定
	SetCreateSoundTimeStretchRate(0.9f);

	// 効果音を読み込む
	se = LoadSoundMem("data/se.mp3");

	// 動画と効果音を再生
	PlayMovieToGraph(image, DX_MOVIEPLAYTYPE_BCANCEL);
	PlaySoundMem(se, DX_PLAYTYPE_BACK);

	// 初期位置を設定
	pos.x = SCREEN_W * 2;
	pos.y = SCREEN_H * 0.5f;

	// 時間のスケールを変更
	Time::SetTimeScale(0.1f);

	// 別の透過色を設定
	SetTransColor(255, 0, 255);

	return 0;
}

// 更新後の処理
void SampleCutIn::PostUpdate()
{
	// タイマーを減らす
	timer -= Time::UnscaledDeltaTime();

	// タイマーが0以下ならコンポーネントを削除
	if (timer < 0)
		RemoveThisComponent();

	// 位置を移動
	if (pos.x > SCREEN_W * 0.5f)
		pos.x -= SCREEN_W * 0.5 * Time::UnscaledDeltaTime() * 10;

	// 描画スクリーンを設定
	SetDrawScreen(handle);

	// 描画スクリーンをクリア
	ClearDrawScreen();

	// カメラの位置と向きを設定
	SetCameraPositionAndTargetAndUpVec(
		cast(owner.lock()->transform->position + cam_pos),
		cast(owner.lock()->transform->position + cam_lookat),
		cast(owner.lock()->transform->AxisY())
	);

	// カメラの近接・遠方クリップを設定
	SetCameraNearFar(0.1f, 3000.0f);

	// カメラの視野角を設定
	SetupCamera_Perspective(TO_RADIAN(45.0f));

	// 動画を描画
	DrawRotaGraph3(
		pos.x, pos.y, SCREEN_W * 0.25f, SCREEN_H * 0.25f,
		2, 0.5f, DEG2RAD(5), image, true
	);

	// モデルを描画
	model.lock()->Draw();

	// 描画スクリーンを戻す
	SetDrawScreen(DX_SCREEN_BACK);

	// 再度カメラ設定
	SetCameraNearFar(0.1f, 3000.0f);
	SetupCamera_Perspective(TO_RADIAN(45.0f));
}

// 後描画処理
void SampleCutIn::LateDraw()
{
	// スクリーン全体に描画
	DrawRotaGraph(SCREEN_W * 0.5f, SCREEN_H * 0.5f, 1, 0, handle, true);

	// タイマーを表示
	DrawFormatString(0, 50, Color::WHITE, "%.1f", timer);
}

// 終了処理
void SampleCutIn::Exit()
{
	// 時間のスケールを元に戻す
	Time::SetTimeScale(1.0f);

	// モデルのアニメーション速度を元に戻す
	if (model.lock())
		animator.lock()->anim_speed = 1.0f;

	// リソースを解放
	DeleteSoundMem(se);
	DeleteGraph(handle);
	DeleteGraph(image);
}
