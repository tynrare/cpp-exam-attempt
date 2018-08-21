#include "Index.h"
#include <algorithm>

using namespace std;

std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

namespace ThirdTest{
	std::smatch wildcard_search(const std::string &subject, const std::string &wildcard){
		cout << "Trying to find " << wildcard << " in " << subject << endl;

		std::string str = wildcard;
		str = replaceAll(str, "?", ".");
		str = replaceAll(str, "*", ".*");

		cout << "Converted wildcard to regex: " << str << endl;

		std::smatch sm;
		std::regex e (str);

		std::regex_search (subject, sm, e);

		return sm;
	}
}

