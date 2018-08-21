#include <Index.h>

using namespace std;

namespace ProgramTypes
{
	const string _1_gen = "1-gen";
	const string _1_parse = "1-parse";
	const string _2_case = "2-case";
	const string _3_case = "3-case";
	const string _4_case = "4-case";
}

void PrintHelpMessage(){
	cout << "Plase, enter test name as first argument: \n\t"
		<< ProgramTypes::_1_gen << " - generate 1gb file\n\t"
		<< ProgramTypes::_1_parse << " - first test implementation\n\t"
		<< ProgramTypes::_2_case << " - shared_mutex implementation\n\t"
		<< ProgramTypes::_3_case << " - wildcards implementation\n\t"
		<< ProgramTypes::_4_case << " - cache implementation\n\t"
		<< endl;
}

int main(int argc, char *argv[]) {
	//---
	//Prepare
	if(argc > 1){
		string type(argv[1]);

		//#1---
		if(!type.compare(ProgramTypes::_1_gen)){
			cout << "Run first test generator" << endl;
			if(argc > 2){
				FirstTest::MainClass::generate_file(argv[2]);
			}
			else
				cout << "Please, enter output file name as second argument" << endl;
		}
		else if(!type.compare(ProgramTypes::_1_parse)){
			cout << "Run first test parser" << endl;
			if(argc > 3){
				FirstTest::MainClass test(argv[2], argv[3]);
				test.begin_sorting();
			}
			else
				cout << "Please, enter input file name as second argument and output name as third" << endl;
		}
		//#2---
		if(!type.compare(ProgramTypes::_2_case)){
			SecondTest::ThreadTester::StartTest();
		}
		//#3---
		if(!type.compare(ProgramTypes::_3_case)){
			cout << "Run third test case" << endl;
			if(argc > 3){
				auto result = ThirdTest::wildcard_search(argv[2], argv[3]);
				if(result.size() == 0)
					cout << "Can't find " << argv[3] << " in " << argv[2] << endl;
				else
					cout << "Found substring:    " << result[0] << endl;
			}
			else
				cout << "Please, enter subject string as second argument and wildcard as third" << endl;
		}
		//#4---
		if(!type.compare(ProgramTypes::_4_case)){
			FourthTest::DBTester::StartTest();
		}
	}
	else
		PrintHelpMessage();

	return 0;
}
