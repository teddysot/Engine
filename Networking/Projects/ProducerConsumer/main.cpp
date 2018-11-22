#include "SharedBuffer.h"
#include <iostream>
#include <thread>
#include <mutex>

const int kProducers = 5;

SharedBuffer buffer;
std::mutex bufferMutex;
int done = 0;

void Producer()
{
	int total = 5;
	while (total > 0)
	{
		{
			std::lock_guard<std::mutex> lock(bufferMutex);
			if (!buffer.IsFull())
			{
				buffer.Push(total);
				--total;
			}
		}
	}
	done++;
}

void Consumer()
{
	const char* kSpaces = "     ";
	while (done < kProducers || !buffer.IsEmpty())
	{
		{
			std::lock_guard<std::mutex> lock(bufferMutex);
			if (!buffer.IsEmpty())
			{
				int value = buffer.Pop();
				const char* spaces = kSpaces + value;
				std::cout << "Consumed: " << spaces << value << std::endl;
			}
		}
	}
}

int main()
{
	std::thread producer_threads[kProducers];
	for (int i = 0; i < kProducers; ++i)
	{
		producer_threads[i] = std::thread(Producer);
	}
	std::thread consumer_thread(Consumer);

	for (int i = 0; i < kProducers; ++i)
	{
		producer_threads[i].join();
	}
	consumer_thread.join();

	system("pause");
 	return 0;
}