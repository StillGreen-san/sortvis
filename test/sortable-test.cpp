#include "sortable.hpp"

#include <catch2/catch.hpp>

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

	const std::vector<int> actual(sortables.begin(), sortables.end());

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
