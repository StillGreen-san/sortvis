#include "gui.hpp"

#include "imgui-SFML.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include <iostream>

constexpr unsigned FRAMERATE = 30;

int main()
{
	try
	{
		sf::VideoMode windowvidmode = sf::VideoMode::getDesktopMode();
		windowvidmode.height /= 2;
		windowvidmode.width /= 2;

		sf::RenderWindow window(windowvidmode, "Sorting Visualization");
		// window.setVerticalSyncEnabled(true);
		window.setFramerateLimit(FRAMERATE);

		ImGui::SFML::Init(window);
		ImPlot::CreateContext();
		ImGui::GetIO().IniFilename = nullptr;

		sortvis::GUIData data;

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

			data.update();

			ImGui::SFML::Update(window, data.deltaTime);

			sortvis::renderSettings(data);
			sortvis::renderSorters(data);

			window.clear();
			ImGui::SFML::Render(window);
			window.display();
		}

		ImPlot::DestroyContext();
		ImGui::SFML::Shutdown();
	}
	catch(sortvis::InitFailureException& e)
	{
		std::cerr << "[InitFailureException] " << e.what() << std::endl;
	}
	catch(std::exception& e)
	{
		std::cerr << "[StdException] " << e.what() << std::endl;
	}
	catch(...)
	{
		std::cerr << "[UNKOWN_EXCEPTION] !" << std::endl;
	}
}
