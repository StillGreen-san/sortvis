#include <iostream>

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

	bool operator<(const Sortable& other)
	{
		return value < other.value;
	}

	Sortable& operator=(int value)
	{
		this->value = value;
		return *this;
	}
};

cppcoro::generator<int> sorter(int data)
{
	int i = 0;
	while(true)
	{
		co_yield i + data;
		++i;
	}
}

constexpr int MAX_NUMS = 10;

int main()
{
	int offset = 0;
	std::cin >> offset;
	cppcoro::generator<int> inf = sorter(offset);
	auto it = inf.begin();
	for(int i = 0; i < MAX_NUMS; ++i)
	{
		std::cout << *it << ' ';
		++it;
	}
}
