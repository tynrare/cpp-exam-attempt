#include "Index.h"
#include <iostream>
#include <thread>

using namespace std;
namespace SecondTest{

	void ThreadTester::StartTest(){
		ThreadTester counter;

		auto increment_and_print = [&counter]() {
			for (int i = 0; i < 3; i++) {
				counter.increment();
				std::cout << std::this_thread::get_id() << ' ' << counter.get() << '\n';
			}
		};

		std::thread thread1(increment_and_print);
		std::thread thread2(increment_and_print);

		thread1.join();
		thread2.join();
	}

	SharedLock::SharedLock() : state(0) {};
	SharedLock::~SharedLock(){
		std::lock_guard<mutex> lock(mutex); //release mutex
	}

	void SharedLock::lock(){
		std::unique_lock<mutex> lock(_mutex);
		while (state & write_entered)
			gate1.wait(lock);
		state |= write_entered;
		while (state & n_readers)
			gate2.wait(lock);
	}
	void SharedLock::unlock()
	{
		std::lock_guard<mutex> lock(mutex);
		state = 0;
		gate1.notify_all();
	}
	void SharedLock::lock_shared()
	{
		std::unique_lock<mutex> lock(_mutex);
		while ((state & write_entered) || (state & n_readers) == n_readers)
			gate1.wait(lock);
		unsigned num_readers = (state & n_readers) + 1;
		state &= ~n_readers;
		state |= num_readers;
	}

	void SharedLock::unlock_shared()
	{
		std::lock_guard<mutex> lock(mutex);
		unsigned num_readers = (state & n_readers) - 1;
		state &= ~n_readers;
		state |= num_readers;
		if (state & write_entered)
		{
			if (num_readers == 0)
				gate2.notify_one();
		}
		else
		{
			if (num_readers == n_readers - 1)
				gate1.notify_one();
		}
	}
}
