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

cppcoro::generator<const int> ValidGen(std::shared_ptr<sortvis::SortableCollection>)
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
		std::unique_ptr<sortvis::Sorter> sorter;

		REQUIRE_NOTHROW(sorter = std::make_unique<sortvis::Sorter>(sortables, sortvis::algorithms::bubble));

		CHECK_FALSE(sorter->hasFinished());
		CHECK(std::strcmp(sorter->name(), "Bubble Sort") == 0);
		CHECK(sorter->data() == sortables);
	}
}

cppcoro::generator<const int> AdvanceGen(std::shared_ptr<sortvis::SortableCollection> data)
{
	co_yield sortvis::algorithms::INIT_MAGIC_VALUE;

	data->swap(0, 1);
	co_yield sortvis::algorithms::SWAP_MAGIC_VALUE;

	data->swap(2, 3);
	co_yield sortvis::algorithms::SWAP_MAGIC_VALUE;
}

TEST_CASE("Sorter::advance")
{
	sortvis::SortableCollection sortables({1, 5, 3, 2, 6, 4});

	sortvis::Sorter sorter(sortables, AdvanceGen);

	CHECK(sorter.advance());

	REQUIRE(sorter[0] == sortables[1]);
	REQUIRE(sorter[1] == sortables[0]);

	CHECK(sorter.advance());

	REQUIRE(sorter[2] == sortables[3]);
	REQUIRE(sorter[3] == sortables[2]);

	CHECK_FALSE(sorter.advance());
	CHECK_FALSE(sorter.advance());
}

// TEST_CASE("Sorter::reset")
// {
// 	sortvis::SortableCollection sortablesA({3, 1, 2, 4, 5, 6});
// 	sortvis::SortableCollection sortablesB({1, 5, 3, 2, 6, 4});

// 	sortvis::Sorter sorter(sortablesA, sortvis::algorithms::bubble);

// 	// CHECK_FALSE(sorter.reset());
// }
