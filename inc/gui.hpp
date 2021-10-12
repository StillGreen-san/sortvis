#pragma once

#undef IMGUI_USE_WCHAR32 // fix for redef in imconfig

#include "imgui.h"

#include <SFML/System/Clock.hpp>

#include <implot.h>

#include "gui-help.hpp"
#include "sortable.hpp"
#include "sorter.hpp"

#include <charconv>
#include <string>

namespace sortvis
{
/**
 * @brief contains flag constants for the gui
 *
 */
namespace flags
{
constexpr auto PLOT = static_cast<ImPlotFlags_>(
    ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMousePos | ImPlotFlags_NoHighlight | ImPlotFlags_NoMenus);
constexpr auto XAXIS = static_cast<ImPlotAxisFlags_>(
    ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoLabel);
constexpr auto YAXIS = static_cast<ImPlotAxisFlags_>(ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoLabel);
constexpr auto SORTER = static_cast<ImGuiWindowFlags_>(ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoDecoration);
constexpr auto SETTINGS = static_cast<ImGuiWindowFlags_>(ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
constexpr auto DELAY = ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic;
} // namespace flags

constexpr unsigned FRAMERATE = 30;
constexpr float desiredXRatio = 1.0000f;
constexpr float desiredYRatio = 0.5625f;

/**
 * @brief class containing data used in gui, call update before drawing a frame
 *
 */
class GUIData
{
public:
	int elements = 32;
	int sortersPerLine = 3;
	float advanceDelta = 0;
	float advanceDelay = 0.2f;
	bool autoReset = false;

	ImVec2 windowSize;
	ImVec2 controlSize;

	sf::Clock deltaClock;
	sf::Time deltaTime;

	sortvis::SortableCollection sortables;
	sortvis::SorterCollection sorters;
	sortvis::BarLabels barLabels;

	GUIData() :
	    sortables(elements, true),
	    sorters(
	        sortables, {sortvis::algorithms::bubble, sortvis::algorithms::quick, sortvis::algorithms::shell,
	                       sortvis::algorithms::heap, sortvis::algorithms::insertion, sortvis::algorithms::selection})
	{
	}

public:
	/**
	 * @brief updates timing and advances/resets sorters
	 *
	 */
	void update()
	{
		deltaTime = deltaClock.restart();
		advanceDelta += deltaTime.asSeconds();

		while(advanceDelta > advanceDelay)
		{
			advanceDelta -= advanceDelay;
			if(!sorters.allHaveFinished())
			{
				sorters.advance();
			}
			else if(autoReset)
			{
				sortables.randomize();
				sorters.reset(sortables);
			}
		}

		windowSize = ImGui::GetMainViewport()->Size;

		const size_t sorterCount = sorters.size();
		float smallestAspectDeviation = 99;

		for(size_t cols = 1; cols <= sorterCount; ++cols)
		{
			size_t rows = (sorterCount + 1) / cols;
			if(rows * cols >= sorterCount)
			{
				float aspectRatio = (windowSize.y / rows) / (windowSize.x / cols);
				float aspectDeviation = std::abs(aspectRatio - desiredYRatio);
				if(aspectDeviation < smallestAspectDeviation)
				{
					smallestAspectDeviation = aspectDeviation;
					sortersPerLine = (int)cols;
				}
			}
		}
	}
};

using RGBcolor = uint32_t;

/**
 * @brief converts 24bit rgb to ImVec4
 *
 * @param rgb 24bit color value
 * @return ImVec4 rgba float color
 */
ImVec4 fromRGB(RGBcolor rgb) noexcept
{
	return {
	    ((rgb & 0xFF0000) >> 16) / 255.0f, ((rgb & 0x00FF00) >> 8) / 255.0f, ((rgb & 0x0000FF) >> 0) / 255.0f, 1.0f};
}

constexpr RGBcolor ROYAL_BLUE = 0x4169E1;
constexpr RGBcolor FOREST_GREEN = 0x228B22;
constexpr RGBcolor SPRING_GREEN = 0x00FF7F;
constexpr RGBcolor GOLDEN_ROD = 0xDAA520;
constexpr RGBcolor FIRE_BRICK = 0xB22222;

/**
 * @brief filtered getter for plotting SortableCollection
 *
 * @tparam STATE_A SortableState to filter for
 * @tparam STATE_B SortableState to filter for
 * @param data void* to SortableCollection
 * @param idx index
 * @return ImPlotPoint for bar if states match or {0,0}
 */
template<auto STATE_A, auto STATE_B = STATE_A>
requires sortvis::SortableState<decltype(STATE_A)> && sortvis::SortableState<decltype(STATE_B)> ImPlotPoint
genericGetter(void* data, int idx)
noexcept
{
	const auto& collection = *static_cast<sortvis::SortableCollection*>(data);
	const auto& element = collection[idx];
	if(element == STATE_A && element == STATE_B)
	{
		return ImPlotPoint(idx, element.value);
	}
	return ImPlotPoint(0, 0);
}
constexpr auto writeGetter = genericGetter<sortvis::Sortable::AccessState::Write>;
constexpr auto readGetter = genericGetter<sortvis::Sortable::AccessState::Read>;
constexpr auto fullGetter = genericGetter<sortvis::Sortable::AccessState::None, sortvis::Sortable::SortState::Full>;
constexpr auto partialGetter =
    genericGetter<sortvis::Sortable::AccessState::None, sortvis::Sortable::SortState::Partial>;
constexpr auto noneGetter = genericGetter<sortvis::Sortable::AccessState::None, sortvis::Sortable::SortState::None>;

/**
 * @brief renders the settings area
 *
 * @param data
 *
 * @todo handle imgui sizing bug for slider
 */
void renderSettings(sortvis::GUIData& data)
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(data.windowSize.x, 0));

