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

/**
 * @brief Thrown if an initialization exception occurred
 *
 */
class InitFailureException : public std::exception
{
	using std::exception::exception;
};
} // namespace sortvis

/**
 * @brief contains all (coroutine) sorting algorithms
 *
 */
namespace sortvis::algorithms
{
constexpr int INIT_MAGIC_VALUE = 0b10101010101010101010101010100001;
constexpr int COMP_MAGIC_VALUE = 0b10101010101010101010101010100010;
constexpr int SWAP_MAGIC_VALUE = 0b10101010101010101010101010100100;

cppcoro::generator<const int> bubble(std::shared_ptr<sortvis::SortableCollection> data);

cppcoro::generator<const int> quick(std::shared_ptr<sortvis::SortableCollection> data);

cppcoro::generator<const int> heap(std::shared_ptr<sortvis::SortableCollection> data);

cppcoro::generator<const int> shell(std::shared_ptr<sortvis::SortableCollection> data);

cppcoro::generator<const int> insertion(std::shared_ptr<sortvis::SortableCollection> data);

cppcoro::generator<const int> selection(std::shared_ptr<sortvis::SortableCollection> data);
} // namespace sortvis::algorithms

namespace sortvis
{
/**
 * @param algorithm function pointer to algo
 * @return const char* to name of algo
 */
const char* getAlgorithmName(SorterAlgorithm algorithm);
} // namespace sortvis
