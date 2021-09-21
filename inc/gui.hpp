#pragma once

#undef IMGUI_USE_WCHAR32 // fix for redef in imconfig

#include "imgui.h"

#include <SFML/System/Clock.hpp>

#include <implot.h>

#include "sortable.hpp"
#include "sorter.hpp"

#include <charconv>
#include <string>

namespace sortvis
{
namespace flags
{
constexpr auto PLOT = static_cast<ImPlotFlags_>(
    ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMousePos | ImPlotFlags_NoHighlight | ImPlotFlags_NoMenus);
constexpr auto XAXIS = static_cast<ImPlotAxisFlags_>(
    ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoLabel);
constexpr auto YAXIS = static_cast<ImPlotAxisFlags_>(ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoLabel);
constexpr auto SORTER = static_cast<ImGuiWindowFlags_>(ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoDecoration);
constexpr auto SETTINGS = static_cast<ImGuiWindowFlags_>(ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
} // namespace flags

constexpr unsigned FRAMERATE = 30;

struct NumberedString
{
	std::array<char, 16> data{'\0'};
	char* staticEnd;
	NumberedString(const char* str)
	{
		size_t len = std::strlen(str);
		std::strncpy(data.data(), str, len);
		staticEnd = data.data() + len;
	}
	void update(unsigned num)
	{
		auto result = std::to_chars(staticEnd, data.data() + data.size(), num);
		*result.ptr = '\0';
	}
};

struct BarLabels
{
	std::array<NumberedString, 4> data{"swap ", "compare ", "sorted ", "none "};
	sortvis::NumberedString& write()
	{
		return data[0];
	}
	sortvis::NumberedString& read()
	{
		return data[1];
	}
	sortvis::NumberedString& full()
	{
		return data[2];
	}
	sortvis::NumberedString& none()
	{
		return data[3];
	}
	void update(const sortvis::Sorter& sorter)
	{
		write().update(sorter.data().getCounter(sortvis::Sortable::AccessState::Write));
		read().update(sorter.data().getCounter(sortvis::Sortable::AccessState::Read));
		full().update(sorter.data().getCounter(sortvis::Sortable::SortState::Full));
		none().update(sorter.data().getCounter(sortvis::Sortable::SortState::None));
	}
};

/**
 * @brief class containing data used in gui, call update before drawing a frame
 *
 */
class GUIData
{
public:
	int elements = 32;
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

		if(advanceDelta > advanceDelay)
		{
			advanceDelta = 0;
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
	}
};

/**
 * @brief converts 24bit rgb to ImVec4
 *
 * @param rgb 24bit color value
 * @return ImVec4 rgba float color
 */
ImVec4 fromRGB(uint32_t rgb) noexcept
{
	return {
	    ((rgb & 0xFF0000) >> 16) / 255.0f, ((rgb & 0x00FF00) >> 8) / 255.0f, ((rgb & 0x0000FF) >> 0) / 255.0f, 1.0f};
}

constexpr uint32_t ROYAL_BLUE = 0x4169E1;
constexpr uint32_t FOREST_GREEN = 0x228B22;
constexpr uint32_t SPRING_GREEN = 0x00FF7F;
constexpr uint32_t GOLDEN_ROD = 0xDAA520;
constexpr uint32_t FIRE_BRICK = 0xB22222;

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
	const auto& element = collection.begin()[idx];
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
 */
void renderSettings(sortvis::GUIData& data)
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(data.windowSize.x, 0));

	ImGui::Begin("Control Window", nullptr, sortvis::flags::SETTINGS);

	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.35f);
	ImGui::SliderFloat("advance delay", &data.advanceDelay, 1.f / sortvis::FRAMERATE, 0.75f);

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

void plotBars(const sortvis::Sorter& sorter, const sortvis::NumberedString& label, uint32_t color,
    ImPlotPoint (*getter)(void* data, int idx))
{
	ImPlot::SetNextFillStyle(fromRGB(color));
	ImPlot::PlotBarsG(label.data.data(), getter,
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

	const ImVec2 plotSize((data.windowSize.x / 3) - 10, ((data.windowSize.y - data.controlSize.y) / 2) - 9);

	int sorterNumber = 0;
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
		if(++sorterNumber != 3)
		{
			ImGui::SameLine();
		}
	}

	ImGui::End();
}
} // namespace sortvis
