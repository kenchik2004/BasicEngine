//---------------------------------------------------------------------------
//! @file   Object.h
//! @brief  ゲームオブジェクトの基底クラスおよびコンポーネント管理
//---------------------------------------------------------------------------
#pragma once


//---------------------------------------------------------------------
//! @struct ObjStat
//! @brief  オブジェクトの状態・優先度・種別を管理する構造体
//---------------------------------------------------------------------
struct ObjStat {
	friend class Scene;
	friend class Object;
	friend class GameObject;
	friend class UIObject;
public:
	enum OBJ_TYPE {
		NORMAL,	//!< 通常の3Dオブジェクト
		UI,		//!< UIオブジェクト
	};
	enum struct STATUS :u32 {
		CONSTRUCTED = 1,	//!< 構築済み
		INITIALIZED = 2,	//!< 初期化済み
		ACTIVE = 3,	//!< アクティブ状態
		DRAW = 4,		//!< 描画有効状態
		REMOVED = 5,	//!< 削除済み
	};

	SBit <STATUS> status_bit;		//!< 状態フラグビット
	const OBJ_TYPE& Type() const { return obj_type; }
private:
	std::string class_name = "Object";	//!< クラス名
	OBJ_TYPE obj_type = NORMAL;			//!< オブジェクト種別
	unsigned int priority = 10;			//!< 更新優先度
	unsigned int draw_priority = 10;	//!< 描画優先度
};

USING_PTR(Component);
USING_PTR(Transform);
USING_PTR(Object);
USING_PTR(Scene);

//---------------------------------------------------------------------
//! @class Object
//! @brief ゲームオブジェクトの基底クラス（コンポーネント管理・ライフサイクル管理）
//---------------------------------------------------------------------
class Object :public std::enable_shared_from_this<Object>
{
	friend class Scene;
	friend class SceneManager;
public:
	Object(unsigned int prio = 10) {
		status.priority = prio;
	}
	virtual ~Object() {}

	USING_SUPER(Object);
	ObjStat status;				//!< オブジェクトの状態情報
	enum TAG {
		Untaged,	//!< タグなし
		Player,		//!< プレイヤー
		Enemy,		//!< 敵
		Stage,		//!< ステージ
		UI,			//!< UI
		GameManager,//!< ゲームマネージャー

		__TagMax,
	};
	TAG tag = Untaged;			//!< オブジェクトのタグ
	std::string name = "EmptyObject";	//!< オブジェクト名
	TransformP transform = nullptr;		//!< トランスフォームコンポーネント
private:


	inline void Construct(SceneP owner_scene) {
		status.status_bit.on(ObjStat::STATUS::CONSTRUCTED);
		status.status_bit.on(ObjStat::STATUS::INITIALIZED);
		status.status_bit.on(ObjStat::STATUS::ACTIVE);
		status.status_bit.on(ObjStat::STATUS::DRAW);
		status.class_name = info.ClassName();
		transform = AddComponent<Transform>();
		scene = owner_scene;
	};


	void SyncComponentsPriority();
	size_t FindInsertPositionByPriority(unsigned int priority);
	bool CheckForSingleComponent(ComponentP comp);


	ComponentPVec components;						//!< 保持するコンポーネントのリスト
	ComponentPVec dirty_priority_components;		//!< 優先度変更待ちコンポーネントのリスト
	trigger_bool is_any_component_removed = false;	//!< コンポーネントが削除されたかのフラグ
	SceneP scene;									//!< 所属シーンへの参照


public:

	virtual void OnSleep() {}
	virtual void OnWakeUp() {}

	void Sleep(bool sleep_draw = true) {
		status.status_bit.off(ObjStat::STATUS::ACTIVE);
		if (sleep_draw)
			status.status_bit.off(ObjStat::STATUS::DRAW);
		for (auto& comp : components) {
			if (!comp->status.status_bit.is(CompStat::STATUS::REMOVED))
				comp->Sleep(sleep_draw);
		}
		OnSleep();
		for (auto& child : transform->GetChildren())
		{
			if (auto child_obj = child->owner)
			{
				child_obj->Sleep(sleep_draw);
			}
		}
	}
	void WakeUp() {
		status.status_bit.on(ObjStat::STATUS::ACTIVE);
		status.status_bit.on(ObjStat::STATUS::DRAW);
		for (auto& comp : components) {
			if (!comp->status.status_bit.is(CompStat::STATUS::REMOVED))
				comp->WakeUp();
		}
		OnWakeUp();
		for (auto& child : transform->GetChildren())
		{
			if (auto child_obj = child->owner)
			{
				child_obj->WakeUp();
			}
		}
	}



