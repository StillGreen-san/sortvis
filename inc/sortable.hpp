#pragma once

#include <numeric>
#include <random>
#include <vector>

namespace sortvis
{
struct Sortable
{
	int value = 0;

	enum class SortState : char
	{
		Full,
		Partial,
		None
	} sortState = SortState::None;

	enum class AccessState : char
	{
		None,
		Read,
		Write
	} accessState = AccessState::None;

	bool operator<(const Sortable& other) const noexcept
	{
		return value < other.value;
	}

	Sortable& operator=(int other) noexcept
	{
		value = other;
		return *this;
	}
};

class SortableCollection
{
private:
	std::vector<sortvis::Sortable> data;

public:
	explicit SortableCollection(size_t elements)
	{
		data.resize(elements);
		std::iota(data.begin(), data.end(), 1);
	}

	explicit SortableCollection(const sortvis::SortableCollection&) = default;

	void randomize()
	{
		randomize(std::random_device{}());
	}

	void randomize(std::default_random_engine::result_type seed)
	{
		std::default_random_engine rng(seed);
		std::shuffle(data.begin(), data.end(), rng);
	}

	bool less(size_t lhs, size_t rhs) noexcept
	{
		data[lhs].accessState = sortvis::Sortable::AccessState::Read;
		data[rhs].accessState = sortvis::Sortable::AccessState::Read;
		return data[lhs] < data[rhs];
	}

	void swap(size_t lhs, size_t rhs) noexcept
	{
		data[lhs].accessState = sortvis::Sortable::AccessState::Write;
		data[rhs].accessState = sortvis::Sortable::AccessState::Write;
		std::swap(data[lhs], data[rhs]);
	}

	[[nodiscard]] size_t size() const noexcept
	{
		return data.size();
	}

	[[nodiscard]] std::vector<sortvis::Sortable>::const_iterator begin() const noexcept
	{
		return data.cbegin();
	}

	[[nodiscard]] std::vector<sortvis::Sortable>::const_iterator end() const noexcept
	{
		return data.cend();
	}

	void reset(const sortvis::SortableCollection& dat)
	{
		data = dat.data;
	}

	bool operator!=(const sortvis::SortableCollection& dat) const
	{
		return data != dat.data;
	}
};
} // namespace sortvis
