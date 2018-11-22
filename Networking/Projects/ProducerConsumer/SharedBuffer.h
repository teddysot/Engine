#ifndef INCLUDED_SHAREDBUFFER_H
#define INCLUDED_SHAREDBUFFER_H

class SharedBuffer
{
public:
	SharedBuffer()
		: mStart(0)
		, mEnd(0)
	{}

	bool IsEmpty() const	{ return mStart == mEnd; }
	bool IsFull() const		{ return (mEnd - mStart) == kMax; }

	void Push(int value)	{ mData[mEnd++ % kMax] = value; }
	int Pop()				{ return mData[mStart++ % kMax]; }

private:
	static const int kMax = 3;
	int mData[kMax];
	int mStart;
	int mEnd;
};

#endif // #ifndef INCLUDED_SHAREDBUFFER_H
