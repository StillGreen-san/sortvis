#pragma once

#include <cppcoro/generator.hpp>

#include <memory>

constexpr int USELESS_MAGIC_VALUE = 0b01001110100101000101010010110011;

using SorterAlgorithm = cppcoro::generator<const int> (*)(std::shared_ptr<SortableVector>);
