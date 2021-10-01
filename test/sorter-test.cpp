#include "sorter.hpp"

#include <catch2/catch.hpp>

cppcoro::generator<const int> EarlyFinishGen(std::shared_ptr<sortvis::SortableCollection> data)
{
	if(data->size() == 0) //! should not be hit
	{
		co_yield -1; //* some return needed to compile
	}
}

cppcoro::generator<const int> NoInitValueGen(std::shared_ptr<sortvis::SortableCollection>)
{
	co_yield sortvis::algorithms::SWAP_MAGIC_VALUE;
}

cppcoro::generator<const int> ChangedStateGen(std::shared_ptr<sortvis::SortableCollection> data)
{
	data->swap(0, 1);

	co_yield sortvis::algorithms::INIT_MAGIC_VALUE;
}

cppcoro::generator<const int> ValidGen(std::shared_ptr<sortvis::SortableCollection> data)
{
	co_yield sortvis::algorithms::INIT_MAGIC_VALUE;
}

TEST_CASE("Sorter::Sorter")
{
	sortvis::SortableCollection sortables({1, 2, 3, 4, 5, 6});

	SECTION("EarlyFinish")
	{
		REQUIRE_THROWS_AS(sortvis::Sorter(sortables, EarlyFinishGen), sortvis::InitFailureException);
	}

	SECTION("NoInitValue")
	{
		REQUIRE_THROWS_AS(sortvis::Sorter(sortables, NoInitValueGen), sortvis::InitFailureException);
	}

	SECTION("ChangedState")
	{
		REQUIRE_THROWS_AS(sortvis::Sorter(sortables, ChangedStateGen), sortvis::InitFailureException);
	}

	SECTION("ValidGen")
	{
		REQUIRE_NOTHROW(sortvis::Sorter(sortables, ValidGen));
	}
}
