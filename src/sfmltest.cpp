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

ImPlotPoint valueGetter(void* data, int idx)
{
	// sortvis::SortableCollection* sortables = reinterpret_cast<sortvis::SortableCollection*>(data);
	const sortvis::Sortable& sortable = reinterpret_cast<sortvis::SortableCollection*>(data)->begin()[idx];
	// return {static_cast<double>(idx), static_cast<double>(sortables->begin()[idx].value)};
	// multicolor with multiple bars?
	return {static_cast<double>(idx), static_cast<double>(sortable.value)};
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

		ImGui::SFML::Update(window, deltaClock.restart());

		constexpr auto plotFlags = static_cast<ImPlotFlags_>(ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMousePos);
		constexpr auto axisFlagsX = static_cast<ImPlotAxisFlags_>(
		    ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks);
		constexpr auto axisFlagsY = static_cast<ImPlotAxisFlags_>(ImPlotAxisFlags_AutoFit);
		constexpr auto windowFlags =
		    static_cast<ImGuiWindowFlags_>(ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoDecoration);

		auto MyBars = [&](const char* title_id, const char* label_id)
		{
			if(ImPlot::BeginPlot(title_id, "index", "value", ImVec2(0, 0), plotFlags, axisFlagsX, axisFlagsY))
			{
				// ImPlot::PlotBars(label_id, &sortables.begin()->value, static_cast<int>(sortables.size()), 1.0, 0.0,
				// 0, sizeof(sortvis::Sortable));
				ImPlot::PlotBarsG(
				    label_id, valueGetter, static_cast<void*>(&sortables), static_cast<int>(sortables.size()), 1.0, 0);
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