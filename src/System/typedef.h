#pragma once

#define PI 3.1415926535f			//�~����
#define DEG2RAD(deg) deg*PI/180		//�I�C���[�p->���W�A���p�̕ϊ�
#define RAD2DEG(rad) rad*180/PI		//���W�A���p->�I�C���[�p�̕ϊ�


#define NON_COPYABLE(CLASS)										\
					CLASS(const CLASS& that)=delete;			\
					void operator=(const CLASS& that)=delete;	\


#define NON_CREATABLE(CLASS) CLASS()=delete; 


#define SINGLETON(CLASS)															\
					CLASS(){};														\
					NON_COPYABLE;													\
					static CLASS* instance(){static CLASS class_; return &class_;}	\

//�X�}�[�g�|�C���^���g�p���邽�߂̃}�N��
#define USING_PTR(CLASS)                                        \
    class CLASS;                                                \
    using CLASS##P          = SafeSharedPtr<CLASS>;           \
    using CLASS##WP      = SafeWeakPtr<CLASS>;                \
    using CLASS##UP    = SafeUniquePtr<CLASS>;                \
    using CLASS##PVec       = std::vector<CLASS##P>;            \
    using CLASS##WPVec   = std::vector<CLASS##WP>;              \
    using CLASS##UPVec = std::vector<CLASS##UP>;                \
    using CLASS##Vec          = std::vector<CLASS*>;			\



using Vector2 = physx::PxVec2;
using Vector3 = physx::PxVec3;
using Vector4 = physx::PxVec4;
using Quaternion = physx::PxQuat;
using mat3x3 = physx::PxMat33;
using mat4x4 = physx::PxMat44;

class TypeInfo {
public:
	TypeInfo(std::string name, size_t size, TypeInfo* parent_info) {
		class_name = name;
		class_size = size;
		if (parent_info == nullptr)
			parent_info = &Root;
		parent = parent_info;

	}
	//  �N���X�����擾
	const char* ClassName() const;
	//  �N���X�̃T�C�Y���擾
	size_t ClassSize() const;
	//  �e�m�[�h���擾
//! @return �m�[�h�ւ̃|�C���^ (���݂��Ȃ��ꍇ��nullptr)
	const TypeInfo* Parent() const;
private:

	std::string class_name;
	size_t class_size = 0;
	TypeInfo* parent = nullptr;
public:
	static TypeInfo Root;

};

template <class T>
class ClassTypeInfo :public TypeInfo {
public:
	ClassTypeInfo(std::string name, TypeInfo* parent_info = nullptr)
		:TypeInfo(name, sizeof(T), parent_info)
	{
	}
};
class Class {
public:
	static inline TypeInfo& info = TypeInfo::Root;
	inline virtual TypeInfo* Info() { return static_cast<TypeInfo*>(&info); }
};



#define USING_SUPER(CLASS)																					\
		using Super = Class;																				\
		using Class = CLASS;																				\
