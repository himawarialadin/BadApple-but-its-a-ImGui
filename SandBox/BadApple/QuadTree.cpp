#include "QuadTree.h"
#include "Constant.h"
#include "Common.h"

#include <Math/Color.h>
#include <array>
#include <immintrin.h>


/*!
* @brief Moton�R�[�h���g�p���āAID������W���擾����֐�
* @param id Moton�R�[�h�ŃG���R�[�h���ꂽID
* @return ���W (x, y)
*/
static Math::Vector2 GetMotonCodeToPosition(uint32_t id)
{
	uint32_t odd		= 0x55555555; // 01010101... (32 bits)
	uint32_t even		= 0xAAAAAAAA; // 10101010... (32 bits)

	uint32_t x = _pext_u32(id, odd);		// x���W�𒊏o
	uint32_t y = _pext_u32(id, even);		// y���W�𒊏o

	return { static_cast<float>(x), static_cast<float>(y) };
}

static SpaceColorType GetColorTypeFromBitColorData(QuadTree::byte* bits, uint32_t x, uint32_t y, uint32_t width)
{
	const uint32_t index = (y * width + x);
	const uint32_t byteIndex	= index / 8; // �r�b�g�C���f�b�N�X���o�C�g�C���f�b�N�X�ɕϊ�
	const uint32_t bitIndex		= index % 8; // �r�b�g�̈ʒu���擾
	const uint32_t bit = static_cast<uint32_t>((bits[byteIndex] >> (7 - bitIndex)) & QuadTree::byte(1)); // �r�b�g���擾

	return (bit == 1) ? SpaceColorType::White : SpaceColorType::Black; // �r�b�g��1�Ȃ甒�A0�Ȃ獕
}

void QuadTree::Create(Desc const& desc)
{
	m_nodes.clear();
	m_desc = desc;

	
	for (uint32_t depth = 0; depth < m_desc.depth; depth++)
	{
		const uint64_t				spaceCount	= 1ULL << (2 * depth);												//���̊K�w�̋�Ԃ̐� (4^depth)
		const Math::Vector2		size				=	m_desc.size / static_cast<float>(1U << depth);		//���̊K�w�̋�Ԃ̃T�C�Y (�K�w���[���Ȃ�قǃT�C�Y��1/2�ɂȂ�)
		
		for (uint32_t n = 0; n < spaceCount; n++)
		{
			QuadTreeNode node
			{
				.level			= depth, //�K�w
				.id				= n,
				.position	= Math::Vector2::Scale(size, GetMotonCodeToPosition(n)),
				.size			= size
			};
			m_nodes.emplace_back(node);
		}
	}


}

std::vector<ColoredNode> QuadTree::GenerateWhiteNodes(byte* colorMaps) const
{
	constexpr auto Spaces = QuadSpaces<QuadTreeMaxDepth>::Spaces;
	if(m_nodes.empty())
		return {};

	std::vector<SpaceColorType> nodeColors;
	std::vector<ColoredNode>		whiteNodes;
	nodeColors.resize(m_nodes.size(), SpaceColorType::Indeterminate); // �m�[�h�̐F��������

	//�ŉ��w�̃m�[�h�̐F������
	{
		const uint64_t beginIndex = Spaces[m_desc.depth - 1];
		const uint64_t spaceCount = 1ULL << (2 * (m_desc.depth - 1)); // 4^depth

		for (uint64_t i = beginIndex; i < spaceCount + beginIndex; i++)
		{
			Math::Vector2	centerPosition	= m_nodes[i].position + m_nodes[i].size * 0.5f;
			SpaceColorType	type					= GetColorTypeFromBitColorData(colorMaps, static_cast<uint32_t>(centerPosition.x), static_cast<uint32_t>(centerPosition.y), static_cast<uint32_t>(m_desc.size.x));
			nodeColors[i] = type;
		}
	}

	//��w�̃m�[�h�̐F������
	{
		for (int depth = m_desc.depth - 2; depth >= 0; --depth)
		{
			const uint64_t beginIndex = Spaces[depth];
			const uint64_t spaceCount = 1ULL << (2 * depth); // 4^depth

			for (uint64_t i = beginIndex; i < spaceCount + beginIndex; i++)
			{
				//�q�m�[�h�̐F���擾
				const std::pair<SpaceColorType, uint64_t> childColors[] = {
					{ nodeColors[i * 4 + 1], i * 4 + 1},
					{ nodeColors[i * 4 + 2], i * 4 + 2},
					{ nodeColors[i * 4 + 3], i * 4 + 3},
					{ nodeColors[i * 4 + 4], i * 4 + 4}
				};


				//�q�m�[�h�̐F���W�v
				SpaceColorType color = SpaceColorType::Indeterminate;
				for(const auto& [child, chidIndex] : childColors)
				{
					// �q�m�[�h��������Ȃ�W�v�I��
					if (child == SpaceColorType::Indeterminate)
					{
						color = SpaceColorType::Indeterminate;
						break; 
					}

					if (color == SpaceColorType::Indeterminate)
					{
						color = child; // �ŏ��̎q�m�[�h�̐F��ݒ�
					}
					else if (color != child)
					{
						color = SpaceColorType::Indeterminate; // �F���قȂ�ꍇ�͍����Ƃ���
						break;
					}
				}

				//�q�m�[�h�̐F�����肳��Ă���ꍇ�A�e�m�[�h�ɐݒ�
				if (color != SpaceColorType::Indeterminate)
				{
					nodeColors[i] = color;
				}
				// �q�m�[�h�̐F���قȂ�ꍇ�͖�����
				else
				{
					nodeColors[i] = SpaceColorType::Indeterminate;

					for (const auto& [child, chidIndex] : childColors)
					{
						// ���F�̃E�B���h�E�m�[�h�݂̂�ǉ�
						if (child == SpaceColorType::White)
						{
							ColoredNode node
							{
								.spaceType		= SpaceColorType::White,
								.node			= &m_nodes[chidIndex]
							};
							whiteNodes.emplace_back(node);
						}
					}
					
				}
				
				
			}
		}
	}

	//���[�g�̐F�����肷��ꍇ
	if (nodeColors[0] == SpaceColorType::White)
	{
		return 
		{
			ColoredNode{
				.spaceType		= SpaceColorType::White,
				.node			= const_cast<QuadTreeNode*>(&m_nodes[0])
			}
		};
	}

	return whiteNodes;
}

