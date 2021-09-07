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

	sortvis::SortableCollection sortables(8);
	sortables.randomize();
	sortables.less(0, 2);
	sortables.swap(1, 3);
	sortables.state(sortvis::Sortable::SortState::Partial, size_t(5));
	sortables.state(sortvis::Sortable::SortState::Full, size_t(6));

	sortvis::SorterCollection sorters(
	    sortables, {sortvis::algorithms::bubble, sortvis::algorithms::quick, sortvis::algorithms::shell});

	sf::Clock deltaClock;
	float advanceDelta = 0;
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

		if(advanceDelta > 1.0f)
		{
			advanceDelta = 0.0f;
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

		auto MyBars = [&](const char* title_id, const char* /*label_id*/)
		{
			if(ImPlot::BeginPlot(title_id, "index", "value", ImVec2(0, 0), plotFlags, axisFlagsX, axisFlagsY))
			{
				ImPlot::SetLegendLocation(
				    ImPlotLocation_::ImPlotLocation_South, ImPlotOrientation_::ImPlotOrientation_Horizontal, true);
				ImPlot::SetNextFillStyle(fromRGB(FIRE_BRICK));
				ImPlot::PlotBarsG("write", writeGetter, &sortables, (int)sortables.size(), 1.0);
				ImPlot::SetNextFillStyle(fromRGB(GOLDEN_ROD));
				ImPlot::PlotBarsG("read", readGetter, &sortables, (int)sortables.size(), 1.0);
				ImPlot::SetNextFillStyle(fromRGB(FOREST_GREEN));
				ImPlot::PlotBarsG("sorted", fullyGetter, &sortables, (int)sortables.size(), 1.0);
				ImPlot::SetNextFillStyle(fromRGB(SPRING_GREEN));
				ImPlot::PlotBarsG("partial", partialGetter, &sortables, (int)sortables.size(), 1.0);
				ImPlot::SetNextFillStyle(fromRGB(ROYAL_BLUE));
				ImPlot::PlotBarsG("none", noneGetter, &sortables, (int)sortables.size(), 1.0);
				ImPlot::EndPlot();
			}
		};

		ImGui::Begin("My Awesome Window", nullptr, windowFlags);
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(ImGui::GetMainViewport()->Size);

		MyBars("Some Plot1", "Some Bars1");
		ImGui::SameLine();
		MyBars("Some Plot2", "Some Bars2");
		MyBars("Some Plot3", "Some Bars3");

		ImGui::End();

		window.clear();
		ImGui::SFML::Render(window);
		window.display();
	}

	ImPlot::DestroyContext();
	ImGui::SFML::Shutdown();

	return 0;
}