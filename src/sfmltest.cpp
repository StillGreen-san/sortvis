#undef IMGUI_USE_WCHAR32 // fix for redef in imconfig

#include "imgui.h" // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h

#include "imgui-SFML.h" // for ImGui::SFML::* functions and SFML-specific overloads

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <implot.h>

#include "sortable.hpp"
#include "sorter.hpp"

constexpr unsigned WINDOW_WIDTH = 1280;
constexpr unsigned WINDOW_HEIGHT = 800;

ImVec4 fromRGB(uint32_t rgb)
{
	return {
	    ((rgb & 0xFF0000) >> 16) / 255.0f, ((rgb & 0x00FF00) >> 8) / 255.0f, ((rgb & 0x0000FF) >> 0) / 255.0f, 1.0f};
}

constexpr uint32_t ROYAL_BLUE = 0x4169E1;
constexpr uint32_t FOREST_GREEN = 0x228B22;
constexpr uint32_t SPRING_GREEN = 0x00FF7F;
constexpr uint32_t GOLDEN_ROD = 0xDAA520;
constexpr uint32_t FIRE_BRICK = 0xB22222;

template<auto STATE_A, auto STATE_B = STATE_A>
requires sortvis::SortableState<decltype(STATE_A)> && sortvis::SortableState<decltype(STATE_B)> ImPlotPoint
genericGetter(void* data, int idx)
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
constexpr auto fullyGetter = genericGetter<sortvis::Sortable::AccessState::None, sortvis::Sortable::SortState::Full>;
constexpr auto partialGetter =
    genericGetter<sortvis::Sortable::AccessState::None, sortvis::Sortable::SortState::Partial>;
constexpr auto noneGetter = genericGetter<sortvis::Sortable::AccessState::None, sortvis::Sortable::SortState::None>;

int main()
{
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "ImGui + SFML == <3");
	// window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(30);

	ImGui::SFML::Init(window);
	ImPlot::CreateContext();
	ImGui::GetIO().IniFilename = nullptr;

	sortvis::SortableCollection sortables(32);
	sortables.randomize();

	sortvis::SorterCollection sorters(
	    sortables, {sortvis::algorithms::bubble, sortvis::algorithms::quick, sortvis::algorithms::shell,
	                   sortvis::algorithms::heap, sortvis::algorithms::insertion, sortvis::algorithms::selection});

	sf::Clock deltaClock;
	float advanceDelta = 0;
	float advanceDelay = 0.2f;
	while(window.isOpen())
	{
		sf::Event event{sf::Event::Count, 0}; //! meaningless event init for tidy
		while(window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			if(event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		sf::Time delta = deltaClock.restart();
		advanceDelta += delta.asSeconds();

		ImGui::SFML::Update(window, delta);

		if(advanceDelta > advanceDelay)
		{
			advanceDelta -= advanceDelay;
			if(sorters.allHaveFinished())
			{
				sortables.randomize();
				sorters.reset(sortables);
			}
			else
				sorters.advance();
		}

		constexpr auto plotFlags = static_cast<ImPlotFlags_>(
		    ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMousePos | ImPlotFlags_NoHighlight | ImPlotFlags_NoMenus);
		constexpr auto axisFlagsX =
		    static_cast<ImPlotAxisFlags_>(ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoGridLines |
		                                  ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoLabel);
		constexpr auto axisFlagsY = static_cast<ImPlotAxisFlags_>(ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoLabel);
		constexpr auto windowFlags =
		    static_cast<ImGuiWindowFlags_>(ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoDecoration);

		ImVec2 windowSize = ImGui::GetMainViewport()->Size;

		ImGui::Begin("Control Window", nullptr, ImGuiWindowFlags_NoDecoration);
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SliderFloat("advance delay", &advanceDelay, 0.05f, 0.75f);
		ImVec2 controlSize = ImGui::GetWindowSize();
		ImGui::End();

		ImGui::Begin("My Awesome Window", nullptr, windowFlags);
		ImGui::SetWindowPos(ImVec2(0, controlSize.y));
		ImGui::SetWindowSize(ImVec2(windowSize.x, windowSize.y - controlSize.y));

		int sorterNumber = 0;
		for(const sortvis::Sorter& sorter : sorters)
		{
			if(ImPlot::BeginPlot(sorter.name(), "index", "value", ImVec2(0, 0), plotFlags, axisFlagsX, axisFlagsY))
			{
				ImPlot::SetLegendLocation(
				    ImPlotLocation_::ImPlotLocation_South, ImPlotOrientation_::ImPlotOrientation_Horizontal, true);
				ImPlot::SetNextFillStyle(fromRGB(FIRE_BRICK));
				ImPlot::PlotBarsG("write", writeGetter, (void*)(&sorter.data()), (int)sortables.size(), 1.0);
				ImPlot::SetNextFillStyle(fromRGB(GOLDEN_ROD));
				ImPlot::PlotBarsG("read", readGetter, (void*)(&sorter.data()), (int)sortables.size(), 1.0);
				ImPlot::SetNextFillStyle(fromRGB(FOREST_GREEN));
				ImPlot::PlotBarsG("sorted", fullyGetter, (void*)(&sorter.data()), (int)sortables.size(), 1.0);
				ImPlot::SetNextFillStyle(fromRGB(SPRING_GREEN));
				ImPlot::PlotBarsG("partial", partialGetter, (void*)(&sorter.data()), (int)sortables.size(), 1.0);
				ImPlot::SetNextFillStyle(fromRGB(ROYAL_BLUE));
				ImPlot::PlotBarsG("none", noneGetter, (void*)(&sorter.data()), (int)sortables.size(), 1.0);
				ImPlot::EndPlot();
			}
			if(++sorterNumber % 3)
				ImGui::SameLine();
		}

		ImGui::End();

		window.clear();
		ImGui::SFML::Render(window);
		window.display();
	}

	ImPlot::DestroyContext();
	ImGui::SFML::Shutdown();

	return 0;
}