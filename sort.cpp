#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <numeric>
#include <random>

using namespace std;
using namespace olc;

Pixel RandomPixel(const int mod = 255)
{
	return{uint8_t(rand()%mod),uint8_t(rand()%mod),uint8_t(rand()%mod)};
}

int MapInt(const int mina, const int maxa,const int minb, const int maxb, const int vala)
{
	int a = (vala - mina)*(maxb - minb);
	int b = maxa - mina;
	return minb + (a / b);
}

class Subwindow
{
private:
	Sprite sprite;

protected:
	bool SetPixel(int x, int y, const Pixel& pixel)
	{
		return sprite.SetPixel(x, y, pixel);
	}

	bool SetLine(int x1, int y1, int x2, int y2, const Pixel& pixel)
	{
		bool hasdrawn = false;
		int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = x2 - x1; dy = y2 - y1;

		if (dx == 0) // Line is vertical
		{
			if (y2 < y1) swap(y1, y2);
			for (y = y1; y <= y2; y++) hasdrawn |= SetPixel(x1, y, pixel);
			return hasdrawn;
		}

		if (dy == 0) // Line is horizontal
		{
			if (x2 < x1) swap(x1, x2);
			for (x = x1; x <= x2; x++) hasdrawn |= SetPixel(x, y1, pixel);
			return hasdrawn;
		}

		// Line is Funk-aye
		dx1 = abs(dx); dy1 = abs(dy);
		px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{ x = x1; y = y1; xe = x2; }
			else
			{ x = x2; y = y2; xe = x1; }

			hasdrawn |= SetPixel(x, y, pixel);

			for (i = 0; x<xe; i++)
			{
				x = x + 1;
				if (px<0)
					px = px + 2 * dy1;
				else
				{
					if ((dx<0 && dy<0) || (dx>0 && dy>0)) y = y + 1; else y = y - 1;
					px = px + 2 * (dy1 - dx1);
				}
				hasdrawn |= SetPixel(x, y, pixel);
			}
		}
		else
		{
			if (dy >= 0)
			{ x = x1; y = y1; ye = y2; }
			else
			{ x = x2; y = y2; ye = y1; }

			hasdrawn |= SetPixel(x, y, pixel);

			for (i = 0; y<ye; i++)
			{
				y = y + 1;
				if (py <= 0)
					py = py + 2 * dx1;
				else
				{
					if ((dx<0 && dy<0) || (dx>0 && dy>0)) x = x + 1; else x = x - 1;
					py = py + 2 * (dx1 - dy1);
				}
				hasdrawn |= SetPixel(x, y, pixel);
			}
		}
		return hasdrawn;
	}

	Subwindow(const int width, const int height)
		: sprite(width, height)
	{}

public:
	virtual void Update() = 0;

	int GetWidth() { return sprite.width; }
	int GetHeight() { return sprite.height; }

	Sprite* GetSprite() { return &sprite; }
};

class DummyWindow : public Subwindow
{
private:
	Pixel color;

public:
	DummyWindow(const int width, const int height, const Pixel color)
		: color{color}, Subwindow(width, height)
	{}

	void Update() override
	{
		SetPixel(rand()%GetWidth(),rand()%GetHeight(),BLACK);
		SetPixel(rand()%GetWidth(),rand()%GetHeight(),BLACK);
		SetPixel(rand()%GetWidth(),rand()%GetHeight(),BLACK);
		SetPixel(rand()%GetWidth(),rand()%GetHeight(),color);
	}
};

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
		Outer
	};

private:
	Pixel unsortedColor;
	Pixel sortedColor;
	vector<bool> sortedIndices;
	Pixel comparedColor;
	vector<int> comparedIndices;
	SortingState sortstate;
	int counter = 0;

protected:
	LoopState loopstate;
	vector<int> values;

	void changeState(SortingState state)
	{
		sortstate = state;
	}
	void changeState(LoopState state)
	{
		loopstate = state;
	}

	void addSorted(const int index)
	{
		sortedIndices[index] = true;
	}

	const vector<bool>& getSortedIndices()
	{
		return sortedIndices;
	}

	void setCompared(const int indexl, const int indexr)
	{
		comparedIndices[0] = indexl;
		comparedIndices[1] = indexr;
	}

	virtual void Swap() = 0;
	virtual void Compare() = 0;

