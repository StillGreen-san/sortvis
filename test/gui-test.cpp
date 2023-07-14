#include "gui-help.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("NumberedString")
{
	sortvis::NumberedString numstr("STATIC: ");

	numstr.update(42);

	CHECK(numstr.data() == std::string("STATIC: 42"));

	numstr.update(64);

	CHECK(numstr.data() == std::string("STATIC: 64"));
}

cppcoro::generator<const int> BarGen(std::shared_ptr<sortvis::SortableCollection> data)
{
	co_yield sortvis::algorithms::INIT_MAGIC_VALUE;
	data->less(2, 3);
	data->swap(4, 5);
	data->state(sortvis::Sortable::SortState::Full, 0U, 1U);
}

TEST_CASE("BarLabels")
{
	sortvis::SortableCollection sortabels{{6, 5, 4, 3, 2, 1}};
	sortvis::Sorter sorter(sortabels, BarGen);

	sortvis::BarLabels labels{{"WRITE ", "READ ", "FULL ", "NONE "}};

	while(sorter.advance()) {}

	labels.update(sorter);

	CHECK(labels.write().data() == std::string("WRITE 1"));
	CHECK(labels.read().data() == std::string("READ 1"));
	CHECK(labels.full().data() == std::string("FULL 2"));
	CHECK(labels.none().data() == std::string("NONE 4"));
}
