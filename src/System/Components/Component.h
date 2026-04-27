//---------------------------------------------------------------------------
//! @file   Component.h
//! @brief  全コンポーネントの基底クラスと、コンポーネント状態管理構造体の定義
//---------------------------------------------------------------------------
#pragma once

//---------------------------------------------------------------------
//! @class CompStat
//! @details コンポーネントの状態フラグや優先度を管理する構造体
//---------------------------------------------------------------------
struct CompStat {

	friend class Component;
	friend class Object;
	//! コンポーネントの状態を表すビットフラグ
	enum class STATUS :u32 {
		CONSTRUCTED = 0,    //!< コンストラクタが呼ばれた
		INITIALIZED = 1,    //!< 初期化済み
		ACTIVE = 1 << 1,    //!< アクティブ（Update/Drawが有効）
		DRAW = 1 << 2,      //!< 描画有効
		DEBUG_DRAW = 1 << 3,//!< デバッグ描画有効
		REMOVED = 1 << 4,   //!< 削除マーク済み
		SINGLE = 1 << 5,    //!< 同一Object内で1つしか持てない
	};
	SBit<STATUS> status_bit; //!< 状態ビットフラグ
private:
	unsigned int priority = 0;    //!< 処理優先度（小さいほど先に処理される）
	std::string class_name;       //!< コンポーネントのクラス名
};
USING_PTR(Object);
//---------------------------------------------------------------------
//! @class Component
//! @details ゲームオブジェクトに付与できる機能の基底クラス。
//!          各コンポーネントはこのクラスを継承して機能を実装する。
//---------------------------------------------------------------------
class Component :public std::enable_shared_from_this<Component>
{
	friend class Object;
private:
public:

	virtual ~Component() {}

	USING_SUPER(Component);
	CompStat status; //!< このコンポーネントの状態管理

	ObjectWP owner; //!< このコンポーネントを所有するObjectへの弱参照

	//! コンポーネントの基底初期化を行う（状態フラグのセット等、必ず実行される処理）
	inline void ConstructBase() {
		//こっちは必ずやる
		status.status_bit.on(CompStat::STATUS::CONSTRUCTED);
		status.status_bit.on(CompStat::STATUS::INITIALIZED);
		status.status_bit.on(CompStat::STATUS::ACTIVE);
		status.status_bit.on(CompStat::STATUS::DRAW);
		status.status_bit.on(CompStat::STATUS::DEBUG_DRAW);
		status.class_name = info.ClassName();
		//こっちはユーザーがカスタマイズできる
		Construct();
	}
	//! コンポーネントを非アクティブにする（Update/Drawが停止）
	void Sleep();
	virtual void OnSleep() {}
	//! コンポーネントをアクティブに戻す
	void WakeUp();
	virtual void OnWakeUp() {}
	//! コンストラクタ相当の初期化処理（ユーザーがカスタマイズ可能）
	inline virtual void Construct() {};
	//! 処理優先度を設定する（小さいほど先に実行される）
	void SetPriority(unsigned int prio);
	//! 現在の処理優先度を取得する
	inline unsigned int GetPriority() { return status.priority; }
	//! このコンポーネントをOwnerのObjectから取り除く
	void RemoveThisComponent();
	//-----------------------------
	// Initブロック(初期化処理)
	//-----------------------------
	inline virtual int Init() { return 0; }
	//-----------------------------

	//-----------------------------
	// Updateブロック(更新前後処理)
	//-----------------------------
	inline virtual void PreUpdate() {}
	inline virtual void Update() {}
	inline virtual void LateUpdate() {}
	inline virtual void PostUpdate() {}
	//-----------------------------

	//-----------------------------
	// Physicsブロック(物理前後処理)
	//-----------------------------
	inline virtual void PrePhysics() {}
	inline virtual void Physics() {}
	inline virtual void PostPhysics() {}
	//-----------------------------

	//-----------------------------
	// Drawブロック(描画前後処理)
	//-----------------------------
	inline virtual void PreDraw() {}
	inline virtual void Draw() {}
	inline virtual void LateDraw() {}
	inline virtual void DebugDraw() {}
	inline virtual void LateDebugDraw() {}

	//ここの処理は描画に次フレームまで反映されない
	inline virtual void PostDraw() {}
	//-----------------------------

	//! コンポーネント終了時の後片付け処理
	inline virtual void Exit() {}
};