static inline ClassTypeInfo<CLASS> info=ClassTypeInfo<CLASS>(#CLASS,&Super::info);							\
inline virtual TypeInfo* Info() {return static_cast<TypeInfo*>(&info);};									\


using s8 = int8_t;      //!<  8bit �������萮��
using s16 = int16_t;     //!< 16bit �������萮��
using s32 = int32_t;     //!< 32bit �������萮��
using s64 = int64_t;     //!< 64bit �������萮��
using u8 = uint8_t;     //!<  8bit �����Ȃ�����
using u16 = uint16_t;    //!< 16bit �����Ȃ�����
using u32 = uint32_t;    //!< 32bit �����Ȃ�����
using u64 = uint64_t;    //!< 64bit �����Ȃ�����
using f32 = float;       //!< �P���x���������_��
using f64 = double;      //!< �{���x���������_��


//�X�e�[�^�X�Ǘ��p�r�b�g���Z�e���v���[�g
template <class T, class V = u32> struct SBit {
	enum struct Bits : V
	{
	};

	inline V on(T b)																	//�r�b�g�̃I��
	{
		status_bit_ |= 1ui64 << static_cast<V>(b);
		return status_bit_;
	}
	inline V off(T b)																	//�r�b�g�̃I�t
	{
		status_bit_ &= ~(1ui64 << static_cast<V>(b));
		return status_bit_;
	}
	inline V  set(T b, bool _on) { return _on ? on(b) : off(b); }						//1�r�b�g�݂̂̃Z�b�g
	inline V  set(T b) { status_bit_ = b; return status_bit_; }							//�S�r�b�g�̃Z�b�g
	inline V  is(T b) const { return status_bit_ & (1ui64 << static_cast<int>(b)); }	//�r�b�g�̐��ۂ�Ԃ�
	inline V& get() { return status_bit_; }												//�S�r�b�g�̏���Ԃ�

private:
	V status_bit_ = 0;
};

std::wstring Str2Wstr(std::string in);			//�ʏ�string�����C�hstring�ɕϊ�
std::string WStr2Str(std::wstring in);			//���C�hstring��ʏ�string�ɕϊ�


template <class T>
class SafeUniquePtr {
	std::unique_ptr<T> u_p = nullptr;

public:
	SafeUniquePtr() = default;
	SafeUniquePtr(const std::unique_ptr<T>& p) :u_p(std::move(p)) {  }
	SafeUniquePtr(std::nullptr_t) : u_p(nullptr) {}


	// ���N���X�ւ̈Öٕϊ��R���X�g���N�^
	template <typename U, std::enable_if_t<std::is_convertible_v<U*, T*>, int> = 0>
	SafeUniquePtr(const SafeUniquePtr<U>& other)
		: u_p(std::move(other.raw_unique())) {}

	T* operator->() const {
		if (!u_p) {
			throw NullptrException("�����m���!�ʂ��!");
		}
		return u_p.get();
	}

	T& operator*() const {
		if (!u_p) {
			throw NullptrException("�����m���!�ʂ��!");
		}
		return *u_p;
	}
	operator bool() const { return u_p != nullptr; }
	T* release() { return u_p.release(); }
	void reset(T* ptr = nullptr) { u_p.reset(ptr); }
	void swap(std::unique_ptr<T>& other) { u_p.swap(other); }
	void swap(SafeUniquePtr<T>& other) { u_p.swap(other.raw_unique()); }

	const std::unique_ptr<T>& raw_unique() const { return u_p; }
	std::unique_ptr<T>& raw_unique() { return u_p; }
};

template <class T>
class SafeSharedPtr {
	std::shared_ptr<T> s_p = nullptr;

public:
	SafeSharedPtr() = default;
	SafeSharedPtr(const std::shared_ptr<T>& p) : s_p(p) {}
	SafeSharedPtr(std::nullptr_t) : s_p(nullptr) {}

	// ���N���X�ւ̈Öٕϊ��R���X�g���N�^
	template <typename U, std::enable_if_t<std::is_convertible_v<U*, T*>, int> = 0>
	SafeSharedPtr(const SafeSharedPtr<U>& other)
		: s_p(other.raw_shared()) {}

	T* operator->() const {
		if (!s_p) {
			throw NullptrException("�����m���!�ʂ��!");
		}
		return s_p.get();
	}

	T& operator*() const {
		if (!s_p) {
			throw NullptrException("�����m���!�ʂ��!");
		}
		return *s_p;
	}
	operator bool() const { return s_p.get() != nullptr; }

	void swap(std::shared_ptr<T>& other) { s_p.swap(other); }
	void swap(SafeSharedPtr<T>& other) { s_p.swap(other.raw_shared()); }
	void reset() { s_p.reset(); }
	long use_count() { return s_p.use_count(); }


	const std::shared_ptr<T>& raw_shared() const { return s_p; }
	std::shared_ptr<T>& raw_shared() { return s_p; }
};


template <class T, class... Args>
class SafeWeakPtr {

	std::weak_ptr<T> w_p;
public:
	SafeWeakPtr() = default;
	SafeWeakPtr(const std::shared_ptr<T>& p) : w_p(p) {}
	SafeWeakPtr(SafeSharedPtr<T> p) : w_p(p.raw_shared()) {}
	void operator=(SafeSharedPtr<T> p) { w_p = p.raw_shared(); }
	void operator=(SafeWeakPtr<T> p) { w_p = p.raw_weak(); }


	// ���N���X�ւ̈Öٕϊ��R���X�g���N�^
	template <typename U, std::enable_if_t<std::is_convertible_v<U*, T*>, int> = 0>
	SafeWeakPtr(const SafeWeakPtr<U>& other)
		: w_p(other.raw_weak()) {}

	T* operator->() const {
		auto lock = w_p.lock();
		if (!lock)
			throw NullptrException("�����m���!�ʂ��!");
		return lock.get();
	}

	T& operator*() const {
		if (!w_p.lock()) {
			throw NullptrException("�����m���!�ʂ��!");
		}
		return *(w_p.lock());
	}
	operator bool() const { return w_p.lock() != nullptr; }

	SafeSharedPtr<T> lock() const { return SafeSharedPtr<T>(w_p.lock()); }
	bool expired() { return w_p.expired(); }
	void reset() { w_p.reset(); }
	void swap(std::weak_ptr<T> other) { w_p.swap(other); }
	void swap(SafeWeakPtr<T> other) { w_p.swap(other.raw_weak()); }
	long use_count() { return w_p.use_count(); }


	const std::weak_ptr<T>& raw_weak() const{ return w_p; }
	std::weak_ptr<T>& raw_weak() { return w_p; }
};


template <typename T, typename... Args>
SafeSharedPtr<T> make_safe_shared(Args&&... args) {
	return SafeSharedPtr<T>(std::make_shared<T>(std::forward<Args>(args)...));
}

template <typename T, typename... Args>
SafeUniquePtr<T> make_safe_unique(Args&&... args) {
	return std::move(SafeUniquePtr<T>(std::make_unique<T>(std::forward<Args>(args)...)));
}




template<typename To, typename From>
SafeSharedPtr<To> SafeDynamicCast(const SafeSharedPtr<From>& from) {
	return SafeSharedPtr<To>(std::dynamic_pointer_cast<To>(from.raw_shared()));
}
template<typename To, typename From>
SafeSharedPtr<To> SafeStaticCast(const SafeSharedPtr<From>& from) {
	return SafeSharedPtr<To>(std::static_pointer_cast<To>(from.raw_shared()));
}



//�e�F��GetColor���g�p�����Ɉ������߂̒萔(16�i�� unsigned int)
#define BLACK     0x000000		//��
#define WHITE     0xFFFFFF		//��
#define RED       0xFF0000		//��
#define GREEN     0x00FF00		//��
#define BLUE      0x0000FF		//��
#define YELLOW    0xFFFF00		//��
#define CYAN      0x00FFFF		//�V�A��
#define MAGENTA   0xFF00FF		//�}�[���^
#define GRAY      0x808080		//�D
#define DARK_RED  0x800000		//�_�[�N���b�h
#define DARK_GREEN 0x006400		//�_�[�N�O���[��
#define DARK_BLUE 0x00008B		//�_�[�N�u���[
#define ORANGE    0xFFA500		//�I�����W
#define PINK      0xFFC0CB		//�s���N
#define BROWN     0xA52A2A		//��
#define PURPLE    0x800080		//��

