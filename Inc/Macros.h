//共有マクロ定義
#pragma once

#ifdef _MSC_VER
#define himawari_interface struct //インターフェースクラス用
#else
#define himawari_interface struct //インターフェースクラス用
#endif

#define CREATE_HELPER()																	\
template<class T>																			\
class CreateHelper : public T																\
{																										\
public:																								\
	template<typename ...Args>															\
	CreateHelper(Args&& ...args) : T(std::forward<Args>(args)...) {}	\
}

//デフォルトコンストラクタ、デストラクタ除く暗黙的に定義されているメンバ関数を全てdelete宣言します。
#define DELETE_COPY_MOVE_COUNSTRACTORS(Type) \
Type(Type const&)						= delete;\
Type(Type&&)							= delete;\
Type&	operator=(Type const&) = delete;\
Type		operator=(Type&&)		= delete;

//コピーコンストラクタとコピー代入演算子をdelete宣言します。
#define DELETE_COPY_CONSTRUCTOR(Type) \
Type(Type const&)							= delete;\
Type&	operator=(Type const&)		= delete;\

//コピーコンストラクタとコピー代入演算子をdefault宣言します。
#define DEFAULT_COPY_CONSTRUCTOR(Type) \
Type(Type const&)							= default;\
Type& operator=(Type const&)		= default

//ムーブコンストラクタとムーブ代入演算子をdefault宣言します。
#define DEFAULT_MOVE_CONSTRUCTOR(Type) \
Type(Type&&)								= default;\
Type& operator=(Type&&)				= default


//静的クラスになるように暗黙的に定義されているメンバ関数を全てdelete宣言します。
#define STATIC_CLASS(Type)	\
Type()										= delete;\
Type(Type const&)						= delete;\
Type(Type&&)							= delete;\
Type&	operator=(Type const&) = delete;\
Type		operator=(Type&&)		= delete;\
~Type()									= delete

//cerealで継承先クラスの登録、及び継承の関連情報を登録します。
#define CEREAL_REGISTER_TYPE_AND_POLYMORPHIC_RELATION(Derived,Base) \
CEREAL_REGISTER_TYPE(Derived);								\
CEREAL_REGISTER_POLYMORPHIC_RELATION(Base,Derived)

//関数引数の未使用警告が出ないようにします
#define UNUSED_PARAM(value) static_cast<void>(value)


namespace Utility
{
	template<typename T>
	class EnableSingleton;
}

//シングルトン宣言
#define ENABLE_SINGLETON(Type) friend class Utility::EnableSingleton<Type>;

#pragma warning(push)
#pragma warning(disable: 4455) // リテラル演算子のオーバーロードが非推奨であることを無視する

// long longリテラルをfloatに変換するためのリテラル演算子 
// C#のように整数型にfサフィックスを付けることでfloat型に変換する
constexpr float operator""f(unsigned long long value)
{
	return static_cast<float>(value);
}
#pragma warning(pop)