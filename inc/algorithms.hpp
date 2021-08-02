#pragma once

#include <cppcoro/generator.hpp>

#include <memory>

namespace sortvis
{
using SorterAlgorithm = cppcoro::generator<const int> (*)(std::shared_ptr<sortvis::SortableCollection>);
} // namespace sortvis

namespace sortvis::algorithms
{
constexpr int USELESS_MAGIC_VALUE = 0b01001110100101000101010010110011;

cppcoro::generator<const int> bubble(std::shared_ptr<sortvis::SortableCollection> data)
{
	auto& vec = *data;
	bool changed = false;
	size_t n = vec.size() - 1;
	do
	{
		changed = false;
		for(size_t i = 0; i < n; ++i)
		{
			bool isLess = vec.less(i + 1, i);
			co_yield USELESS_MAGIC_VALUE;

			if(isLess)
			{
				vec.swap(i + 1, i);
				co_yield USELESS_MAGIC_VALUE;

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
