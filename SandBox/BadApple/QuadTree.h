#pragma once
#include <stdint.h>
#include <vector>
#include <Math/Vector2.h>

//空間の色を定義
enum class SpaceColorType
{
	Indeterminate = 0,		//色が不定の空間 (白と黒が混在しているため、再分割が必要)
	White,						//白色の空間
	Black,						//黒色の空間
};

struct QuadTreeNode
{
	uint32_t			level; //階層
	uint32_t			id;				//ノードのID
	Math::Vector2	position;	//ノードの位置 (左上隅)
	Math::Vector2	size;			//ノードのサイズ (幅と高さ)
};

struct ColoredNode
{
	SpaceColorType	spaceType;		//空間の色
	const QuadTreeNode* node;			//対応するノードへのポインタ(reference)
};

class QuadTree
{
public:
	using byte = unsigned char;

	struct Desc
	{
		Math::Vector2	size;
		uint32_t			depth = 3;	//分割深度
	};

public:
	QuadTree() = default;
	~QuadTree() = default;

public:
	void Create(Desc const& desc);

	std::vector<ColoredNode> GenerateWhiteNodes(byte* imageData) const;
	std::vector<QuadTreeNode> const& GetNodes() const { return m_nodes; }


private:
	Desc											m_desc;		//クアッドツリーの設定
	std::vector<QuadTreeNode>		m_nodes;		//ノードのリスト
};




