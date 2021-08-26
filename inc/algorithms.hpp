#pragma once

#include "sortable.hpp"

#include <cppcoro/generator.hpp>

#include <exception>
#include <memory>

namespace sortvis
{
/**
 * @brief Function Signature for Algorithms
 *
 */
using SorterAlgorithm = cppcoro::generator<const int> (*)(std::shared_ptr<sortvis::SortableCollection>);

class InitFailureException : public std::exception
{
	using std::exception::exception;
};
} // namespace sortvis

namespace sortvis::algorithms
{
constexpr int INIT_MAGIC_VALUE = 0b10101010101010101010101010100001;
constexpr int COMP_MAGIC_VALUE = 0b10101010101010101010101010100010;
constexpr int SWAP_MAGIC_VALUE = 0b10101010101010101010101010100100;

cppcoro::generator<const int> bubble(std::shared_ptr<sortvis::SortableCollection> data)
{
	bool changed = false;
	size_t n = data->size() - 1;

	co_yield INIT_MAGIC_VALUE;

	do
	{
		changed = false;
		for(size_t i = 0; i < n; ++i)
		{
			bool isLess = data->less(i + 1, i);
			co_yield COMP_MAGIC_VALUE;
			data->state(sortvis::Sortable::AccessState::None, i, i + 1);

			if(isLess)
			{
				data->swap(i + 1, i);
				co_yield SWAP_MAGIC_VALUE;
				data->state(sortvis::Sortable::AccessState::None, i, i + 1);

				changed = true;
			}
		}
		data->state(sortvis::Sortable::SortState::Full, n);
		--n;
	} while(changed);
}

cppcoro::generator<const int> quick(std::shared_ptr<sortvis::SortableCollection> data);

cppcoro::generator<const int> heap(std::shared_ptr<sortvis::SortableCollection> data);

cppcoro::generator<const int> shell(std::shared_ptr<sortvis::SortableCollection> data)
{
	constexpr size_t gaps[] = {701, 301, 132, 57, 23, 10, 4, 1};

	const size_t len = data->size();

	co_yield INIT_MAGIC_VALUE;

	for(size_t gap : gaps)
	{
		for(size_t i = gap; i < len; ++i)
		{
			for(size_t j = i; j >= gap; j -= gap)
			{
				bool greater = data->greater(j - gap, j);
				co_yield COMP_MAGIC_VALUE;
				data->state(sortvis::Sortable::AccessState::None, j - gap, j);

				if(!greater)
					break;

				data->swap(j - gap, j);
				co_yield SWAP_MAGIC_VALUE;
				data->state(sortvis::Sortable::AccessState::None, j - gap, j);
			}
		}
	}
}

cppcoro::generator<const int> insertion(std::shared_ptr<sortvis::SortableCollection> data)
{
	const size_t len = data->size();

	co_yield INIT_MAGIC_VALUE;

	for(size_t i = 1; i < len; ++i)
	{
		for(size_t j = i; j > 0; --j)
		{
			bool greater = data->greater(j - 1, j);
			co_yield COMP_MAGIC_VALUE;
			data->state(sortvis::Sortable::AccessState::None, j - 1, j);

			if(!greater)
				break;

			data->swap(j - 1, j);
			co_yield SWAP_MAGIC_VALUE;
			data->state(sortvis::Sortable::AccessState::None, j - 1, j);
		}
	}
}

cppcoro::generator<const int> selection(std::shared_ptr<sortvis::SortableCollection> data);
} // namespace sortvis::algorithms
