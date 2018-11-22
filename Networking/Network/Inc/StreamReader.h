//====================================================================================================
// Filename:	StreamReader.h
// Created by:	Peter Chan
//====================================================================================================

#ifndef INCLUDED_NETWORK_STREAMREADER_H
#define INCLUDED_NETWORK_STREAMREADER_H

#include "MemoryStream.h"

namespace Network {

	class StreamReader
	{
	public:
		StreamReader(MemoryStream& memoryStream);
		~StreamReader();

		void Read(void* data, uint32_t size);
		void Read(std::string& str);

		template <typename T>
		void Read(T& data)
		{
			static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Generic Read only supports primitive data types");
			Read(&data, sizeof(T));
		}

		template <typename T>
		void Read(std::vector<T>& data)
		{
			size_t count = 0;
			Read(count);
			data.resize(count);
			for (T& element : data)
			{
				Read(element);
			}
		}

		uint32_t GetRemainingDataSize() const
		{
			return mMemoryStream.mCapacity - mMemoryStream.mHead;
		}

	private:
		MemoryStream& mMemoryStream;
	};

} // namespace Network

#endif // #ifndef INCLUDED_NETWORK_STREAMREADER_H