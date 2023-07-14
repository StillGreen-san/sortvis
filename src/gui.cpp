#include "gui.hpp"

namespace
{
/**
 * @brief contains flag constants for the gui
 *
 */
namespace flags
{
constexpr auto PLOT =
    static_cast<ImPlotFlags_>(ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMouseText | ImPlotFlags_NoMenus);
constexpr auto LEGEND = static_cast<ImPlotLegendFlags_>(
    ImPlotLegendFlags_NoHighlightAxis | ImPlotLegendFlags_NoHighlightItem | ImPlotLegendFlags_Horizontal);
constexpr auto XAXIS = static_cast<ImPlotAxisFlags_>(
    ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoLabel);
constexpr auto YAXIS = static_cast<ImPlotAxisFlags_>(ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoLabel);
constexpr auto SORTER = static_cast<ImGuiWindowFlags_>(ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoDecoration);
constexpr auto SETTINGS = static_cast<ImGuiWindowFlags_>(ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
constexpr auto DELAY = ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic;
} // namespace flags

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
    requires sortvis::SortableState<decltype(STATE_A)> && sortvis::SortableState<decltype(STATE_B)>
ImPlotPoint genericGetter(int idx, void* data) noexcept
{
	const auto& collection = *static_cast<sortvis::SortableCollection*>(data);
	const auto& element = collection[idx];
	if(element == STATE_A && element == STATE_B)
	{
		return {static_cast<double>(idx), static_cast<double>(element.value)};
	}
	return {0, 0};
}
constexpr auto writeGetter = genericGetter<sortvis::Sortable::AccessState::Write>;
constexpr auto readGetter = genericGetter<sortvis::Sortable::AccessState::Read>;
constexpr auto fullGetter = genericGetter<sortvis::Sortable::AccessState::None, sortvis::Sortable::SortState::Full>;
constexpr auto noneGetter = genericGetter<sortvis::Sortable::AccessState::None, sortvis::Sortable::SortState::None>;

/**
 * @brief helper function to plot SortableCollection
 *
 * @param sorter Sorter to plot
 * @param label
 * @param color
 * @param getter
 */
void plotBars(const sortvis::Sorter& sorter, const sortvis::NumberedString& label, RGBcolor color,
    ImPlotPoint (*getter)(int idx, void* data))
{
	ImPlot::SetNextFillStyle(fromRGB(color));
	ImPlot::PlotBarsG(label.data(), getter,
	    static_cast<void*>(const_cast<sortvis::SortableCollection*>(&sorter.data())),
	    static_cast<int>(sorter.data().size()), 1.0);
}

constexpr int OUTER_BORDER_MARGIN = 14;
constexpr int INNER_X_BORDER_MARGIN = 8;
constexpr int INNER_Y_BORDER_MARGIN = 4;

/**
 * @brief renders the settings area
 *
 * @param data
 */
void renderSettings(sortvis::GUIData& data)
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(data.windowSize.x, 0));

	ImGui::Begin("Control Window", nullptr, flags::SETTINGS);

	if(ImGui::Button("?"))
	{
		data.showAbout = true;
	}

	ImGui::SameLine();
	if(ImGui::Button("reset sorters"))
	{
		data.resetSorters();
	}

	ImGui::SameLine();
	ImGui::Checkbox("auto reset", &data.autoReset);

	const float sliderWidth = (ImGui::GetWindowWidth() - 409) / 2;

	ImGui::SameLine();
	ImGui::PushItemWidth(sliderWidth);
	ImGui::SliderFloat("advance delay", &data.advanceDelay, 0.004f, 1.0f, "%.3f", flags::DELAY);

	ImGui::SameLine();
	ImGui::PushItemWidth(sliderWidth);
	ImGui::SliderInt("elements", &data.elements, 8, 128);

	data.controlSize = ImGui::GetWindowSize();

	ImGui::End();
}

/**
 * @brief renders the sorters area
 *
 * @todo better handle small window sizes
 *
 * @param data
 */
void renderSorters(sortvis::GUIData& data)
{
	ImGui::SetNextWindowPos(ImVec2(0, data.controlSize.y));
	ImGui::SetNextWindowSize(ImVec2(data.windowSize.x, data.windowSize.y - data.controlSize.y));
	ImGui::Begin("Sorter Window", nullptr, flags::SORTER);

	const ImVec2 plotSize(
	    ((data.windowSize.x - OUTER_BORDER_MARGIN) - ((data.sortersPerRow - 1) * INNER_X_BORDER_MARGIN)) /
	        data.sortersPerRow,
	    (((data.windowSize.y - OUTER_BORDER_MARGIN) - ((data.sortersPerCol - 1) * INNER_Y_BORDER_MARGIN)) -
	        data.controlSize.y) /
	        data.sortersPerCol);

	int sorterLineNumber = 0;
	for(const sortvis::Sorter& sorter : data.sorters)
	{
		if(ImPlot::BeginPlot(sorter.name(), plotSize, flags::PLOT))
		{
			ImPlot::SetupLegend(ImPlotLocation_::ImPlotLocation_South, flags::LEGEND);
			ImPlot::SetupAxes("index", "value", flags::XAXIS, flags::YAXIS);

			data.barLabels.update(sorter);

			plotBars(sorter, data.barLabels.write(), FIRE_BRICK, writeGetter);
			plotBars(sorter, data.barLabels.read(), GOLDEN_ROD, readGetter);
			plotBars(sorter, data.barLabels.full(), FOREST_GREEN, fullGetter);
			plotBars(sorter, data.barLabels.none(), ROYAL_BLUE, noneGetter);

			ImPlot::EndPlot();
		}
		if(++sorterLineNumber != data.sortersPerRow)
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

void addAboutRow(const char* name, const char* license, const char* link)
{
	ImGui::TableNextRow();
	ImGui::TableNextColumn();
	ImGui::TextUnformatted(name);
	ImGui::TableNextColumn();
	ImGui::TextUnformatted(license);
	ImGui::TableNextColumn();
	if(name == nullptr)
	{
		ImGui::TextUnformatted(link);
	}
	else if(ImGui::Selectable(link, false, ImGuiSelectableFlags_DontClosePopups))
	{
		ImGui::SetClipboardText(link);
	}
}
} // namespace

void sortvis::render(sortvis::GUIData& data)
{
	renderSettings(data);
	renderSorters(data);

	if(!data.showAbout)
	{
		return;
	}

	ImGui::OpenPopup("About");
	if(ImGui::BeginPopupModal("About", &data.showAbout, ImGuiWindowFlags_NoResize))
	{
		ImGui::TextUnformatted("this program uses the following libraries:\n");

		if(ImGui::BeginTable("AboutTableID", 3))
		{
			ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed);

			addAboutRow("Dear IMGUI", "[MIT]", "https://github.com/ocornut/imgui");
			addAboutRow("ImPlot", "[MIT]", "https://github.com/epezent/implot");
			addAboutRow("SFML", "[ZLIB]", "https://github.com/SFML/SFML");
			addAboutRow("ImGui-SFML", "[MIT]", "https://github.com/eliasdaler/imgui-sfml");
			addAboutRow("CppCoro", "[MIT]", "https://github.com/lewissbaker/cppcoro");
			addAboutRow(nullptr, nullptr, "for testing only:");
			addAboutRow("Catch2", "[BSL]", "https://github.com/catchorg/Catch2");

			ImGui::EndTable();
		}

		ImGui::EndPopup();
	}
}
