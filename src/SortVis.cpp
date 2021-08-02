#include "algorithms.hpp"
#include "cli.hpp"
#include "sorter.hpp"

constexpr size_t TEN = 10;

int main()
{
	sortvis::SorterCollection sorters(
	    10, {sortvis::algorithms::bubble/*, sortvis::algorithms::quick, sortvis::algorithms::shell,
	            sortvis::algorithms::heap, sortvis::algorithms::insertion, sortvis::algorithms::selection*/});

	while(!sorters.finished())
	{
		sortvis::cli::print(sorters);

		sorters.advance();
	}

	sorters.reset();
}
