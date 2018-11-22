#include <iostream>
#include <thread>

using namespace std;

const int kMax = 10;

void thread_main(int i)
{
	cout << "I am thread " << i << endl;
}

int main()
{
	thread t[kMax];
	for (int i = 0; i < kMax; ++i)
	{
		t[i] = thread(thread_main, i);
	}

	cout << "I am main thread" << endl;

	for (int i = 0; i < kMax; i++)
	{
		t[i].join();
	}

	return 0;
}