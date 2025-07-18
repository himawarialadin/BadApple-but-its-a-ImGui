#include "ImPlotMode.h"
#include <ImPlot/implot.h>
#include <Math/Color.h>

#include "Common.h"
#include "Constant.h"

void ImPlotMode::Show()
{

	if (ImGui::Begin("Bad Apple!!"))
	{
		if (ImPlot::BeginPlot("##CustomRend", { -1.0f, -1.0f }))
		{
			ShowNodes();
			ImPlot::EndPlot();
		}
		
	}
	ImGui::End();
	
}

void ImPlotMode::ShowNodes()
{
	
	
	if (m_data.whiteNodes == nullptr)
		return;

	const uint32_t count = static_cast<uint32_t>(m_data.whiteNodes->size());
	ImPlot::SetupAxesLimits(0, m_data.size.x, -m_data.size.y, 0, ImPlotCond_Always);
	for (uint32_t i = 0; i < count; i++)
	{
		const auto& node = m_data.whiteNodes->at(i);

		Math::Vector2 min = node.node->position;
		Math::Vector2 max = node.node->position + node.node->size;

		min.y = -min.y; // Invert Y axis for ImPlot
		max.y = -max.y; // Invert Y axis for ImPlot

		ImVec2 pixelMin = ImPlot::PlotToPixels({ min.x, min.y });
		ImVec2 pixelMax = ImPlot::PlotToPixels({ max.x, max.y });

		ImPlot::PushPlotClipRect();

		auto drawList = ImPlot::GetPlotDrawList();
		if (m_data.showTree)
		{
			auto color = QuadTreeColorsToColor(*node.node);
			drawList->AddRectFilled(pixelMin, pixelMax, color.ToABGR32ColorCode());
			drawList->AddRect(pixelMin, pixelMax, Math::Colors::Black.ToABGR32ColorCode(), 0.0f, 0, QuadTreeColorFrameThickness);
		}
		else
		{
			drawList->AddRectFilled(pixelMin, pixelMax, Math::Colors::White.ToABGR32ColorCode());
		}
		ImPlot::PopPlotClipRect();
	}
}
