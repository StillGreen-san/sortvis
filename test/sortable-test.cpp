#include "sortable.hpp"

#include <catch2/catch.hpp>

bool operator==(const std::vector<int>& vec, const std::initializer_list<int>& lst)
{
	if(vec.size() != lst.size())
		return false;
	auto vecIt = vec.begin();
	auto lstIt = lst.begin();
	const auto vecEnd = vec.end();
	while(vecIt != vecEnd)
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

	REQUIRE(actual == expected);
}

TEST_CASE("SortableCollection::SortableCollection(6, false)")
{
	const std::initializer_list<int> expected = {1, 2, 3, 4, 5, 6};

	const sortvis::SortableCollection sortables(6, false);

	const std::vector<int> actual(sortables.begin(), sortables.end());

	REQUIRE(actual == expected);
}
