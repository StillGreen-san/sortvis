#undef IMGUI_USE_WCHAR32 // fix for redef in imconfig

#include "imgui.h" // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h

#include "imgui-SFML.h" // for ImGui::SFML::* functions and SFML-specific overloads

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <implot.h>

#include "sortable.hpp"

constexpr unsigned WINDOW_WIDTH = 1280;
constexpr unsigned WINDOW_HEIGHT = 800;

const char* LABELS[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f", "g", "h", "i",
    "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"};

ImVec4 fromRGB(uint32_t rgb)
{
	return {
	    ((rgb & 0xFF0000) >> 16) / 255.0f, ((rgb & 0x00FF00) >> 8) / 255.0f, ((rgb & 0x0000FF) >> 0) / 255.0f, 1.0f};
}

constexpr uint32_t ROYAL_BLUE = 0x4169E1;
constexpr uint32_t FOREST_GREEN = 0x228B22;
constexpr uint32_t SPRING_GREEN = 0x228B22;
constexpr uint32_t GOLDEN_ROD = 0xDAA520;
constexpr uint32_t FIRE_BRICK = 0xB22222;

void setSortableFillStyle(const sortvis::Sortable& sortable)
{
	using AccessState = sortvis::Sortable::AccessState;
	using SortState = sortvis::Sortable::SortState;

	if(sortable.accessState != AccessState::None)
	{
		switch(sortable.accessState)
		{
		case AccessState::Write:
			ImPlot::SetNextFillStyle(fromRGB(FIRE_BRICK));
			break;
		case AccessState::Read:
			ImPlot::SetNextFillStyle(fromRGB(GOLDEN_ROD));
			break;
		case AccessState::None:
			ImPlot::SetNextFillStyle(fromRGB(ROYAL_BLUE));
			break;
		}
	}
	else
	{
		switch(sortable.sortState)
		{
		case SortState::Full:
			ImPlot::SetNextFillStyle(fromRGB(FOREST_GREEN));
			break;
		case SortState::Partial:
			ImPlot::SetNextFillStyle(fromRGB(SPRING_GREEN));
			break;
		case SortState::None:
			ImPlot::SetNextFillStyle(fromRGB(ROYAL_BLUE));
			break;
		}
	}
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "ImGui + SFML == <3");
	window.setVerticalSyncEnabled(true);

	ImGui::SFML::Init(window);
	ImPlot::CreateContext();
	ImGui::GetIO().IniFilename = nullptr;

	sortvis::SortableCollection sortables(8);
	sortables.randomize();
	sortables.less(0, 2);
	sortables.swap(1, 3);
	sortables.state(sortvis::Sortable::SortState::Partial, size_t(5));
	sortables.state(sortvis::Sortable::SortState::Full, size_t(6));

	sf::Clock deltaClock;
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

		const char** label = LABELS;

		ImGui::SFML::Update(window, deltaClock.restart());

		constexpr auto plotFlags = static_cast<ImPlotFlags_>(ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMousePos);
		constexpr auto axisFlagsX = static_cast<ImPlotAxisFlags_>(
		    ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks);
		constexpr auto axisFlagsY = static_cast<ImPlotAxisFlags_>(ImPlotAxisFlags_AutoFit);
		constexpr auto windowFlags =
		    static_cast<ImGuiWindowFlags_>(ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoDecoration);

		auto MyBars = [&](const char* title_id, const char* /*label_id*/)
		{
			if(ImPlot::BeginPlot(title_id, "index", "value", ImVec2(0, 0), plotFlags, axisFlagsX, axisFlagsY))
			{
				double shift = 0.0;
				for(const sortvis::Sortable& sortable : sortables)
				{
					setSortableFillStyle(sortable);
					ImPlot::PlotBars(*label++, &sortable.value, 1, 1.0, shift++);
				}
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