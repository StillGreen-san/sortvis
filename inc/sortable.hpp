#pragma once

#include <numeric>
#include <random>
#include <vector>

namespace sortvis
{
/**
 * @brief Value for SortableCollection with additional State for Visualization
 *
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

	explicit Sortable(int val) : value{val}
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

public:
	/**
	 * @brief Construct a new Sortable Collection object
	 *
	 * @param elements number of elements of this collection, numbered [1-elements]
	 */
	explicit SortableCollection(size_t elements)
	{
		data.resize(elements);
		std::iota(data.begin(), data.end(), 1);
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
	}

	constexpr explicit SortableCollection() noexcept = delete;
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
		randomize(std::random_device{}());
	}

	/**
	 * @brief Randomize the Collection
	 *
	 * @param seed to initialize random engine with
	 */
	void randomize(std::default_random_engine::result_type seed)
	{
		std::default_random_engine rng(seed);
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
		data[lhs].accessState = sortvis::Sortable::AccessState::Read;
		data[rhs].accessState = sortvis::Sortable::AccessState::Read;
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
		data[lhs].accessState = sortvis::Sortable::AccessState::Read;
		data[rhs].accessState = sortvis::Sortable::AccessState::Read;
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
		data[lhs].accessState = sortvis::Sortable::AccessState::Write;
		data[rhs].accessState = sortvis::Sortable::AccessState::Write;
		std::swap(data[lhs], data[rhs]);
	}

	/**
	 * @brief gets Value at idx,
	 * sets AccessState of Sortable to Read
	 *
	 * @param idx
	 * @return int
	 */
	[[nodiscard]] int get(size_t idx) noexcept
	{
		data[idx].accessState = sortvis::Sortable::AccessState::Read;
		return data[idx].value;
	}

	/**
	 * @brief Sets the AccessState of all Sortables to state
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
	 * @brief resets the Collection with dat
	 *
	 * @param dat Collection to copy data from
	 */
	void reset(const sortvis::SortableCollection& dat)
	{
		data = dat.data;
	}

	/**
	 * @param other Collection to compare to
	 * @return true if Collection != other Collection
	 */
	[[nodiscard]] bool operator!=(const sortvis::SortableCollection& other) const
	{
		return data != other.data;
	}

	/**
	 * @param other Collection to compare to
	 * @return true if Collection == other Collection
	 */
	[[nodiscard]] bool operator==(const sortvis::SortableCollection& other) const
	{
		return data == other.data;
	}
};
} // namespace sortvis
