#include "sortable.hpp"

#include <catch2/catch_test_macros.hpp>

bool operator==(const std::vector<int>& vec, const std::initializer_list<int>& lst)
{
	if(vec.size() != lst.size())
		return false;
	auto vecIt = vec.begin();
	auto lstIt = lst.begin();
	const auto vecEnd = vec.end();
	for(; vecIt != vecEnd; ++vecIt, ++lstIt)
	{
		if(*vecIt != *lstIt)
			return false;
	}
	return true;
}

TEST_CASE("SortableCollection::SortableCollection({})")
{
	const std::initializer_list<int> expected = {1, 2, 3, 4, 5, 6};

	const sortvis::SortableCollection sortables(expected);

	const std::vector<int> actual(sortables.begin(), sortables.end());

	CHECK(sortables.size() == expected.size());
	CHECK(actual == expected);

	CHECK(sortables.getCounter(sortvis::Sortable::AccessState::Read) == 0);
	CHECK(sortables.getCounter(sortvis::Sortable::AccessState::Write) == 0);

	CHECK(sortables.getCounter(sortvis::Sortable::SortState::None) == expected.size());
	CHECK(sortables.getCounter(sortvis::Sortable::SortState::Full) == 0);
}

TEST_CASE("SortableCollection::SortableCollection(6, false)")
{
	const std::initializer_list<int> expected = {1, 2, 3, 4, 5, 6};

	const sortvis::SortableCollection sortables(6, false);

	const std::vector<int> actual(sortables.begin(), sortables.end());

	CHECK(sortables.size() == expected.size());
	CHECK(actual == expected);

	CHECK(sortables.getCounter(sortvis::Sortable::AccessState::Read) == 0);
	CHECK(sortables.getCounter(sortvis::Sortable::AccessState::Write) == 0);

	CHECK(sortables.getCounter(sortvis::Sortable::SortState::None) == expected.size());
	CHECK(sortables.getCounter(sortvis::Sortable::SortState::Full) == 0);
}

TEST_CASE("SortableCollection::SortableCollection(6, true)")
{
	const std::initializer_list<int> expected = {1, 2, 3, 4, 5, 6};

	const sortvis::SortableCollection sortables(6, true);

	CHECK(sortables.size() == expected.size());
	for(int i : expected)
	{
		CHECK(std::find(sortables.begin(), sortables.end(), i) != sortables.end());
	}

	CHECK(sortables.getCounter(sortvis::Sortable::AccessState::Read) == 0);
	CHECK(sortables.getCounter(sortvis::Sortable::AccessState::Write) == 0);

	CHECK(sortables.getCounter(sortvis::Sortable::SortState::None) == expected.size());
	CHECK(sortables.getCounter(sortvis::Sortable::SortState::Full) == 0);
}

TEST_CASE("SortableCollection::randomize()")
{
	const std::initializer_list<int> initial = {1, 2, 3, 4, 5, 6};

	sortvis::SortableCollection sortables(initial);

	sortables.randomize();

	const std::vector<int> shuffled(sortables.begin(), sortables.end());

	REQUIRE_FALSE(shuffled == initial);

	CHECK(sortables.size() == initial.size());
	for(int i : initial)
	{
		CHECK(std::find(sortables.begin(), sortables.end(), i) != sortables.end());
	}
}

TEST_CASE("SortableCollection::randomize(seed)")
{
	const std::initializer_list<int> initial = {1, 2, 3, 4, 5, 6};
	constexpr unsigned seedA = 73195;
	constexpr unsigned seedB = 46820;

	sortvis::SortableCollection seededA(initial);
	sortvis::SortableCollection seededB(initial);

	seededA.randomize(seedA);
	seededB.randomize(seedB);

	CHECK(seededA != seededB);

	seededB.randomize(seedA);

	CHECK(seededA == seededB);

	seededA.randomize(seedB);
	seededB.randomize(seedB);

	CHECK(seededA == seededB);
}

TEST_CASE("SortableCollection::reset")
{
	const std::initializer_list<int> initialA = {1, 2, 3, 4, 5, 6};
	const std::initializer_list<int> initialB = {6, 4, 5, 2, 1, 3};

	sortvis::SortableCollection sortablesA(initialA);
	sortvis::SortableCollection sortablesB(initialB);

	sortablesA.greater(0, 1);
	sortablesA.state(sortvis::Sortable::SortState::Full, 4U, 5U);
	sortablesB.greater(0, 1);
	sortablesB.state(sortvis::Sortable::SortState::Full, 4U, 5U);

	sortablesA.reset(sortablesB);

	CHECK(sortablesA == sortablesB);

	CHECK(sortablesA.getCounter(sortvis::Sortable::SortState::Full) == 0);
	CHECK(sortablesA.getCounter(sortvis::Sortable::AccessState::Read) == 0);
}

