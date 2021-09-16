#pragma once

#include "sortable.hpp"

#include <cppcoro/generator.hpp>

#include <exception>
#include <memory>
#include <stack>

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

	for(size_t i = 0; i <= n; ++i)
	{
		data->state(sortvis::Sortable::SortState::Full, i);
	}
}

cppcoro::generator<const int> quick(std::shared_ptr<sortvis::SortableCollection> data)
{
	std::stack<std::pair<size_t, size_t>> stack;

	stack.push(std::make_pair(0, data->size() - 1));

	co_yield INIT_MAGIC_VALUE;

	while(!stack.empty())
	{
		const size_t start = stack.top().first;
		const size_t end = stack.top().second;
		stack.pop();

		size_t pivot = start;

		for(size_t i = start; i < end; i++)
		{
			const bool greater = data->greater(i, end);
			co_yield COMP_MAGIC_VALUE;
			data->state(sortvis::Sortable::AccessState::None, i, end);

			if(!greater)
			{
				data->swap(i, pivot);
				co_yield SWAP_MAGIC_VALUE;
				data->state(sortvis::Sortable::AccessState::None, i, pivot);

				pivot++;
			}
		}

		data->swap(pivot, end);
		co_yield SWAP_MAGIC_VALUE;
		data->state(sortvis::Sortable::AccessState::None, pivot, end);
		data->state(sortvis::Sortable::SortState::Full, pivot);

		bool pushed = false;

		if(pivot > 0 && pivot - 1 > start)
		{
			stack.push(std::make_pair(start, pivot - 1));
			pushed = true;
		}
		else if(start != pivot)
		{
			data->state(sortvis::Sortable::SortState::Full, start);
		}

		if(pivot + 1 < end)
		{
			stack.push(std::make_pair(pivot + 1, end));
			pushed = true;
		}
		else if(end != pivot)
		{
			data->state(sortvis::Sortable::SortState::Full, end);
		}
	}
}

namespace detail
{
size_t iParent(size_t i)
{
	return (i - 1) / 2;
}
size_t iLeftChild(size_t i)
{
	return (2 * i) + 1;
}
size_t iRightChild(size_t i)
{
	return (2 * i) + 2;
}

cppcoro::generator<const int> siftDown(std::shared_ptr<sortvis::SortableCollection> data, size_t start, size_t end)
{
	size_t root = start;

	while(iLeftChild(root) <= end)
	{
		size_t child = iLeftChild(root);
		size_t swap = root;

		bool isLess = data->less(swap, child);
		co_yield COMP_MAGIC_VALUE;
		data->state(sortvis::Sortable::AccessState::None, swap, child);

		if(isLess)
		{
			swap = child;
		}
		if(child + 1 <= end)
		{
			isLess = data->less(swap, child + 1);
			co_yield COMP_MAGIC_VALUE;
			data->state(sortvis::Sortable::AccessState::None, swap, child + 1);

			if(isLess)
			{
				swap = child + 1;
			}
		}
		if(swap == root)
		{
			break;
		}

		data->swap(root, swap);
		co_yield SWAP_MAGIC_VALUE;
		data->state(sortvis::Sortable::AccessState::None, root, swap);

		root = swap;
	}
}

cppcoro::generator<const int> heapify(std::shared_ptr<sortvis::SortableCollection> data)
{
	size_t start = iParent(data->size() - 1);

	while(start != -1)
	{
		cppcoro::generator<const int> generator = siftDown(data, start, data->size() - 1);
		for(const int magicValue : generator)
		{
			co_yield magicValue;
		}
		--start;
	}
}
} // namespace detail

cppcoro::generator<const int> heap(std::shared_ptr<sortvis::SortableCollection> data)
{
	cppcoro::generator<const int> heapGenerator = detail::heapify(data);
	size_t end = data->size() - 1;

	co_yield INIT_MAGIC_VALUE;

	for(const int magicValue : heapGenerator)
	{
		co_yield magicValue;
	}

	while(end > 0)
	{
		data->swap(0, end);
		co_yield SWAP_MAGIC_VALUE;
		data->state(sortvis::Sortable::AccessState::None, size_t(0), end);

		data->state(sortvis::Sortable::SortState::Full, end);
		end = end - 1;

		cppcoro::generator<const int> shiftGenerator = detail::siftDown(data, 0, end);
		for(const int magicValue : shiftGenerator)
		{
			co_yield magicValue;
		}
	}

	data->state(sortvis::Sortable::SortState::Full, size_t(0));
}

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

	for(size_t i = 0; i < len; ++i)
	{
		data->state(sortvis::Sortable::SortState::Full, i);
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

	for(size_t i = 0; i < len; ++i)
	{
		data->state(sortvis::Sortable::SortState::Full, i);
	}
}

cppcoro::generator<const int> selection(std::shared_ptr<sortvis::SortableCollection> data)
{
	const size_t len = data->size();

	co_yield INIT_MAGIC_VALUE;

	for(size_t i = 0; i < len - 1; ++i)
	{
		size_t jMin = i;

		for(size_t j = i + 1; j < len; ++j)
		{
			bool less = data->less(j, jMin);
			co_yield COMP_MAGIC_VALUE;
			data->state(sortvis::Sortable::AccessState::None, j, jMin);

			if(less)
			{
				jMin = j;
			}
		}

		if(jMin != i)
		{
			data->swap(i, jMin);
			co_yield SWAP_MAGIC_VALUE;
			data->state(sortvis::Sortable::AccessState::None, i, jMin);
		}

		data->state(sortvis::Sortable::SortState::Full, i);
	}

	data->state(sortvis::Sortable::SortState::Full, len - 1);
}
} // namespace sortvis::algorithms

namespace sortvis
{
const char* getAlgorithmName(SorterAlgorithm algorithm)
{
	if(algorithm == sortvis::algorithms::bubble)
		return "Bubble Sort";
	if(algorithm == sortvis::algorithms::heap)
		return "Heap Sort";
	if(algorithm == sortvis::algorithms::insertion)
		return "Insertion Sort";
	if(algorithm == sortvis::algorithms::selection)
		return "Selection Sort";
	if(algorithm == sortvis::algorithms::quick)
		return "Quick Sort";
	if(algorithm == sortvis::algorithms::shell)
		return "Shell Sort";
	return "UNKNOWN";
}
} // namespace sortvis
