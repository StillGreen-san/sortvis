#pragma once
#include "sortwindow.hpp"

class SelectionSort : public SortWindow
{
private:
	int outercounter;
	int innercounter;
	int firstunsortedindex;
	int minimumindex;

public:
	SelectionSort(const int width, const int height);

protected:
	void Swap() override;

	void Compare() override;

	/*
	repeat (numOfElements - 1) times
		set the first unsorted element as the minimum
		for each of the unsorted elements
			if element < currentMinimum
				set element as new minimum
		swap minimum with first unsorted position
	*/
};