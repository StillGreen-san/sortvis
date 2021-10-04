#pragma once

#include "sorter.hpp"

#include <array>
#include <charconv>

namespace sortvis
{
/**
 * @brief represents a string with a number at the end,
 * with fixed max size of 14
 *
 */
class NumberedString
{
private:
	std::array<char, 15> string{'\0'};
	unsigned char staticSize;

public:
	/**
	 * @brief Construct a new Numbered String object
	 *
	 * @param str string befor number
	 */
	NumberedString(const char* str)
	{
		staticSize = static_cast<unsigned char>(std::strlen(str));
		strncpy_s(string.data(), string.size(), str, staticSize);
	}

	/**
	 * @brief updates the number after the string
	 *
	 * @param num the number
	 */
	void update(unsigned num) noexcept
	{
		auto result = std::to_chars(string.data() + staticSize, string.data() + (string.size() - 1), num);
		*result.ptr = '\0';
	}

	/**
	 * @return const char* to begin of string
	 */
	const char* data() const noexcept
	{
		return string.data();
	}
};

/**
 * @brief helper struct for bar labels
 *
 */
struct BarLabels
{
	std::array<NumberedString, 4> data{"swap ", "compare ", "sorted ", "none "};
	sortvis::NumberedString& write()
	{
		return data[0];
	}
	sortvis::NumberedString& read()
	{
		return data[1];
	}
	sortvis::NumberedString& full()
	{
		return data[2];
	}
	sortvis::NumberedString& none()
	{
		return data[3];
	}

	/**
	 * @brief update NumberedStrings with counters of sorter
	 *
	 * @param sorter to use counters from
	 */
	void update(const sortvis::Sorter& sorter)
	{
		write().update(sorter.data().getCounter(sortvis::Sortable::AccessState::Write));
		read().update(sorter.data().getCounter(sortvis::Sortable::AccessState::Read));
		full().update(sorter.data().getCounter(sortvis::Sortable::SortState::Full));
		none().update(sorter.data().getCounter(sortvis::Sortable::SortState::None));
	}
};
} // namespace sortvis
