#pragma once
#include "sortwindow.hpp"

class InsertionSort : public SortWindow
{
private:
	int outercounter;
	int innercounter;
	int lastsortedindex;
	int elementxindex;

public:
	InsertionSort(const int width, const int height);

protected:
	void Swap() override;

	void Compare() override;

	/*
	mark first element as sorted
	for each unsorted element X
		'extract' the element X
		for j = lastSortedIndex down to 0
			if current element j > X
				move sorted element to the right by 1
			else
				break loop and insert X here
	*/
};