#pragma once

#include "algorithms.hpp"
#include "sortable.hpp"

#include <cppcoro/generator.hpp>

#include <memory>

namespace sortvis
{
/**
 * @brief A SorterAlgorithm that sorts a SortableCollection
 *
 */
class Sorter
{
private:
	std::shared_ptr<sortvis::SortableCollection> colct;
	cppcoro::generator<const int> gen;
	cppcoro::generator<const int>::iterator it;
	cppcoro::detail::generator_sentinel end;

public:
	/**
	 * @brief Construct a new Sorter object
	 *
	 * @param datavec SortableCollection to sort
	 * @param generator SorterAlgorithm to use to sort
	 */
	Sorter(const sortvis::SortableCollection& datavec, sortvis::SorterAlgorithm generator) :
	    colct{std::make_shared<sortvis::SortableCollection>(datavec)}, gen{generator(colct)}, it{gen.begin()},
	    end{gen.end()}
	{
	}

	/**
	 * @brief returns wether the SorterAlgoritm has finished sorting the SortableCollection
	 *
	 */
	[[nodiscard]] bool hasFinished() const noexcept
	{
		return it == end;
	}

	/**
	 * @brief advances the SorterAlgorithm by one step
	 *
	 * @return hasFinished()
	 */
	bool advance()
	{
		++it;
		return hasFinished();
	}

	/**
	 * @brief resets the SortableCollection to the dat argument
	 * and restarts the SorterAlgorithm
	 *
	 * @param dat SortableCollection to reset to
	 * @return hasFinished()
	 */
	bool reset(const sortvis::SortableCollection& dat)
	{
		colct->reset(dat);
		it = gen.begin();
		end = gen.end();
		return hasFinished();
	}

	/**
	 * @brief returns const& to the SortableCollection
	 */
	const sortvis::SortableCollection& data() const noexcept
	{
		return *colct;
	}
};

class SorterCollection
{
private:
	std::vector<sortvis::Sorter> sorters;
	sortvis::SortableCollection initialState;
	bool allFinished = true;

public:
	SorterCollection(size_t elements, std::initializer_list<sortvis::SorterAlgorithm> algorithms) :
	    initialState(elements)
	{
		initialState.randomize();
		for(sortvis::SorterAlgorithm algo : algorithms)
		{
			sorters.emplace_back(initialState, algo);
			allFinished &= sorters.back().hasFinished();
			if(sorters.back().data() != initialState)
			{
				throw sortvis::InitFailureException();
			}
		}
	}

	[[nodiscard]] bool allHaveFinished() const noexcept
	{
		return allFinished;
	}

	bool advance()
	{
		allFinished = true;
		for(sortvis::Sorter& sorter : sorters)
		{
			allFinished &= sorter.advance();
		}
		return allHaveFinished();
	}

	bool reset()
	{
		allFinished = true;
		initialState.randomize();
		for(sortvis::Sorter& sorter : sorters)
		{
			allFinished &= sorter.reset(initialState);
		}
		return allHaveFinished();
	}
};
} // namespace sortvis
