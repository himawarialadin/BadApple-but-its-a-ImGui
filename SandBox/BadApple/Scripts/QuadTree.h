#pragma once
#include <stdint.h>
#include <vector>
#include <Math/Vector2.h>

//��Ԃ̐F���`
enum class SpaceColorType
{
	Indeterminate = 0,		//�F���s��̋�� (���ƍ������݂��Ă��邽�߁A�ĕ������K�v)
	White,						//���F�̋��
	Black,						//���F�̋��
};

struct QuadTreeNode
{
	uint32_t			level; //�K�w
	uint32_t			id;				//�m�[�h��ID
	Math::Vector2	position;	//�m�[�h�̈ʒu (�����)
	Math::Vector2	size;			//�m�[�h�̃T�C�Y (���ƍ���)
};

struct ColoredNode
{
	SpaceColorType	spaceType;		//��Ԃ̐F
	const QuadTreeNode* node;			//�Ή�����m�[�h�ւ̃|�C���^(reference)
};

class QuadTree
{
public:
	using byte = unsigned char;

	struct Desc
	{
		Math::Vector2	size;
		uint32_t			depth = 3;	//�����[�x
	};

public:
	QuadTree() = default;
	~QuadTree() = default;

public:
	void Create(Desc const& desc);

	std::vector<ColoredNode> GenerateWhiteNodes(byte* imageData) const;
	std::vector<QuadTreeNode> const& GetNodes() const { return m_nodes; }


private:
	Desc											m_desc;		//�N�A�b�h�c���[�̐ݒ�
	std::vector<QuadTreeNode>		m_nodes;		//�m�[�h�̃��X�g
};