	//! @brief コンポーネントの優先度を設定する
	void SetComponentPriority(unsigned int prio, ComponentP who) {
		who->status.priority = prio;
		if (std::find(dirty_priority_components.begin(), dirty_priority_components.end(), who) != dirty_priority_components.end())
			dirty_priority_components.push_back(who);
	}

	//! @brief コンポーネントを新規作成して追加する
	template <class T, std::enable_if_t<std::is_convertible_v<T*, Component*>, int> = 0, typename... Args>
	SafeSharedPtr<T> AddComponent(Args&& ...args) {
		auto comp = make_safe_shared<T>(std::forward<Args>(args)...);
		try {
			comp->owner = SafeSharedPtr(shared_from_this());
			comp->ConstructBase();
			if (!CheckForSingleComponent(comp))
				return nullptr;
			size_t pos = FindInsertPositionByPriority(comp->GetPriority());
			components.insert(components.begin() + pos, comp);
			comp->Init();
		}
		catch (Exception& ex) {
			ex.Show();
		}
		return comp;
	}

	//登録前のコンポーネントであれば、ここからも登録できる(CreateInstanceしたものなど)
	template <class T, std::enable_if_t<std::is_convertible_v<T*, Component*>, int> = 0>
	SafeSharedPtr<T> AddComponentFromPtr(SafeSharedPtr<T> component) {
		auto comp = SafeStaticCast<Component>(component);
		if (comp->status.status_bit.is(CompStat::STATUS::CONSTRUCTED))
			return nullptr;
		try {
			comp->owner = SafeSharedPtr(shared_from_this());
			comp->ConstructBase();
			if (!CheckForSingleComponent(comp))
				return nullptr;

			size_t pos = FindInsertPositionByPriority(comp->GetPriority());
			components.insert(components.begin() + pos, comp);
			comp->Init();
		}
		catch (Exception& ex) {
			ex.Show();
		}
		return component;
	}

	//! @brief 指定型のコンポーネントを取得する
	template <class T, std::enable_if_t<std::is_convertible_v<T*, Component*>, int> = 0>
	SafeSharedPtr<T> GetComponent() {
		for (auto& comp : components) {
			if (!comp->status.status_bit.is(CompStat::STATUS::REMOVED))
				if (auto pick_comp = SafeDynamicCast<T>(comp)) {
					return pick_comp;
				}
		}
		return nullptr;
	}
	//! @brief 指定型のコンポーネントを全て取得する
	template <class T, std::enable_if_t<std::is_convertible_v<T*, Component*>, int> = 0>
	std::vector<SafeSharedPtr<T>> GetComponents() {
		std::vector<SafeSharedPtr<T>> vec(0);
		if (!GetComponent<T>())
			return vec;
		for (auto& comp : components) {
			if (!comp->status.status_bit.is(CompStat::STATUS::REMOVED))
				if (auto pick_comp = SafeDynamicCast<T>(comp)) {
					vec.push_back(pick_comp);
				}
		}
		return vec;
	}

	//! @brief コンポーネントを削除する（右辺値参照）
	void RemoveComponent(ComponentP&& remove_comp) {

		if (auto comp = std::find(dirty_priority_components.begin(), dirty_priority_components.end(), remove_comp); comp != dirty_priority_components.end())
		{
			dirty_priority_components.erase(comp);
		}
		if (auto comp = std::find(components.begin(), components.end(), remove_comp); comp != components.end())
		{
			remove_comp->Exit();
			remove_comp->status.status_bit.on(CompStat::STATUS::REMOVED);
			remove_comp.reset();
			is_any_component_removed.set();
		}

	}
	//! @brief コンポーネントを削除する（参照）
	void RemoveComponent(ComponentP& remove_comp) {

		if (auto comp = std::find(dirty_priority_components.begin(), dirty_priority_components.end(), remove_comp); comp != dirty_priority_components.end())
		{
			dirty_priority_components.erase(comp);
		}
		if (auto comp = std::find(components.begin(), components.end(), remove_comp); comp != components.end())
		{
			remove_comp->Exit();
			remove_comp->status.status_bit.on(CompStat::STATUS::REMOVED);
			remove_comp.reset();
			is_any_component_removed.set();
		}

	}