public:
	SortWindow(const int width, const int height)
		: Subwindow(width, height), values(width), sortedIndices(width),
		sortstate(SortingState::Compare), comparedIndices(2), loopstate(LoopState::Outer),
		unsortedColor(BLUE), sortedColor(GREEN), comparedColor(YELLOW)
	{
		iota(begin(values), end(values), 1);
		shuffle(begin(values), end(values), mt19937{random_device{}()});
		fill(begin(sortedIndices), end(sortedIndices), false);
		fill(begin(comparedIndices), end(comparedIndices), 0);
	}

	void Update() final
	{
		if(++counter < 6) return;
		counter = 0;

		switch (sortstate)
		{
		case SortingState::Compare:
			Compare();
			break;
		case SortingState::Swap:
			Swap();
			break;
		}

		int canvaswidth = GetWidth()-1;
		int canvasheight = GetHeight()-1;		
		for(int x = 0; x < values.size(); x++)
		{
			int lineheight = MapInt(0,canvaswidth,0,canvasheight,values[x]);
			SetLine(x, 0, x, canvasheight-lineheight-1, BLACK);
			if(sortedIndices[x])
				SetLine(x, canvasheight-lineheight, x, canvasheight, sortedColor);
			else
				SetLine(x, canvasheight-lineheight, x, canvasheight, unsortedColor);
		}
		if(sortstate == SortingState::Done) return;
		for(int index : comparedIndices)
		{
			int lineheight = MapInt(0,canvaswidth,0,canvasheight,values[index]);
			SetLine(index, canvasheight-lineheight, index, canvasheight, comparedColor);
		}
	}
};

class BubbleSort : public SortWindow
{
private:
	bool swapped;
	int innercounter;
	int lastunsortedindex;

public:
	BubbleSort(const int width, const int height)
		: SortWindow(width, height)
	{
		lastunsortedindex = values.size()-1;
	}

protected:
	void Swap() override
	{
		swap(values[innercounter], values[innercounter+1]);
		swapped = true;
		innercounter++;
		changeState(SortingState::Compare);
	}

	void Compare() override
	{
		if(loopstate == LoopState::Outer)
		{
			swapped = false;
			innercounter = 0;
			loopstate = LoopState::Inner;
		}

		setCompared(innercounter, innercounter+1);
		if(innercounter < lastunsortedindex)
		{
			if(values[innercounter] > values[innercounter+1])
			{
				changeState(SortingState::Swap);
			}
			else innercounter++;
		}
		else
		{
			loopstate = LoopState::Outer;
			addSorted(lastunsortedindex--);
			if(!swapped)
			{
				changeState(SortingState::Done);
				for(int i = 0; i <= lastunsortedindex; i++)
					addSorted(i);
			}
		}
	}

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

class SelectionSort : public SortWindow
{
private:
	int outercounter;
	int innercounter;
	int firstunsortedindex;
	int minimumindex;

public:
	SelectionSort(const int width, const int height)
		: SortWindow(width, height)
	{
		firstunsortedindex = 0;
		outercounter = 0;
	}

protected:
	void Swap() override
	{
		swap(values[minimumindex], values[firstunsortedindex]);
		addSorted(firstunsortedindex++);
		outercounter++;
		changeState(SortingState::Compare);
	}

	void Compare() override
	{
		if(outercounter == values.size())
		{
			changeState(SortingState::Done);
			return;
		}
		
		if(loopstate == LoopState::Outer)
		{
			minimumindex = firstunsortedindex;
			loopstate = LoopState::Inner;
			innercounter = firstunsortedindex;
		}

		if(innercounter < values.size())
		{
			if(values[innercounter] < values[minimumindex])
				minimumindex = innercounter;
			setCompared(innercounter, minimumindex);
			innercounter++;
		}
		else
		{
			changeState(LoopState::Outer);
			changeState(SortingState::Swap);
		}
	}

	/*
	repeat (numOfElements - 1) times
		set the first unsorted element as the minimum
		for each of the unsorted elements
			if element < currentMinimum
				set element as new minimum
		swap minimum with first unsorted position
	*/
};

class InsertionSort : public SortWindow
{
private:
	int outercounter;
	int innercounter;
	int lastsortedindex;
	int elementxindex;

public:
	InsertionSort(const int width, const int height)
		: SortWindow(width, height)
	{
		outercounter = 0;
		lastsortedindex = 0;
	}

protected:
	//TODO: check if actually ionsertion sort (insertion overhead)
	void Swap() override
	{
		if(loopstate == LoopState::Inner)
		{
			swap(values[innercounter], values[elementxindex]);
			elementxindex = innercounter--;
		}
		changeState(SortingState::Compare);
		if(innercounter == -1)
			changeState(LoopState::Outer);
	}

	void Compare() override
	{
		if(loopstate == LoopState::Outer)
		{
			addSorted(outercounter);
			lastsortedindex = outercounter++;
			
			if(!(outercounter < values.size()))
			{
				changeState(SortingState::Done);
				return;
			}
			elementxindex = outercounter;
			innercounter = lastsortedindex;
			changeState(LoopState::Inner);
		}

		if(innercounter >= 0)
		{
			setCompared(innercounter, elementxindex);
			if(values[innercounter] > values[elementxindex])
			{
				changeState(SortingState::Swap);
			}
			else
			{
				changeState(SortingState::Swap);
				changeState(LoopState::Outer);
			}
		}
	}

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
	pair<int, int> findValidPartition()
	{
		pair<int, int> partition{-1,-1};
		const vector<bool>& sortedindices = getSortedIndices();
		
		for(int index=0; index < sortedindices.size(); index++)
		{
			if(partition.first == -1)
			{
				if(!sortedindices[index])
					partition.first = index;
			}
			else
			{
				if(sortedindices[index])
				{
					partition.second = index;
					return partition;
				}
			}
		}

		if(partition.first > partition.second)
			partition.second = sortedindices.size();
		else
			partition.second = 0;

		return partition;
	}

public:
	QuickSort(const int width, const int height)
		: SortWindow(width, height)
	{
	}

protected:
	void Swap() override
	{
		if(loopstate == LoopState::Inner)
		{
			swap(values[innercounter-1], values[storeindex]);
			setCompared(innercounter-1, storeindex);
			storeindex++;
		}
		else
		{
			swap(values[pivotindex], values[--storeindex]);
			setCompared(pivotindex, storeindex);
			addSorted(storeindex);
		}
		changeState(SortingState::Compare);
	}

