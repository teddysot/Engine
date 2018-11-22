//====================================================================================================
// Filename:	StreamWriter.h
// Created by:	Peter Chan
//====================================================================================================

#ifndef INCLUDED_NETWORK_STREAMWRITER_H
#define INCLUDED_NETWORK_STREAMWRITER_H

#include "MemoryStream.h"

namespace Network {

	class StreamWriter
	{
	public:
		StreamWriter(MemoryStream& memoryStream);
		~StreamWriter();

		void Write(const void* data, uint32_t size);
		void Write(const std::string& str);

		template <typename T>
		void Write(T data)
		{
			static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Generic Write only supports primitive data types");
			Write(&data, sizeof(T));
		}

		template <typename T>
		void Write(const std::vector<T>& data)
		{
			const size_t count = data.size();
			Write(count);
			for (const T& item : data)
			{
				Write(item);
			}
		}

	private:
		MemoryStream& mMemoryStream;
	};

} // namespace Network

#endif // #ifndef INCLUDED_NETWORK_STREAMWRITER_H