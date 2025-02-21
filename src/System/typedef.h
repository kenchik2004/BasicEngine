#pragma once

#define PI 3.1415926535f
#define DEG2RAD(deg) deg*PI
#define RAD2DEG(rad) rad/PI

#define NON_COPYABLE(CLASS)										\
					CLASS(const CLASS& that)=delete;			\
					void operator=(const CLASS& that)=delete;	\


#define NON_CREATABLE(CLASS) CLASS()=delete; 


#define SINGLETON(CLASS)															\
					CLASS(){};														\
					NON_COPYABLE;													\
					static CLASS* instance(){static CLASS class_; return &class_;}	\


#define USING_PTR(CLASS)                                        \
    class CLASS;                                                \
    using CLASS##P          = std::shared_ptr<CLASS>;           \
    using CLASS##WP      = std::weak_ptr<CLASS>;                \
    using CLASS##UP    = std::unique_ptr<CLASS>;                \
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
	//  クラス名を取得
	const char* ClassName() const;
	//  クラスのサイズを取得
	size_t ClassSize() const;
	//  親ノードを取得
//! @return ノードへのポインタ (存在しない場合はnullptr)
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


using s8 = int8_t;      //!<  8bit 符号あり整数
using s16 = int16_t;     //!< 16bit 符号あり整数
using s32 = int32_t;     //!< 32bit 符号あり整数
using s64 = int64_t;     //!< 64bit 符号あり整数
using u8 = uint8_t;     //!<  8bit 符号なし整数
using u16 = uint16_t;    //!< 16bit 符号なし整数
using u32 = uint32_t;    //!< 32bit 符号なし整数
using u64 = uint64_t;    //!< 64bit 符号なし整数
using f32 = float;       //!< 単精度浮動小数点数
using f64 = double;      //!< 倍精度浮動小数点数

template <class T, class V = u32> struct SBit {
	enum struct Bits : V
	{
	};

	inline V on(T b)
	{
		status_bit_ |= 1ui64 << static_cast<V>(b);
		return status_bit_;
	}
	inline V off(T b)
	{
		status_bit_ &= ~(1ui64 << static_cast<V>(b));
		return status_bit_;
	}
	inline V  set(T b, bool _on) { return _on ? on(b) : off(b); }
	inline V  is(T b) const { return status_bit_ & (1ui64 << static_cast<int>(b)); }
	inline V& get() { return status_bit_; }

private:
	V status_bit_ = 0;
};

std::wstring Str2Wstr(std::string in);
std::string WStr2Str(std::wstring in);