	void Compare() override
	{
		if(loopstate == LoopState::Outer)
		{
			pair<int, int> partition = findValidPartition();
			if(partition.first == partition.second-1)
			{
				if(partition.first == -1)
				{
					changeState(SortingState::Done);
					return;
				}
				addSorted(partition.first);
				setCompared(partition.first, partition.first);
				return;
			}
			pivotindex = partition.first;
			storeindex = pivotindex + 1;
			innercounter = storeindex;
			pastendindex = partition.second;
			changeState(LoopState::Inner);
		}

		if(innercounter < pastendindex)
		{
			if(values[innercounter] < values[pivotindex])
				changeState(SortingState::Swap);
			innercounter++;
			setCompared(innercounter, pivotindex);
		}
		else
		{
			changeState(SortingState::Swap);
			changeState(LoopState::Outer);
		}
	}

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

struct SubwindowStorage
{
	int xoffset;
	int yoffset;
	int scale;
	unique_ptr<Subwindow> subwindow;
};

class OLC_SORT : public olc::PixelGameEngine
{
private:
	vector<SubwindowStorage> subwindows;

public:
	OLC_SORT()
	{
		sAppName = "Sort Stuff";
	}

private:
	void DrawSubwindows()
	{
		for (auto& storage : subwindows)
		{
			storage.subwindow->Update();
			DrawSprite(	storage.xoffset,
						storage.yoffset,
						storage.subwindow->GetSprite(),
						storage.scale);
		}
	}

	void CreateSubwindows(const int rows, const int collumns, const int scale)
	{
		int rowoffset = ScreenHeight()/rows;
		int collumnoffset = ScreenWidth()/collumns;
		int width = (collumnoffset-3)/scale;
		int w = (fmodf((float)collumnoffset/(float)scale,1.f)*scale)/2;
		int height = (rowoffset-8-2)/scale;
		int h = (fmodf((float)rowoffset/(float)scale,1.f)*scale)/2;
		
		subwindows.reserve(rows*collumns);

		for (int yoffset = 8+h; yoffset < rowoffset*rows; yoffset+=rowoffset)
		{
			for (int xoffset = 0+w; xoffset < collumnoffset*collumns; xoffset+=collumnoffset)
			{
				Pixel pixel = RandomPixel();
				subwindows.push_back({
					xoffset+2,
					yoffset+1,
					scale,
					unique_ptr<Subwindow>(
						new DummyWindow{width,height, pixel})
					});

				DrawString(xoffset, yoffset-8," X:"+to_string(xoffset)+" Y:"+to_string(yoffset),WHITE);
				DrawRect(xoffset+1, yoffset, (width*scale)+1, (height*scale)+1, pixel);
			}
		}
	}

	void SetText(const int subwindowindex, const string& text)
	{
		if(subwindowindex < 0 || subwindowindex >= subwindows.size()) return;

		FillRect(subwindows[subwindowindex].xoffset-1,
				subwindows[subwindowindex].yoffset-9,
				(subwindows[subwindowindex].subwindow->GetWidth()*subwindows[subwindowindex].scale)+1,
				8, BLACK);

		DrawString(subwindows[subwindowindex].xoffset,
					subwindows[subwindowindex].yoffset-9,
					text, WHITE);
	}

	template <class CLASS>
	void SetSubWindow(const int subwindowindex, const string& text)
	{
		if(subwindowindex < 0 || subwindowindex >= subwindows.size()) return;

		SetText(subwindowindex, text);
		swap(subwindows[subwindowindex].subwindow, unique_ptr<Subwindow>(
			new CLASS{subwindows[subwindowindex].subwindow->GetWidth(),
					subwindows[subwindowindex].subwindow->GetHeight()}));
	}

protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		CreateSubwindows(2, 3, 4);
		SetSubWindow<BubbleSort>(4, "Bubble Sort");
		SetSubWindow<SelectionSort>(3, "Selection Sort");
		SetSubWindow<InsertionSort>(1, "Insertion Sort");
		SetSubWindow<QuickSort>(2, "Quick Sort");

		return true;
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		DrawSubwindows();

		return true;
	}
};


int main()
{
	OLC_SORT game;
	game.Construct(795, 503, 2, 2, false, true);
	game.Start();

	return 0;
}
