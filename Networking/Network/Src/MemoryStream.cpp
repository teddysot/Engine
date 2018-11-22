//====================================================================================================
// Filename:	MemoryStream.cpp
// Created by:	Peter Chan
//====================================================================================================

#include "Precompiled.h"
#include "MemoryStream.h"

using namespace Network;

//----------------------------------------------------------------------------------------------------

MemoryStream::MemoryStream()
	: mBuffer(nullptr)
	, mHead(0)
	, mCapacity(0)
{
}

//----------------------------------------------------------------------------------------------------

MemoryStream::MemoryStream(uint32_t capacity)
	: mBuffer(nullptr)
	, mHead(0)
	, mCapacity(0)
{
	ReallocBuffer(capacity);
}

//----------------------------------------------------------------------------------------------------

MemoryStream::MemoryStream(const uint8_t* data, uint32_t size, bool takeOwnership)
	: mBuffer(nullptr)
	, mHead(0)
	, mCapacity(size)
{
	if (takeOwnership)
	{
		mBuffer = const_cast<uint8_t*>(data);
	}
	else
	{
		mBuffer = static_cast<uint8_t*>(std::malloc(size));
		memcpy(mBuffer, data, size);
	}
}


//----------------------------------------------------------------------------------------------------

MemoryStream::~MemoryStream()
{
	std::free(mBuffer);
}

//----------------------------------------------------------------------------------------------------

void MemoryStream::Reset()
{
	mHead = 0;
}

//----------------------------------------------------------------------------------------------------

void MemoryStream::ReallocBuffer(uint32_t newCapacity)
{
	mBuffer = static_cast<uint8_t*>(std::realloc(mBuffer, newCapacity));
	mCapacity = newCapacity;
}