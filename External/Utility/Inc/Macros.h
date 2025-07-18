//���L�}�N����`
#pragma once

#ifdef _MSC_VER
#define himawari_interface struct //�C���^�[�t�F�[�X�N���X�p
#else
#define himawari_interface struct //�C���^�[�t�F�[�X�N���X�p
#endif

#define CREATE_HELPER()																	\
template<class T>																			\
class CreateHelper : public T																\
{																										\
public:																								\
	template<typename ...Args>															\
	CreateHelper(Args&& ...args) : T(std::forward<Args>(args)...) {}	\
}

//�f�t�H���g�R���X�g���N�^�A�f�X�g���N�^�����ÖٓI�ɒ�`����Ă��郁���o�֐���S��delete�錾���܂��B
#define DELETE_COPY_MOVE_COUNSTRACTORS(Type) \
Type(Type const&)						= delete;\
Type(Type&&)							= delete;\
Type&	operator=(Type const&) = delete;\
Type		operator=(Type&&)		= delete;

//�R�s�[�R���X�g���N�^�ƃR�s�[������Z�q��delete�錾���܂��B
#define DELETE_COPY_CONSTRUCTOR(Type) \
Type(Type const&)							= delete;\
Type&	operator=(Type const&)		= delete;\

//�R�s�[�R���X�g���N�^�ƃR�s�[������Z�q��default�錾���܂��B
#define DEFAULT_COPY_CONSTRUCTOR(Type) \
Type(Type const&)							= default;\
Type& operator=(Type const&)		= default

//���[�u�R���X�g���N�^�ƃ��[�u������Z�q��default�錾���܂��B
#define DEFAULT_MOVE_CONSTRUCTOR(Type) \
Type(Type&&)								= default;\
Type& operator=(Type&&)				= default


//�ÓI�N���X�ɂȂ�悤�ɈÖٓI�ɒ�`����Ă��郁���o�֐���S��delete�錾���܂��B
#define STATIC_CLASS(Type)	\
Type()										= delete;\
Type(Type const&)						= delete;\
Type(Type&&)							= delete;\
Type&	operator=(Type const&) = delete;\
Type		operator=(Type&&)		= delete;\
~Type()									= delete

//cereal�Ōp����N���X�̓o�^�A�y�ьp���̊֘A����o�^���܂��B
#define CEREAL_REGISTER_TYPE_AND_POLYMORPHIC_RELATION(Derived,Base) \
CEREAL_REGISTER_TYPE(Derived);								\
CEREAL_REGISTER_POLYMORPHIC_RELATION(Base,Derived)

//�֐������̖��g�p�x�����o�Ȃ��悤�ɂ��܂�
#define UNUSED_PARAM(value) static_cast<void>(value)


namespace Utility
{
	template<typename T>
	class EnableSingleton;
}

//�V���O���g���錾
#define ENABLE_SINGLETON(Type) friend class Utility::EnableSingleton<Type>;

#pragma warning(push)
#pragma warning(disable: 4455) // ���e�������Z�q�̃I�[�o�[���[�h���񐄏��ł��邱�Ƃ𖳎�����

// long long���e������float�ɕϊ����邽�߂̃��e�������Z�q 
// C#�̂悤�ɐ����^��f�T�t�B�b�N�X��t���邱�Ƃ�float�^�ɕϊ�����
constexpr float operator""f(unsigned long long value)
{
	return static_cast<float>(value);
}
#pragma warning(pop)