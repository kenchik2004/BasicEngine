//---------------------------------------------------------------------------
//! @file   Transform.h
//! @brief  オブジェクトの位置・回転・スケールを管理するTransformコンポーネント
//---------------------------------------------------------------------------
#pragma once
USING_PTR(Transform);
//---------------------------------------------------------------------
//! @class Transform
//! @brief オブジェクトのワールド/ローカル座標・回転・スケールを管理するコンポーネント
//---------------------------------------------------------------------
class Transform :
	public Component
{
private:
	friend class Object;
	friend class RigidBody;
	TransformWPVec children; //!< 子TransformへのWeakPointerのリスト
	Vector3 position_prev; //!< 前回の位置
	Quaternion rotation_prev; //!< 前回の回転
	Vector3 scale_prev; //!< 前回のスケール

	//! @brief トランスフォームを計算して子に伝播する
	void CalculateTransform();

public:
	USING_SUPER(Transform);

	Vector3 position;                          //!< ワールド座標
	Vector3 local_position; //!< 親Transformからの相対位置

	Quaternion rotation;          //!< ワールド回転
	Quaternion local_rotation; //!< 親Transformからの相対回転
	Vector3 scale;                             //!< ワールドスケール
	Vector3 local_scale; //!< 親Transformからの相対スケール
	TransformWP parent; //!< 親TransformへのWeakPointer
	//! @brief コンストラクタ
	Transform();

	//! @brief コンストラクト処理
	void Construct() override;
	//! @brief 描画前処理
	void PreDraw() override;
	//! @brief 物理演算前処理
	void PrePhysics() override;
	//! @brief 更新前処理
	void PreUpdate() override;
	//! @brief 更新処理
	void Update() override;
	//! @brief 遅延更新処理
	void LateUpdate() override;
	//! @brief 更新後処理
	void PostUpdate() override;
	//! @brief デバッグ描画処理
	void DebugDraw() override;
	//! @brief 終了処理
	void Exit() override;
	//! @brief 指定インデックスの子Transformを取得する
	TransformWP GetChild(size_t index) const;
	//! @brief 子Transformのリストを取得する
	TransformWPVec& GetChildren();
	//! @brief 子Transformを追加する
	void SetChild(TransformP new_child);
	//! @brief 親Transformを設定する
	void SetParent(TransformP new_parent);

	//! @brief 指定した子Transformを外す
	void ResetChild(TransformWP reset_child);
	//! @brief 親Transformを外す
	void ResetParent();

	//! @brief デルタタイムを乗じた量だけワールド座標を移動する
	inline void MovePosition(const Vector3& dir) { position += dir * Time::DeltaTime(); }
	//! @brief デルタタイムを乗じた量だけローカル座標を移動する
	inline void MoveLocalPosition(const Vector3& dir) { local_position += dir * Time::DeltaTime(); }
	//! @brief ワールド座標を直接設定する
	inline void SetPosition(const Vector3& pos) { position = pos; }
	//! @brief ローカル座標を直接設定する
	inline void SetLocalPosition(const Vector3& pos) { local_position = pos; }
	//! @brief オイラー角（度）でワールド回転を加算する
	void AddRotation(Vector3 euler_angles);
	//! @brief クォータニオンでワールド回転を加算する
	void AddRotation(Quaternion q);
	//! @brief オイラー角（度）でワールド回転を設定する
	void SetRotation(Vector3 euler_angles);
	//! @brief クォータニオンでワールド回転を設定する
	void SetRotation(Quaternion q);
	//! @brief クォータニオンでローカル回転を加算する
	void AddLocalRotation(Quaternion q);
	//! @brief オイラー角（度）でローカル回転を加算する
	void AddLocalRotation(Vector3 euler_angles);
	//! @brief クォータニオンでローカル回転を設定する
	void SetLocalRotation(Quaternion q);
	//! @brief オイラー角（度）でローカル回転を設定する
	void SetLocalRotation(Vector3 euler_angles);
	//! @brief ローカルX軸方向を取得する
	Vector3 AxisX();
	//! @brief ローカルY軸方向を取得する
	Vector3 AxisY();
	//! @brief ローカルZ軸方向を取得する
	Vector3 AxisZ();
	//! @brief X軸をターゲット方向に向ける
	void SetAxisX(Vector3 target, Vector3 up = { 0,1,0 });
	//! @brief Y軸をターゲット方向に向ける
	void SetAxisY(Vector3 target, Vector3 right = { 1,0,0 });
	//! @brief Z軸をターゲット方向に向ける
	void SetAxisZ(Vector3 target, Vector3 up = { 0,1,0 });

};

