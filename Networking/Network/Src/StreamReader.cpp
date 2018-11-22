//====================================================================================================
// Filename:	StreamReader.cpp
// Created by:	Peter Chan
//====================================================================================================

#include "Precompiled.h"
#include "StreamReader.h"

using namespace Network;

//----------------------------------------------------------------------------------------------------

StreamReader::StreamReader(MemoryStream& MemoryStream)
	: mMemoryStream(MemoryStream)
{
}

//----------------------------------------------------------------------------------------------------

StreamReader::~StreamReader()
{
}

//----------------------------------------------------------------------------------------------------

void StreamReader::Read(void* data, uint32_t size)
{
	const uint32_t newHead = mMemoryStream.mHead + size;
	ASSERT(newHead <= mMemoryStream.mCapacity, "[StreamReader] End of stream reached.");
	std::memcpy(data, mMemoryStream.mBuffer + mMemoryStream.mHead, size);
	mMemoryStream.mHead = newHead;
}

//----------------------------------------------------------------------------------------------------

void StreamReader::Read(std::string& str)
{
	size_t length = 0;
	Read(length);
	ASSERT(length <= GetRemainingDataSize(), "[StreamReader] End of stream reached.");
	str.assign((const char*)mMemoryStream.mBuffer + mMemoryStream.mHead, length);
	mMemoryStream.mHead += length;
}