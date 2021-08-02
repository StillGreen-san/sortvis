#pragma once

#include "sortable.hpp"

#include <cppcoro/generator.hpp>

#include <memory>

namespace sortvis
{
struct Sorter
{
	std::shared_ptr<SortableCollection> data;
	cppcoro::generator<const int> gen;
	cppcoro::generator<const int>::iterator it;
	cppcoro::detail::generator_sentinel end;

	Sorter(std::shared_ptr<SortableCollection> datavec, sortvis::SorterAlgorithm generator) :
	    data{std::move(datavec)}, gen{generator(data)}, it{gen.begin()}, end{gen.end()}
	{
	}

	[[nodiscard]] bool finished() const noexcept
	{
		return it == end;
	}

	bool advance()
	{
		++it;
		return !finished();
	}
};

class SorterCollection
{
private:
	std::vector<sortvis::Sorter> sorters;
	sortvis::SortableCollection initialState;
	bool finished = false;

public:
	SorterCollection(size_t elements, std::initializer_list<sortvis::SorterAlgorithm> algorithms) :
	    initialState(elements)
	{
		for(sortvis::SorterAlgorithm algo : algorithms)
		{
			sorters.emplace_back(std::make_shared<sortvis::SortableCollection>(initialState), algo);
			finished |= !sorters.back().finished();
		}
	}

	[[nodiscard]] bool finished() const noexcept
	{
		return finished;
	}

	void advance()
	{
		for(sortvis::Sorter& sorter : sorters)
		{
			finished |= sorter.advance();
		}
	}

	void reset();
};
} // namespace sortvis
