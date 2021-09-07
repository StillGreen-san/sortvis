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
		if(hasFinished())
		{
			throw sortvis::InitFailureException("Initialization of SorterAlgorithm Failed! Sorter finished early!");
		}
		if(*it != sortvis::algorithms::INIT_MAGIC_VALUE)
		{
			throw sortvis::InitFailureException(
			    "Initialization of SorterAlgorithm Failed! Sorter returned unexpected Initial-value!");
		}
		if(*colct != datavec)
		{
			throw sortvis::InitFailureException(
			    "Initialization of SorterAlgorithm Failed! Sorter Data does not match InitialState Data!");
		}
	}

	// TODO add custom copy constructors (defaults break with cppcoro::generator)
	Sorter() = delete;
	Sorter(Sorter&&) = default;
	Sorter(const Sorter&) = delete;
	Sorter& operator=(Sorter&&) = default;
	Sorter& operator=(const Sorter&) = delete;
	~Sorter() = default;

	/**
	 * @brief returns wether the SorterAlgoritm has finished sorting the SortableCollection
	 *
	 */
	[[nodiscard]] bool hasFinished() const noexcept
	{
		return it == end;
	}

	/**
	 * @brief advances the SorterAlgorithm by one step if not finished
	 *
	 * @return hasFinished()
	 */
	bool advance()
	{
		if(!hasFinished())
		{
			++it;
			return hasFinished();
		}
		return true;
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

/**
 * @brief A Collections of Sorters supplying them with the same randomized SortableCollection
 *
 */
class SorterCollection
{
private:
	std::vector<sortvis::Sorter> sorters;
	sortvis::SortableCollection initialState;
	bool allFinished = false;

	/**
	 * @brief reset all sorters with initialState
	 *
	 * @return allHaveFinished()
	 */
	bool reset_()
	{
		allFinished = true;
		for(sortvis::Sorter& sorter : sorters)
		{
			allFinished &= sorter.reset(initialState);
		}
		return allHaveFinished();
	}

public:
	/**
	 * @brief Construct a new Sorter Collection object
	 *
	 * @param elements the number of elements to generate for the Sorters
	 * @param algorithms a list of SorterAlgorithms for the Sorters to use
	 */
	SorterCollection(size_t elements, std::initializer_list<sortvis::SorterAlgorithm> algorithms) :
	    initialState(elements)
	{
		initialState.randomize();
		for(sortvis::SorterAlgorithm algo : algorithms)
		{
			sorters.emplace_back(initialState, algo);
		}
	}

	/**
	 * @brief Construct a new Sorter Collection object
	 *
	 * @param elements the initial state to use
	 * @param algorithms a list of SorterAlgorithms for the Sorters to use
	 */
	SorterCollection(
	    const sortvis::SortableCollection& elements, std::initializer_list<sortvis::SorterAlgorithm> algorithms) :
	    initialState(elements)
	{
		for(sortvis::SorterAlgorithm algo : algorithms)
		{
			sorters.emplace_back(initialState, algo);
		}
	}

	/**
	 * @brief returns true if all Sorters have finished sorting
	 */
	[[nodiscard]] bool allHaveFinished() const noexcept
	{
		return allFinished;
	}

	// TODO change return of function from allHaveFinished to !allHaveFinished (and other functions?)

	/**
	 * @brief advance all sorters by one step
	 *
	 * @return allHaveFinished()
	 */
	bool advance()
	{
		allFinished = true;
		for(sortvis::Sorter& sorter : sorters)
		{
			allFinished &= sorter.advance();
		}
		return allHaveFinished();
	}

	/**
	 * @brief resets all Sorters with their initial data
	 *
	 * @return allHaveFinished()
	 */
	bool reset()
	{
		initialState.randomize();
		return reset_();
	}

	/**
	 * @brief resets all Sorters with elements
	 *
	 * @param elements the new initial state
	 * @return allHaveFinished()
	 */
	bool reset(const sortvis::SortableCollection& elements)
	{
		initialState = elements;
		return reset_();
	}
};
} // namespace sortvis
