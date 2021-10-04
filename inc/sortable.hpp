#pragma once

#include <array>
#include <cassert>
#include <numeric>
#include <random>
#include <vector>

namespace sortvis
{
/**
 * @brief Value for SortableCollection with additional State for Visualization
 *
 * @todo rename/remove states to better reflect actual usage
 */
struct Sortable
{
	int value = 0;

	/**
	 * @brief Indicates if this value can be considered sorted
	 *
	 */
	enum class SortState : char
	{
		Full,
		Partial,
		None
	} sortState = SortState::None;

	/**
	 * @brief Indicates Read/Write Access for this value
	 *
	 */
	enum class AccessState : char
	{
		None,
		Read,
		Write
	} accessState = AccessState::None;

	Sortable() noexcept = default;

	explicit Sortable(int val) noexcept : value{val}
	{
	}

	/**
	 * @brief = operator to asign value, does not change states
	 *
	 */
	Sortable& operator=(int other) noexcept
	{
		value = other;
		return *this;
	}

	/**
	 * @brief = operator to asign accessState
	 *
	 */
	Sortable& operator=(AccessState state) noexcept
	{
		accessState = state;
		return *this;
	}

	/**
	 * @brief = operator to asign sortState
	 *
	 */
	Sortable& operator=(SortState state) noexcept
	{
		sortState = state;
		return *this;
	}

	/**
	 * @brief < operator compares only value not state
	 */
	[[nodiscard]] bool operator<(const Sortable& other) const noexcept
	{
		return value < other.value;
	}

	/**
	 * @brief > operator compares only value not state
	 */
	[[nodiscard]] bool operator>(const Sortable& other) const noexcept
	{
		return value > other.value;
	}

	/**
	 * @brief == operator compares only value not state
	 */
	[[nodiscard]] bool operator==(const Sortable& other) const noexcept
	{
		return value == other.value;
	}

	/**
	 * @brief == operator compares SortState
	 */
	[[nodiscard]] bool operator==(const SortState other) const noexcept
	{
		return sortState == other;
	}

	/**
	 * @brief == operator compares AccessState
	 */
	[[nodiscard]] bool operator==(const AccessState other) const noexcept
	{
		return accessState == other;
	}

	/**
	 * @brief != operator compares SortState
	 */
	[[nodiscard]] bool operator!=(const SortState other) const noexcept
	{
		return sortState != other;
	}

	/**
	 * @brief != operator compares AccessState
	 */
	[[nodiscard]] bool operator!=(const AccessState other) const noexcept
	{
		return accessState != other;
	}

	operator int() const
	{
		return value;
	}
};

template<typename T>
concept SortableState =
    std::same_as<T, sortvis::Sortable::AccessState> || std::same_as<T, sortvis::Sortable::SortState>;

/**
 * @brief A Collection of Sortables with additional functionality
 *
 */
class SortableCollection
{
private:
	std::vector<sortvis::Sortable> data;
	std::array<unsigned, 3> accessCounter{0, 0, 0};
	std::array<unsigned, 3> sortCounter{0, 0, 0};

public:
	/**
	 * @brief Construct a new Sortable Collection object
	 *
	 * @param elements number of elements of this collection, numbered [1-elements]
	 */
	explicit SortableCollection(size_t elements, bool doRandomize = false)
	{
		data.resize(elements);
		std::iota(data.begin(), data.end(), 1);
		if(doRandomize)
		{
			randomize();
		}
		sortCounter[static_cast<size_t>(sortvis::Sortable::SortState::None)] = static_cast<unsigned>(data.size());
	}

	/**
	 * @brief Construct a new Sortable Collection object
	 *
	 * @param elements list of elements to use
	 */
	explicit SortableCollection(std::initializer_list<int> elements)
	{
		data.reserve(elements.size());
		for(int i : elements)
		{
			data.emplace_back(i);
		}
		sortCounter[static_cast<size_t>(sortvis::Sortable::SortState::None)] = static_cast<unsigned>(data.size());
	}

	explicit SortableCollection() = delete;

	explicit SortableCollection(SortableCollection&&) = default;
	explicit SortableCollection(const SortableCollection&) = default;
	SortableCollection& operator=(SortableCollection&&) noexcept = default;
	SortableCollection& operator=(const SortableCollection&) = default;
	~SortableCollection() = default;

	/**
	 * @brief Randomize the Collection
	 *
	 */
	void randomize()
	{
		std::default_random_engine rng(std::random_device{}());
		std::shuffle(data.begin(), data.end(), rng);
	}

	/**
	 * @brief Randomize the Collection deterministically,
	 * previous item order does not affect result
	 *
	 * @param seed
	 */
	void randomize(std::default_random_engine::result_type seed)
	{
		std::default_random_engine rng(seed);
		std::sort(data.begin(), data.end());
		std::shuffle(data.begin(), data.end(), rng);
	}

