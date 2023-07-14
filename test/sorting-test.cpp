#include <ostream>

#include "sorter.hpp"

std::ostream& operator<<(std::ostream& os, const sortvis::Sortable& value)
{
	os << value.value;
	return os;
}
std::ostream& operator<<(std::ostream& os, const sortvis::SortableCollection& collection)
{
	os << '[';
	for(const sortvis::Sortable& value : collection)
	{
		os << ' ' << value;
	}
	os << " ]";
	return os;
}

#include <catch2/catch_test_macros.hpp>

const sortvis::SortableCollection SC_ASC{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
const sortvis::SortableCollection SC_DSC{20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
const sortvis::SortableCollection SC_GRP{18, 17, 16, 15, 20, 19, 14, 13, 7, 8, 9, 5, 4, 3, 6, 10, 11, 12, 2, 1};
const sortvis::SortableCollection SC_RND{3, 16, 14, 20, 4, 1, 11, 2, 18, 5, 9, 6, 17, 19, 7, 10, 12, 8, 15, 13};

const sortvis::SortableCollection* SC_ARR[]{&SC_ASC, &SC_DSC, &SC_GRP, &SC_RND};

TEST_CASE("bubble")
{
	for(const sortvis::SortableCollection* sc_ptr : SC_ARR)
	{
		sortvis::Sorter asc(*sc_ptr, sortvis::algorithms::bubble);

		while(asc.advance()) {}

		CHECK(asc.data() == SC_ASC);
	}
}

TEST_CASE("insertion")
{
	for(const sortvis::SortableCollection* sc_ptr : SC_ARR)
	{
		sortvis::Sorter asc(*sc_ptr, sortvis::algorithms::insertion);

		while(asc.advance()) {}

		CHECK(asc.data() == SC_ASC);
	}
}

TEST_CASE("shell")
{
	for(const sortvis::SortableCollection* sc_ptr : SC_ARR)
	{
		sortvis::Sorter asc(*sc_ptr, sortvis::algorithms::shell);

		while(asc.advance()) {}

		CHECK(asc.data() == SC_ASC);
	}
}

TEST_CASE("selection")
{
	for(const sortvis::SortableCollection* sc_ptr : SC_ARR)
	{
		sortvis::Sorter asc(*sc_ptr, sortvis::algorithms::selection);

		while(asc.advance()) {}

		CHECK(asc.data() == SC_ASC);
	}
}

TEST_CASE("quick")
{
	for(const sortvis::SortableCollection* sc_ptr : SC_ARR)
	{
		sortvis::Sorter asc(*sc_ptr, sortvis::algorithms::quick);

		while(asc.advance()) {}

		CHECK(asc.data() == SC_ASC);
	}
}

TEST_CASE("heap")
{
	for(const sortvis::SortableCollection* sc_ptr : SC_ARR)
	{
		sortvis::Sorter asc(*sc_ptr, sortvis::algorithms::heap);

		while(asc.advance()) {}

		CHECK(asc.data() == SC_ASC);
	}
}
