#include "Common.h"
#include "Constant.h"
#include "QuadTree.h"
#include <Math/Math.h>

Math::Color QuadTreeColorsToColor(QuadTreeNode const& node)
{
	constexpr auto Spaces = QuadSpaces<QuadTreeMaxDepth>::Spaces;

	uint64_t		index		= node.id + Spaces[node.level];
	Math::Color	color		= QuadTreeColors[index % _countof(QuadTreeColors)];

	if(node.level < 2)
		return color; // ƒŒƒxƒ‹‚ª2–¢–ž‚Ìê‡‚ÍF‚ð‚»‚Ì‚Ü‚Ü•Ô‚·
	
	color *= Math::PowInt(QuadTreeColorParLevelScale, node.level - 1);
	color.A = 1.0f;
	return color;
	
}

void CalculateNodeDisplaySize(QuadTreeNode const& node, Math::Vector2 const& size, Math::Vector2 const& rectMin, Math::Vector2 const& rectMax, Math::Vector2* outPositon, Math::Vector2* outSize)
{
	Math::Vector2 displaySize = rectMax - rectMin;
	outPositon->x	= Math::Remap(node.position.x, 0.0f, size.x, rectMin.x,		rectMin.x + displaySize.x);
	outPositon->y	= Math::Remap(node.position.y, 0.0f, size.y, rectMin.y,		rectMin.y + displaySize.y);
	outSize->x			= Math::Remap(node.size.x,		0.0f, size.x, 0.0f,			displaySize.x);
	outSize->y			= Math::Remap(node.size.y,		0.0f, size.y, 0.0f,			displaySize.y);
}
