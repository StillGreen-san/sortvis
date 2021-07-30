#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <vector>

#include <cppcoro/generator.hpp>

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

struct SortableVector
{
	std::vector<Sortable> data;

	explicit SortableVector(std::vector<Sortable> data) : data{std::move(data)}
	{
	}

	bool less(size_t lhs, size_t rhs) noexcept
	{
		data[lhs].accessState = Sortable::AccessState::Read;
		data[rhs].accessState = Sortable::AccessState::Read;
		return data[lhs] < data[rhs];
	}

	void swap(size_t lhs, size_t rhs) noexcept
	{
		data[lhs].accessState = Sortable::AccessState::Write;
		data[rhs].accessState = Sortable::AccessState::Write;
		std::swap(data[lhs], data[rhs]);
	}

	[[nodiscard]] size_t size() const noexcept
	{
		return data.size();
	}

	std::vector<Sortable>::iterator begin() noexcept
	{
		return data.begin();
	}

	std::vector<Sortable>::iterator end() noexcept
	{
		return data.end();
	}
};

constexpr int USELESS_MAGIC_VALUE = 0b01001110100101000101010010110011;

cppcoro::generator<const int> bubble(std::shared_ptr<SortableVector> data)
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

std::ostream& operator<<(std::ostream& os, const std::vector<Sortable>& dat)
{
	os << '[';
	for(const Sortable& elm : dat)
	{
		os << ' ' << elm.value;
	}
	os << ' ' << ']';
	return os;
}

struct GeneratorStorage
{
	std::shared_ptr<SortableVector> data;
	cppcoro::generator<const int> gen;
	cppcoro::generator<const int>::iterator it;
	cppcoro::detail::generator_sentinel end;

	GeneratorStorage(std::shared_ptr<SortableVector> datavec,
	    cppcoro::generator<const int> (*generator)(std::shared_ptr<SortableVector>)) :
	    data{std::move(datavec)},
	    gen{generator(data)}, it{gen.begin()}, end{gen.end()}
	{
	}

	[[nodiscard]] bool generationLeft() const noexcept
	{
		return it != end;
	}
};

constexpr size_t TEN = 10;

int main()
{
	std::vector<Sortable> data(TEN);
	std::iota(data.begin(), data.end(), 0);
	std::mt19937_64 rng(std::random_device{}());
	std::shuffle(data.begin(), data.end(), rng);

	std::vector<GeneratorStorage> generators;
	generators.emplace_back(std::make_shared<SortableVector>(data), bubble);

	bool generationleft = true;
	while(generationleft)
	{
		generationleft = false;
		for(GeneratorStorage& storage : generators)
		{
			if(storage.generationLeft())
			{
				std::cout << storage.data->data << std::endl;
				++storage.it;
				generationleft = true;
			}
		}
	}
}
