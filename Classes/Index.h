#include <stdio.h>
#include <iostream>
#include <regex>
#include <mutex>
#include <queue>
#include <condition_variable>

namespace FirstTest {
	class MainClass {
		public:
			MainClass(std::string readFile, std::string writeFile);
			static void generate_file(std::string name);
			void begin_sorting();
		private:
			std::string inputFileName, outputFileName;
			int minimumValuesPow = 9999, maximumValuesPow = 0;

			//each line - 26 bytes, 1mb is 40330 lines
			static constexpr int fileTotalLines = 40330*1000; //>1gb
			static constexpr int maxPermittedAlloc = 1024*1024*32;//32

			void cache_file_by_pow();
			void split_by_max_file_size();
			void sort_cached_files();
			void restore_file_from_cache();
	};

	template <class T>
	class WriteStack
	{
		public:
			WriteStack(int maxAlloc, std::string filename, bool clearExisting = true);
			~WriteStack();
			void push(T data);
			void push(const T *data, int length);
			void commit();
		private:
			std::string filename;
			const bool useWByte = false;

			T *buffer;
			bool hasDataInBuffer;
			int bufferPos;
			int bufferSize;
	};
}

namespace SecondTest {
	class SharedLock
	{
		//based on https://github.com/HowardHinnant/HowardHinnant.github.io/blob/master/shared_mutex
		public:
			SharedLock();
			~SharedLock();
			void lock();
			void unlock();
			void lock_shared();
			void unlock_shared();

		private:
			std::mutex _mutex;
			unsigned state;
			std::condition_variable gate1, gate2;

			static const unsigned write_entered = 1U << (sizeof(unsigned)*8 - 1);
			static const unsigned n_readers = ~write_entered;
	};

	class ThreadTester
	{
		//Test case from https://en.cppreference.com/w/cpp/thread/shared_mutex
		public:
			ThreadTester() = default;

			// Multiple threads/readers can read the counter's value at the same time.
			unsigned int get() const {
				sharedLocker.lock_shared();
				return value_;
				sharedLocker.unlock_shared();
			}

			// Only one thread/writer can increment/write the counter's value.
			void increment() {
				sharedLocker.unlock();
				value_++;
			}

			// Only one thread/writer can reset/write the counter's value.
			void reset() {
				sharedLocker.lock();
				value_ = 0;
				sharedLocker.unlock();
			}

			static void StartTest();
		private:
			mutable SharedLock sharedLocker;
			unsigned int value_ = 0;
	};
}
namespace ThirdTest {
	std::smatch wildcard_search(const std::string &subject, const std::string &wildcard);
}

namespace FourthTest {
	struct db_entry
	{
		unsigned long int id;
		std::string value;
	};
	class i_db_transaction
	{
		public:
			i_db_transaction();

			bool begin();
			bool commit();
			bool abort();

			bool extract(const std::string& key);
			bool set(const std::string& key, const std::string& data);
			bool del(const std::string& key);

			std::map<std::string, std::string> get_extract_results();
		private:
			static const unsigned long int maxCacheSize = 10;
			static unsigned long int operationID;
			static std::map<std::string, db_entry> cache;

			bool inTransaction;

			std::queue<std::string> delQueue;
			std::queue<std::pair<std::string, std::string>> setQueue;
			std::queue<std::string> getQueue;
			std::map<std::string, std::string> extracted_results;

			void clear_transaction_data();
	};

	class DBTester{
		public:
			static void StartTest();
	};
}
