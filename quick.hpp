#pragma once
#include "sortwindow.hpp"

class QuickSort : public SortWindow
{
private:
	int pastendindex;
	int innercounter;
	int pivotindex;
	int storeindex;

	/** returns start & end of first valid partion
	 * if no partition is found return is -1, 0
	 * 
	 * @brief returns start & pastend of first valid partion
	 * 
	 * @return pair<int start, int end> 
	 */
	std::pair<int, int> findValidPartition();

public:
	QuickSort(const int width, const int height);

protected:
	void Swap() override;

	void Compare() override;

	/*
	for each (unsorted) partition
		set first element as pivot
		storeIndex = pivotIndex + 1
		for i = pivotIndex + 1 to rightmostIndex
			if element[i] < element[pivot]
				swap(i, storeIndex)
				storeIndex++
		swap(pivot, storeIndex - 1)
	*/
};