	ImGui::Begin("Control Window", nullptr, sortvis::flags::SETTINGS);

	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.35f);
	ImGui::SliderFloat("advance delay", &data.advanceDelay, 0.004f, 1.0f, "%.3f", sortvis::flags::DELAY);

	ImGui::SameLine();
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.35f);
	ImGui::SliderInt("elements", &data.elements, 8, 128);

	ImGui::SameLine();
	if(ImGui::Button("reset sorters"))
	{
		data.sortables = sortvis::SortableCollection(data.elements);
		data.sortables.randomize();
		data.sorters.reset(data.sortables);
	}

	ImGui::SameLine();
	ImGui::Checkbox("auto reset", &data.autoReset);

	data.controlSize = ImGui::GetWindowSize();

	ImGui::End();
}

/**
 * @brief helper function to plot SortableCollection
 *
 * @param sorter Sorter to plot
 * @param label
 * @param color
 * @param getter
 */
void plotBars(const sortvis::Sorter& sorter, const sortvis::NumberedString& label, RGBcolor color,
    ImPlotPoint (*getter)(void* data, int idx))
{
	ImPlot::SetNextFillStyle(fromRGB(color));
	ImPlot::PlotBarsG(label.data(), getter,
	    static_cast<void*>(const_cast<sortvis::SortableCollection*>(&sorter.data())),
	    static_cast<int>(sorter.data().size()), 1.0);
}

/**
 * @brief renders the sorters area
 *
 * @param data
 */
void renderSorters(sortvis::GUIData& data)
{
	ImGui::SetNextWindowPos(ImVec2(0, data.controlSize.y));
	ImGui::SetNextWindowSize(ImVec2(data.windowSize.x, data.windowSize.y - data.controlSize.y));
	ImGui::Begin("Sorter Window", nullptr, sortvis::flags::SORTER);

	const ImVec2 plotSize(((data.windowSize.x - 14) / data.sortersPerLine),
	    (((data.windowSize.y - 14) - data.controlSize.y) / (data.sorters.size() / data.sortersPerLine)));

	int sorterLineNumber = 0;
	for(const sortvis::Sorter& sorter : data.sorters)
	{
		if(ImPlot::BeginPlot(sorter.name(), "index", "value", plotSize, sortvis::flags::PLOT, sortvis::flags::XAXIS,
		       sortvis::flags::YAXIS))
		{
			data.barLabels.update(sorter);

			ImPlot::SetLegendLocation(
			    ImPlotLocation_::ImPlotLocation_South, ImPlotOrientation_::ImPlotOrientation_Horizontal, true);

			sortvis::plotBars(sorter, data.barLabels.write(), FIRE_BRICK, writeGetter);
			sortvis::plotBars(sorter, data.barLabels.read(), GOLDEN_ROD, readGetter);
			sortvis::plotBars(sorter, data.barLabels.full(), FOREST_GREEN, fullGetter);
			sortvis::plotBars(sorter, data.barLabels.none(), ROYAL_BLUE, noneGetter);

			ImPlot::EndPlot();
		}
		if(++sorterLineNumber != data.sortersPerLine)
		{
			ImGui::SameLine();
		}
		else
		{
			sorterLineNumber = 0;
		}
	}

	ImGui::End();
}
} // namespace sortvis
