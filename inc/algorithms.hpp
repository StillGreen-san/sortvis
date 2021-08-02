#pragma once

#include "sortable.hpp"

#include <cppcoro/generator.hpp>

#include <exception>
#include <memory>

namespace sortvis
{
using SorterAlgorithm = cppcoro::generator<const int> (*)(std::shared_ptr<sortvis::SortableCollection>);

class InitFailureException : public std::exception
{
};
} // namespace sortvis

namespace sortvis::algorithms
{
constexpr int INIT_MAGIC_VALUE = 0b10101010101010101010101010100001;
constexpr int COMP_MAGIC_VALUE = 0b10101010101010101010101010100010;
constexpr int SWAP_MAGIC_VALUE = 0b10101010101010101010101010100100;

cppcoro::generator<const int> bubble(std::shared_ptr<sortvis::SortableCollection> data)
{
	auto& vec = *data;
	bool changed = false;
	size_t n = vec.size() - 1;

	co_yield INIT_MAGIC_VALUE;

	do
	{
		changed = false;
		for(size_t i = 0; i < n; ++i)
		{
			bool isLess = vec.less(i + 1, i);
			co_yield COMP_MAGIC_VALUE;

			if(isLess)
			{
				vec.swap(i + 1, i);
				co_yield SWAP_MAGIC_VALUE;

				changed = true;
			}
		}
		--n;
	} while(changed);
}

cppcoro::generator<const int> quick(std::shared_ptr<sortvis::SortableCollection> data);

cppcoro::generator<const int> heap(std::shared_ptr<sortvis::SortableCollection> data);

cppcoro::generator<const int> shell(std::shared_ptr<sortvis::SortableCollection> data);

cppcoro::generator<const int> insertion(std::shared_ptr<sortvis::SortableCollection> data);

cppcoro::generator<const int> selection(std::shared_ptr<sortvis::SortableCollection> data);
} // namespace sortvis::algorithms