	//! @brief 更新優先度を設定する
	void SetPriority(unsigned int prio);
	//! @brief 更新優先度を取得する
	inline unsigned int GetPriority() { return status.priority; }
	//! @brief 所属シーンを取得する
	inline SceneP GetScene() { return scene; }


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
	// Hit時コールバック
	//-----------------------------
	inline virtual void OnCollisionEnter(const HitInfo& hit_info) {}
	inline virtual void OnCollisionStay(const HitInfo& hit_info) {}
	inline virtual void OnCollisionExit(const HitInfo& hit_info) {}

	//-----------------------------
	// Hit時コールバック(トリガー)
	//-----------------------------
	inline virtual void OnTriggerEnter(const HitInfo& hit_info) {}
	inline virtual void OnTriggerStay(const HitInfo& hit_info) {}
	inline virtual void OnTriggerExit(const HitInfo& hit_info) {}
	//-----------------------------

	//-----------------------------
	// Drawブロック(描画前後処理)
	//-----------------------------
	inline virtual void PreDraw() {}
	inline virtual void Draw() {}
	inline virtual void LateDraw() {}
	//デバッグ用描画(デバッグウィンドウに描画される)
	inline virtual void DebugDraw() {}
	inline virtual void LateDebugDraw() {}

	//ここの処理は描画に次フレームまで反映されない
	inline virtual void PostDraw() {}
	//-----------------------------

	inline virtual void Exit() {}
};

USING_PTR(GameObject);

//---------------------------------------------------------------------
//! @class GameObject
//! @brief 3D空間に配置される通常のゲームオブジェクトクラス
//---------------------------------------------------------------------
class GameObject :public Object {
public:
	USING_SUPER(GameObject);
	GameObject();
	GameObject(unsigned int prio);
	GameObject(std::string name_);
	void DebugDraw() override;


};


USING_PTR(UIObject);

//---------------------------------------------------------------------
//! @class UIObject
//! @brief UIに使用されるオブジェクトの基底クラス（アンカー・背景色管理）
//---------------------------------------------------------------------
class UIObject : public Object
{
public:
	USING_SUPER(UIObject);

	UIObject();
	int          Init() override;
	void          PreUpdate() override;
	void          Update() override;
	void          PreDraw() override final;
	void          LateDraw() override final;
	void		  DebugDraw() override;
	virtual Color& BackGroundColor() { return back_ground_color; }
	virtual bool& UseBackGround() { return use_back_color; }

	//! @brief 描画位置を取得する
	inline const Vector3& GetDrawPos() { return draw_pos; }

	//! @brief アンカータイプの列挙型
	enum ANCHOR_TYPE
	{
		LEFT_TOP,		//!< 左上
		CENTER_TOP,		//!< 中央上
		RIGHT_TOP,		//!< 右上
		LEFT_MIDDLE,	//!< 左中央
		CENTER,			//!< 中央
		RIGHT_MIDDLE,	//!< 右中央
		LEFT_BOTTOM,	//!< 左下
		CENTER_BOTTOM,	//!< 中央下
		RIGHT_BOTTOM	//!< 右下
	};
	//! @brief 自身のアンカータイプを取得・設定する
	inline ANCHOR_TYPE& AnchorType() { return anchor_type; }
	//! @brief キャンバスアンカータイプを取得・設定する
	inline ANCHOR_TYPE& CanvasAnchorType() { return canvas_anchor_type; }

	//! @brief アンカー基準点を取得・設定する
	inline Vector3& AnchorPoint() { return anchor_point; }

	//! @brief キャンバスアンカー基準点を取得・設定する
	inline Vector3& CanvasAnchorPoint() { return canvas_anchor_point; }



protected:
	int          draw_priolity = 0;					//!< 描画優先度
	bool         use_back_color = false;			//!< 背景色を使用するか
	Color back_ground_color = Color::GRAY;			//!< 背景色
	Vector3       anchor_point = { 0.0f,0.0f,0.0f };						//!< アンカー基準点
	Vector3       canvas_anchor_point = { 0.0f,0.0f,0.0f };				//!< キャンバスアンカー基準点
	Vector3       draw_pos = { 0.0f,0.0f,0.0f };							//!< 実際の描画位置
	ANCHOR_TYPE  anchor_type = CENTER;				//!< アンカータイプ
	ANCHOR_TYPE  canvas_anchor_type = CENTER;		//!< キャンバスアンカータイプ
};
