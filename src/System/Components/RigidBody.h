//---------------------------------------------------------------------------
//! @file   RigidBody.h
//! @brief  剛体物理シミュレーションを管理するRigidBodyコンポーネント
//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------
//! @struct LockFlag
//! @brief  各軸の移動・回転を固定するフラグ
//---------------------------------------------------------------------
struct LockFlag {
	bool x = false; //!< X軸のロックフラグ
	bool y = false; //!< Y軸のロックフラグ
	bool z = false; //!< Z軸のロックフラグ
};
//! @brief 力を加えるモードの列挙型
enum ForceMode {		//力を加えるモード
	Force = physx::PxForceMode::eFORCE,						//重さを加味した単発的な力の変化(N/s^2)
	Impulse = physx::PxForceMode::eIMPULSE,					//重さを加味した継続的な運動量の変化(N/s)
	VelocityCange = physx::PxForceMode::eVELOCITY_CHANGE,	//重さを無視した速度の変化(m/s)
	Acceleration = physx::PxForceMode::eACCELERATION,		//重さを無視した加速度の変化(m/s^2)
};
USING_PTR(RigidBody);
//---------------------------------------------------------------------
//! @class RigidBody
//! @brief PhysXを用いた剛体物理演算を管理するコンポーネント
//---------------------------------------------------------------------
class RigidBody :public Component
{
public:
	USING_SUPER(RigidBody);

	//! @brief コンポーネントを非アクティブにしたときの処理
	void OnSleep() override;
	//! @brief コンポーネントをアクティブにしたときの処理
	void OnWakeUp() override;
	//! @brief コンストラクト処理
	void Construct() override;
	//! @brief 初期化処理
	int Init() override;
	//! @brief 物理演算前処理（位置・速度の書き込み）
	void PrePhysics() override;
	//! @brief 物理演算後処理（結果の読み取り）
	void PostPhysics() override;
	//! @brief 更新処理
	void Update() override;
	//! @brief デバッグ描画処理
	void DebugDraw() override;
	//! @brief 終了処理
	void Exit() override;
	//! @brief 指定方向に力を加える
	void AddForce(Vector3 force, ForceMode force_mode = ForceMode::Force);
	//! @brief トルク（回転力）を加える
	void AddTorque(Vector3 torque, ForceMode force_mode = ForceMode::Force);
	//! @brief ワールド座標の指定位置に力を加える
	void AddForceAtPosition(Vector3 force, Vector3 world_position, ForceMode force_mode = ForceMode::Force);
	//! @brief 重心位置を設定する
	void SetMassCenter(Vector3 center);
	//! @brief 速度を直接設定する
	void SetVelocity(Vector3 velocity_);
	Vector3 velocity = { 0,0,0 };          //!< 現在の線速度
	Vector3 angular_velocity = { 0,0,0 };  //!< 現在の角速度
	float mass = 1.0f;                     //!< 質量（kg）
	//! @brief PhysXのアクターを取得する
	physx::PxRigidActor* GetBody() { return body; }
	LockFlag freeze_position = { 0,0,0 };  //!< 各軸の移動固定フラグ
	LockFlag freeze_rotation = { 0,0,0 };  //!< 各軸の回転固定フラグ
	bool use_gravity = true;               //!< 重力を適用するか
	bool is_kinematic = false;             //!< キネマティック（物理演算を受けない）モードか
	//! @brief 静的剛体に変更する
	void ChangeToStatic();

private:
	physx::PxRigidActor* body = nullptr;               //!< PhysXの剛体アクター
	Vector3 cache_pos_gl = { 0,0,0 };                  //!< キャッシュしたグローバル位置
	Vector3 cache_pos_lc = { 0,0,0 };                  //!< キャッシュしたローカル位置
	Quaternion cache_rot_gl = Quaternion(physx::PxIdentity); //!< キャッシュしたグローバル回転
	Quaternion cache_rot_lc = Quaternion(physx::PxIdentity); //!< キャッシュしたローカル回転
	Vector3 cache_vel_l = { 0,0,0 };                   //!< キャッシュした線速度
	Vector3 cache_vel_a = { 0,0,0 };                   //!< キャッシュした角速度
	//! @brief キャッシュに変化があったか確認する
	u8 CheckCacheChanged();
	//! @brief 現在のデータをキャッシュに保存する
	void CacheCurrentData();
};

