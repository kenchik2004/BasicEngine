#pragma once

struct CompStat {

	friend class Component;
	friend class ObjBase;
	enum class STATUS :u32 {
		INITIALIZED = 0,
		ACTIVE = 1 << 1,
		DRAW = 1 << 2,
		REMOVED = 1 << 3,
	};
	SBit<STATUS> status_bit;
private:
	int priority = 0;
	std::string class_name;
};
USING_PTR(ObjBase);
class Component :public std::enable_shared_from_this<Component>
{
	friend class ObjBase;
private:
public:
	USING_SUPER(Component);
	CompStat status;

	ObjBaseWP owner;

	template <class T>
	inline void Construct() {
		auto this_class = std::static_pointer_cast<T>(shared_from_this());
		this_class->status.status_bit.on(CompStat::STATUS::INITIALIZED);
		this_class->status.status_bit.on(CompStat::STATUS::ACTIVE);
		this_class->status.status_bit.on(CompStat::STATUS::DRAW);
		this_class->status.class_name = typeid(T).name();
	};
	void SetPriority(int prio);
	inline int GetPriority() { return status.priority; }
	void RemoveThisComponent();
	//-----------------------------
	// Init�u���b�N(����������)
	//-----------------------------
	inline virtual int Init() { return 0; }
	//-----------------------------

	//-----------------------------
	// Update�u���b�N(�X�V�O�㏈��)
	//-----------------------------
	inline virtual void PreUpdate() {}
	inline virtual void Update() {}
	inline virtual void LateUpdate() {}
	inline virtual void PostUpdate() {}
	//-----------------------------

	//-----------------------------
	// Physics�u���b�N(�����O�㏈��)
	//-----------------------------
	inline virtual void PrePhysics() {}
	inline virtual void Physics() {}
	inline virtual void PostPhysics() {}
	//-----------------------------

	//-----------------------------
	// Draw�u���b�N(�`��O�㏈��)
	//-----------------------------
	inline virtual void PreDraw() {}
	inline virtual void Draw() {}
	inline virtual void LateDraw() {}
	inline virtual void DebugDraw() {}
	inline virtual void LateDebugDraw() {}

	//�����̏����͕`��Ɏ��t���[���܂Ŕ��f����Ȃ�
	inline virtual void PostDraw() {}
	//-----------------------------

	inline virtual void Exit() {}
};