TEST_CASE("SortableCollection::less")
{
	const std::initializer_list<int> initial = {1, 2, 3, 4, 5, 6};

	sortvis::SortableCollection sortables(initial);

	CHECK(sortables.less(0, 1));

	REQUIRE(sortables.getCounter(sortvis::Sortable::AccessState::Read) == 1);
	REQUIRE(sortables.getCounter(sortvis::Sortable::AccessState::Write) == 0);
	REQUIRE(sortables[0] == sortvis::Sortable::AccessState::Read);
	REQUIRE(sortables[1] == sortvis::Sortable::AccessState::Read);

	CHECK(sortables.less(0, 2));
	CHECK_FALSE(sortables.less(3, 0));

	REQUIRE(sortables.getCounter(sortvis::Sortable::AccessState::Read) == 3);
	REQUIRE(sortables.getCounter(sortvis::Sortable::AccessState::Write) == 0);
	REQUIRE(sortables[2] == sortvis::Sortable::AccessState::Read);
	REQUIRE(sortables[3] == sortvis::Sortable::AccessState::Read);

	REQUIRE(sortables.getCounter(sortvis::Sortable::SortState::Full) == 0);
}

TEST_CASE("SortableCollection::greater")
{
	const std::initializer_list<int> initial = {1, 2, 3, 4, 5, 6};

	sortvis::SortableCollection sortables(initial);

	CHECK(sortables.greater(1, 0));

	REQUIRE(sortables.getCounter(sortvis::Sortable::AccessState::Read) == 1);
	REQUIRE(sortables.getCounter(sortvis::Sortable::AccessState::Write) == 0);
	REQUIRE(sortables[0] == sortvis::Sortable::AccessState::Read);
	REQUIRE(sortables[1] == sortvis::Sortable::AccessState::Read);

	CHECK(sortables.greater(2, 0));
	CHECK_FALSE(sortables.greater(0, 3));

	REQUIRE(sortables.getCounter(sortvis::Sortable::AccessState::Read) == 3);
	REQUIRE(sortables.getCounter(sortvis::Sortable::AccessState::Write) == 0);
	REQUIRE(sortables[2] == sortvis::Sortable::AccessState::Read);
	REQUIRE(sortables[3] == sortvis::Sortable::AccessState::Read);

	REQUIRE(sortables.getCounter(sortvis::Sortable::SortState::Full) == 0);
}

TEST_CASE("SortableCollection::swap")
{
	const std::initializer_list<int> initial = {1, 2, 3, 4, 5, 6};

	sortvis::SortableCollection sortables(initial);

	sortables.swap(0, 1);

	REQUIRE(sortables.getCounter(sortvis::Sortable::AccessState::Read) == 0);
	REQUIRE(sortables.getCounter(sortvis::Sortable::AccessState::Write) == 1);
	REQUIRE(sortables[0] == sortvis::Sortable::AccessState::Write);
	REQUIRE(sortables[1] == sortvis::Sortable::AccessState::Write);

	sortables.swap(0, 2);
	sortables.swap(0, 3);

	REQUIRE(sortables.getCounter(sortvis::Sortable::AccessState::Read) == 0);
	REQUIRE(sortables.getCounter(sortvis::Sortable::AccessState::Write) == 3);
	REQUIRE(sortables[2] == sortvis::Sortable::AccessState::Write);
	REQUIRE(sortables[3] == sortvis::Sortable::AccessState::Write);

	REQUIRE(sortables.getCounter(sortvis::Sortable::SortState::Full) == 0);
}

TEST_CASE("SortableCollection::state")
{
	const std::initializer_list<int> initial = {1, 2, 3, 4, 5, 6};

	sortvis::SortableCollection sortables(initial);

	sortables.state(sortvis::Sortable::AccessState::Read, 0U, 1U, 5U);

	REQUIRE(sortables.getCounter(sortvis::Sortable::AccessState::Read) == 0);
	REQUIRE(sortables[0] == sortvis::Sortable::AccessState::Read);
	REQUIRE(sortables[1] == sortvis::Sortable::AccessState::Read);
	REQUIRE(sortables[5] == sortvis::Sortable::AccessState::Read);

	sortables.state(sortvis::Sortable::AccessState::Write, 0U, 5U);

	REQUIRE(sortables.getCounter(sortvis::Sortable::AccessState::Write) == 0);
	REQUIRE(sortables[0] == sortvis::Sortable::AccessState::Write);
	REQUIRE(sortables[1] == sortvis::Sortable::AccessState::Read);
	REQUIRE(sortables[5] == sortvis::Sortable::AccessState::Write);

	sortables.state(sortvis::Sortable::SortState::Full, 0U, 1U);

	REQUIRE(sortables.getCounter(sortvis::Sortable::SortState::Full) == 2);
	REQUIRE(sortables[0] == sortvis::Sortable::SortState::Full);
	REQUIRE(sortables[1] == sortvis::Sortable::SortState::Full);
}
