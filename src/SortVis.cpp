#include "algorithms.hpp"
#include "cli.hpp"
#include "sorter.hpp"

#include <iostream>

constexpr size_t ELEMENTS_TO_SORT = 10;

int main()
{
	try
	{
		sortvis::SorterCollection sorters(
		    ELEMENTS_TO_SORT, {sortvis::algorithms::bubble, sortvis::algorithms::insertion});

		while(!sorters.allHaveFinished())
		{
			sortvis::cli::print(sorters);

			sorters.advance();
		}

		sorters.reset();
	}
	catch(sortvis::InitFailureException& e)
	{
		std::cerr << e.what() << std::endl;
	}
	catch(...)
	{
		std::cerr << "UNKOWN EXCEPTION!" << std::endl;
	}
}
