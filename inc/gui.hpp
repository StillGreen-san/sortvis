#pragma once

#undef IMGUI_USE_WCHAR32 // fix for redef in imconfig

#include <imgui.h>

#include <SFML/System/Clock.hpp>

#include <implot.h>

#include "gui-help.hpp"
#include "sortable.hpp"
#include "sorter.hpp"

namespace sortvis
{
constexpr unsigned FRAMERATE = 30;

/**
 * @brief clears least significant bit to make unsigned number even
 *
 * @tparam T std::unsigned_integral
 * @param n number of T
 * @return T even n
 */
template<std::unsigned_integral T>
T makeEven(T n)
{
	return n & ~1;
}

/**
 * @brief class containing data used in gui, call update before drawing a frame
 *
 */
class GUIData
{
public:
	int elements = 32;
	int sortersPerRow = 3;
	int sortersPerCol = 2;
	float advanceDelta = 0;
	float advanceDelay = 0.2f;
	bool autoReset = false;
	bool showAbout = false;

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
	 * @brief resize to 'elements' and randomize 'sortables'
	 *
	 */
	void resetSorters()
	{
		sortables = sortvis::SortableCollection(elements);
		sortables.randomize();
		sorters.reset(sortables);
	}

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
				resetSorters();
			}
		}

		windowSize = ImGui::GetMainViewport()->Size;

		const size_t sorterCount = sorters.size();
		float smallestAspectDeviation = 99;
		constexpr float desiredYRatio = 0.5625f;

		for(size_t cols = 1; cols <= sorterCount; ++cols)
		{
			size_t rows = makeEven(sorterCount) / cols;
			if(rows * cols >= sorterCount)
			{
				float aspectRatio = (windowSize.y / rows) / (windowSize.x / cols);
				float aspectDeviation = std::abs(aspectRatio - desiredYRatio);
				if(aspectDeviation < smallestAspectDeviation)
				{
					smallestAspectDeviation = aspectDeviation;
					sortersPerRow = static_cast<int>(cols);
					sortersPerCol = static_cast<int>(rows);
				}
			}
		}
	}
};

/**
 * @brief render gui (settings & sorters)
 *
 * @param data gui data
 */
void render(sortvis::GUIData& data);
} // namespace sortvis