	/**
	 * @brief compares both indicies with operator <,
	 * sets AccessState of Sortables to Read
	 *
	 * @param lhs left index into data
	 * @param rhs right index into data
	 * @return true if data[lhs] < data[rhs]
	 */
	bool less(size_t lhs, size_t rhs) noexcept
	{
		data[lhs] = sortvis::Sortable::AccessState::Read;
		data[rhs] = sortvis::Sortable::AccessState::Read;
		accessCounter[static_cast<size_t>(sortvis::Sortable::AccessState::Read)]++;
		return data[lhs] < data[rhs];
	}

	/**
	 * @brief compares both indicies with operator >,
	 * sets AccessState of Sortables to Read
	 *
	 * @param lhs left index into data
	 * @param rhs right index into data
	 * @return true if data[lhs] > data[rhs]
	 */
	bool greater(size_t lhs, size_t rhs) noexcept
	{
		data[lhs] = sortvis::Sortable::AccessState::Read;
		data[rhs] = sortvis::Sortable::AccessState::Read;
		accessCounter[static_cast<size_t>(sortvis::Sortable::AccessState::Read)]++;
		return data[lhs] > data[rhs];
	}

	/**
	 * @brief swaps Sortables at indices,
	 * sets AccessState of Sortables to Write
	 *
	 * @param lhs left index into data
	 * @param rhs right index into data
	 */
	void swap(size_t lhs, size_t rhs) noexcept
	{
		assert(data[lhs] != sortvis::Sortable::SortState::Full);
		assert(data[rhs] != sortvis::Sortable::SortState::Full);
		data[lhs] = sortvis::Sortable::AccessState::Write;
		data[rhs] = sortvis::Sortable::AccessState::Write;
		accessCounter[static_cast<size_t>(sortvis::Sortable::AccessState::Write)]++;
		std::swap(data[lhs], data[rhs]);
	}

	[[nodiscard]] unsigned getCounter(sortvis::Sortable::AccessState state) const noexcept
	{
		return accessCounter[static_cast<size_t>(state)];
	}

	[[nodiscard]] unsigned getCounter(sortvis::Sortable::SortState state) const noexcept
	{
		return sortCounter[static_cast<size_t>(state)];
	}

	/**
	 * @brief Sets the AccessState of all Sortables to state,
	 * if SortState == Full sort counters will be updated
	 *
	 * @tparam STATE sortvis::SortableState
	 * @tparam IDX... std::unsigned_integral
	 * @param state state to set
	 * @param idx... indices to set
	 *
	 * @todo have IDX be size_t to reduce instanciations / converison errors?
	 */
	template<sortvis::SortableState STATE, std::unsigned_integral... IDX>
	void state(STATE state, IDX... idx) noexcept
	{
		((data[idx] = state), ...);
		if constexpr(std::is_same_v<STATE, sortvis::Sortable::SortState>)
		{
			if(state == sortvis::Sortable::SortState::Full)
			{
				sortCounter[static_cast<size_t>(sortvis::Sortable::SortState::None)] -= sizeof...(IDX);
				sortCounter[static_cast<size_t>(sortvis::Sortable::SortState::Full)] += sizeof...(IDX);
			}
		}
	}

	/**
	 * @return size_t Size of Collection
	 */
	[[nodiscard]] size_t size() const noexcept
	{
		return data.size();
	}

	/**
	 * @return std::vector<sortvis::Sortable>::const_iterator to begin of Collection
	 */
	[[nodiscard]] std::vector<sortvis::Sortable>::const_iterator begin() const noexcept
	{
		return data.cbegin();
	}

	/**
	 * @return std::vector<sortvis::Sortable>::const_iterator to end of Collection
	 */
	[[nodiscard]] std::vector<sortvis::Sortable>::const_iterator end() const noexcept
	{
		return data.cend();
	}

	/**
	 * @brief resets the Collection with dat and resets counters
	 *
	 * @param dat Collection to copy data from
	 */
	void reset(const sortvis::SortableCollection& dat)
	{
		data = dat.data;
		accessCounter = {0, 0, 0};
		sortCounter = {0, 0, 0};
		sortCounter[static_cast<size_t>(sortvis::Sortable::SortState::None)] = static_cast<unsigned>(data.size());
	}

	/**
	 * @brief compares only values not states or counters
	 *
	 * @param other Collection to compare to
	 * @return true if Collection != other Collection
	 */
	[[nodiscard]] bool operator!=(const sortvis::SortableCollection& other) const
	{
		return data != other.data;
	}

	/**
	 * @brief compares only values not states or counters
	 *
	 * @param other Collection to compare to
	 * @return true if Collection == other Collection
	 */
	[[nodiscard]] bool operator==(const sortvis::SortableCollection& other) const
	{
		return data == other.data;
	}

	/**
	 * @brief unchecked element access
	 *
	 * @param idx index to access
	 * @return const sortvis::Sortable& element at idx
	 */
	[[nodiscard]] const sortvis::Sortable& operator[](size_t idx) const noexcept
	{
		return data[idx];
	}
};
} // namespace sortvis
