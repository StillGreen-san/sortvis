#pragma once
#include "subwindow.hpp"

class SortWindow : public Subwindow
{
protected:
	enum class SortingState
	{
		Compare,
		Swap,
		Done
	};
	enum class LoopState
	{
		Inner,
		Outer,
		Innerst
	};

private:
	olc::Pixel unsortedColor;
	olc::Pixel sortedColor;
	std::vector<bool> sortedIndices;
	olc::Pixel comparedColor;
	std::vector<int> comparedIndices;
	SortingState sortstate;
	int counter = 0;

protected:
	LoopState loopstate;
	std::vector<int> values;

	void changeState(SortingState state);
	void changeState(LoopState state);

	void addSorted(const int index);

	const std::vector<bool>& getSortedIndices();

	void setCompared(const int indexl, const int indexr);

	virtual void Swap() = 0;
	virtual void Compare() = 0;

public:
	SortWindow(const int width, const int height);

	void Update() final;
};
