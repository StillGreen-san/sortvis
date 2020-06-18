#pragma once
#include "sortwindow.hpp"

class BubbleSort : public SortWindow
{
private:
	bool swapped;
	int innercounter;
	int lastunsortedindex;

public:
	BubbleSort(const int width, const int height);

protected:
	void Swap() override;

	void Compare() override;

	/*
	do
	swapped = false
	for i = 0 to indexOfLastUnsortedElement-1
		if leftElement > rightElement
		swap(leftElement, rightElement)
		swapped = true
	while swapped
	*/
};