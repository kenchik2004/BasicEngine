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

