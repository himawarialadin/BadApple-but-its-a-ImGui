#pragma once
#include "QuadTree.h"

typedef int ImGuiWindowFlags;

struct ModeData
{
	const std::vector<ColoredNode>*		whiteNodes = nullptr;
	Math::Vector2							size;
	bool											showTree = false;
};

class IMode
{
public:
	virtual ~IMode() = default;
	virtual void Show() = 0;
	virtual void ShowSettings() {}

public:
	ModeData m_data;

};

