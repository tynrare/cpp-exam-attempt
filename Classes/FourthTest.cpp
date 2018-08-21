#include "Index.h"

using namespace std;

namespace FourthTest{
	unsigned long int i_db_transaction::operationID = 0;
	std::map<std::string, db_entry> i_db_transaction::cache = std::map<std::string, db_entry>();

	void DBTester::StartTest(){
		i_db_transaction dbtrans;

		cout << "Test incorrect operation: " << endl;
		dbtrans.extract("key1");

		cout << "Test simple transaction: " << endl;
		dbtrans.begin();
		dbtrans.set("key1", "val1");
		dbtrans.commit();

		cout << "Test bigger transaction (100 enties): " << endl;
		dbtrans.begin();

		for(int i = 0; i < 100;i++)
			dbtrans.set("key"+to_string(i), "val"+to_string(i));

		dbtrans.commit();

		cout << "Now get something from \"cache\"" << endl;
		dbtrans.begin();
		dbtrans.extract("key93"); 
		dbtrans.extract("key97");
		dbtrans.extract("key99");
		dbtrans.commit();

		auto results = dbtrans.get_extract_results();
		cout << "Found enties: " << results.size() << endl;
		for(auto key : results)
			cout << "Got value: " << key.second << endl;
	}

	i_db_transaction::i_db_transaction() : 
		inTransaction(false)
	{
	}

	bool i_db_transaction::begin(){
		inTransaction = true;
		extracted_results.clear();

		return true;
	}
	
	bool i_db_transaction::abort(){
		clear_transaction_data();
		return true;
	}

	std::mutex databaseEmulation;
	bool i_db_transaction::commit(){
		databaseEmulation.lock();

		cout << "Perform delete operations" << endl;
		while(!delQueue.empty())
			delQueue.pop(); 

		cout << "Perform set operations" << endl;
		while(!setQueue.empty()){
			cache[setQueue.front().first] = db_entry { .id = operationID++, .value = setQueue.front().second };
			setQueue.pop();
		}

		cout << "Perform get operations" << endl;
		while(!getQueue.empty()){
			auto cached = cache.find(getQueue.front());

			string value;
			if(cached != cache.end())
				value = cached->second.value;
			else
				value = "not cachad db value";

			extracted_results[getQueue.front()] = value;

			cache[getQueue.front()] = db_entry { .id = operationID++, .value = value };

			getQueue.pop();
		}

		clear_transaction_data();

		databaseEmulation.unlock();

		return true;
	}

	void i_db_transaction::clear_transaction_data(){
		inTransaction = false;

		while(!delQueue.empty())
			delQueue.pop();
		while(!setQueue.empty())
			setQueue.pop();
		while(!getQueue.empty())
			getQueue.pop();

		if(cache.size() > maxCacheSize){
			cout << "Clear cache: " << cache.size() - maxCacheSize/2 << " values" << endl;
			for(auto key : cache){
				auto threshold = operationID - maxCacheSize/2;
				if(key.second.id < threshold)
					cache.erase(key.first);
			}
		}
	}
	std::map<std::string, std::string> i_db_transaction::get_extract_results(){
		return extracted_results;
	}

	bool i_db_transaction::extract(const std::string& key){
		if(!inTransaction) {
			cout << "Operations doesn't permitted outside transactions" << endl;
			return false;
		}
		getQueue.push(key);
		return true;
	}
	bool i_db_transaction::set(const std::string& key, const std::string& data){
		if(!inTransaction) {
			cout << "Operations doesn't permitted outside transactions" << endl;
			return false;
		}
		setQueue.push(make_pair(key, data));
		return true;
	}
	bool i_db_transaction::del(const std::string& key){
		if(!inTransaction) {
			cout << "Operations doesn't permitted outside transactions" << endl;
			return false;
		}
		delQueue.push(key);
		return true;
	}

	
}
