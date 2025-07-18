#include "QuadTree.h"
#include "Constant.h"
#include "Common.h"

#include <Math/Color.h>
#include <array>
#include <immintrin.h>


/*!
* @brief Motonコードを使用して、IDから座標を取得する関数
* @param id MotonコードでエンコードされたID
* @return 座標 (x, y)
*/
static Math::Vector2 GetMotonCodeToPosition(uint32_t id)
{
	uint32_t odd		= 0x55555555; // 01010101... (32 bits)
	uint32_t even		= 0xAAAAAAAA; // 10101010... (32 bits)

	uint32_t x = _pext_u32(id, odd);		// x座標を抽出
	uint32_t y = _pext_u32(id, even);		// y座標を抽出

	return { static_cast<float>(x), static_cast<float>(y) };
}

static SpaceColorType GetColorTypeFromBitColorData(QuadTree::byte* bits, uint32_t x, uint32_t y, uint32_t width)
{
	const uint32_t index = (y * width + x);
	const uint32_t byteIndex	= index / 8; // ビットインデックスをバイトインデックスに変換
	const uint32_t bitIndex		= index % 8; // ビットの位置を取得
	const uint32_t bit = static_cast<uint32_t>((bits[byteIndex] >> (7 - bitIndex)) & QuadTree::byte(1)); // ビットを取得

	return (bit == 1) ? SpaceColorType::White : SpaceColorType::Black; // ビットが1なら白、0なら黒
}

void QuadTree::Create(Desc const& desc)
{
	m_nodes.clear();
	m_desc = desc;

	
	for (uint32_t depth = 0; depth < m_desc.depth; depth++)
	{
		const uint64_t				spaceCount	= 1ULL << (2 * depth);												//この階層の空間の数 (4^depth)
		const Math::Vector2		size				=	m_desc.size / static_cast<float>(1U << depth);		//この階層の空間のサイズ (階層が深くなるほどサイズは1/2になる)
		
		for (uint32_t n = 0; n < spaceCount; n++)
		{
			QuadTreeNode node
			{
				.level			= depth, //階層
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
	nodeColors.resize(m_nodes.size(), SpaceColorType::Indeterminate); // ノードの色を初期化

	//最下層のノードの色を決定
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

	//上層のノードの色を決定
	{
		for (int depth = m_desc.depth - 2; depth >= 0; --depth)
		{
			const uint64_t beginIndex = Spaces[depth];
			const uint64_t spaceCount = 1ULL << (2 * depth); // 4^depth

			for (uint64_t i = beginIndex; i < spaceCount + beginIndex; i++)
			{
				//子ノードの色を取得
				const std::pair<SpaceColorType, uint64_t> childColors[] = {
					{ nodeColors[i * 4 + 1], i * 4 + 1},
					{ nodeColors[i * 4 + 2], i * 4 + 2},
					{ nodeColors[i * 4 + 3], i * 4 + 3},
					{ nodeColors[i * 4 + 4], i * 4 + 4}
				};


				//子ノードの色を集計
				SpaceColorType color = SpaceColorType::Indeterminate;
				for(const auto& [child, chidIndex] : childColors)
				{
					// 子ノードが未決定なら集計終了
					if (child == SpaceColorType::Indeterminate)
					{
						color = SpaceColorType::Indeterminate;
						break; 
					}

					if (color == SpaceColorType::Indeterminate)
					{
						color = child; // 最初の子ノードの色を設定
					}
					else if (color != child)
					{
						color = SpaceColorType::Indeterminate; // 色が異なる場合は混合とする
						break;
					}
				}

				//子ノードの色が決定されている場合、親ノードに設定
				if (color != SpaceColorType::Indeterminate)
				{
					nodeColors[i] = color;
				}
				// 子ノードの色が異なる場合は未決定
				else
				{
					nodeColors[i] = SpaceColorType::Indeterminate;

					for (const auto& [child, chidIndex] : childColors)
					{
						// 白色のウィンドウノードのみを追加
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

	//ルートの色が決定する場合
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

