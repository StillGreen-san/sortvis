#include "algorithms.hpp"
#include "cli.hpp"
#include "sorter.hpp"

constexpr size_t ELEMENTS_TO_SORT = 10;

int main()
{
	sortvis::SorterCollection sorters(
	    ELEMENTS_TO_SORT, {sortvis::algorithms::bubble/*, sortvis::algorithms::quick, sortvis::algorithms::shell,
	            sortvis::algorithms::heap, sortvis::algorithms::insertion, sortvis::algorithms::selection*/});

	while(!sorters.finished())
	{
		sortvis::cli::print(sorters);

		sorters.advance();
	}

	sorters.reset();
}
