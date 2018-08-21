#include "Index.h"
#include <iostream>
#include <locale>
#include <string>
#include <codecvt>
#include <fstream>
#include <random>
#include <iomanip>
#include <map>
#include <algorithm>
#include <cstdio>

using namespace FirstTest;
using namespace std;

inline bool isFileExists (const std::string& name) {
	ifstream f(name.c_str());
	return f.good();
}

void MainClass::generate_file(std::string name){
	//Yup, write in 16 because it makes file bigger:)
	//---
	//Open stream
	cout << "Generating file..." << endl;

	wchar_t buffer[13];
	const double maxDouble = std::numeric_limits<double>::max();
	stringstream ss;
	ss << scientific;

	for(int i = 0; i <= fileTotalLines; i++){
		ss << (double)rand() / RAND_MAX * maxDouble << endl;
	}

	std::wofstream wofs;
	wofs.open(name, std::ios::binary);
	wofs.imbue(
			std::locale(
				wofs.getloc(),
				new std::codecvt_utf16<wchar_t, 0x10ffff, std::consume_header>));
	wofs << ss.str().c_str();
	wofs.close();
	ss.clear();

	cout << "Done." << endl;
}

MainClass::MainClass(std::string readFile, std::string writeFile) :
	inputFileName(readFile),
	outputFileName(writeFile)
{
}

void MainClass::begin_sorting(){
	cache_file_by_pow();
	split_by_max_file_size();
	sort_cached_files();
	restore_file_from_cache();
}

void MainClass::cache_file_by_pow(){
	//---
	//Open stream
	cout << "Begin file reading..." << endl;
	std::wifstream wifs;
	wifs.open(inputFileName, std::ios::binary);
	wifs.imbue(
			std::locale(
				wifs.getloc(),
				new std::codecvt_utf16<wchar_t, 0x10ffff, std::consume_header>));

	//---
	//Start reading
	wchar_t singleWChar;
	char singleChar;
	string singleLine;

	//i'll write all values in different files by their power
	string singleLinePow;
	ofstream *lastWroteStream = nullptr;
	bool writeSingleLinePow = false, writeSingleLine = true;
	map<string, WriteStack<float>*> streamsMap;

	int valuesStored = 0;

	//reading cycle
	while(wifs.get(singleWChar)){
		//read char
		singleChar = singleWChar;

		//line end trigger
		if(singleChar == '\n'){
			auto fileKey = streamsMap.find(singleLinePow);
			WriteStack<float> *stack;
			if( fileKey == streamsMap.end() ){
				cout << "Create file: " << singleLinePow << endl;
				stack = new WriteStack<float>(maxPermittedAlloc/10, singleLinePow);
				streamsMap[singleLinePow] = stack;

				//store maximum and minimum files name
				int intpow = stoi(singleLinePow);
				if(minimumValuesPow > intpow)
					minimumValuesPow = intpow;
				if(maximumValuesPow < intpow)
					maximumValuesPow = intpow;
			}
			else
				stack = fileKey->second;

			stack->push(stof(singleLine));

			//clrear for next line
			singleLine.clear();
			singleLinePow.clear();
			writeSingleLinePow = false;
			writeSingleLine = true;
			valuesStored++;
			continue;
		}

		//Find double's power
		if(writeSingleLinePow)
			singleLinePow.push_back(singleChar);

		if(!writeSingleLinePow && singleChar == '+')
			writeSingleLinePow = true;
		if(writeSingleLine && singleChar == 'e')
			writeSingleLine = false;

		if(writeSingleLine)
			singleLine.push_back(singleChar);
	}

	cout << "Values stored: " << valuesStored << endl;

	//clean up
	for(auto key : streamsMap){
		key.second->commit();
		delete key.second;
	}
	wifs.close();
}
void MainClass::split_by_max_file_size(){
	cout << "Start splittion operation" << endl;

	for(int i = minimumValuesPow; i <= maximumValuesPow;i++){
		//open it first as ate and find size
		ifstream f(to_string(i), std::ifstream::ate | std::ifstream::binary);
		if(!f.good()) {
			cout << "Can't open file " << i << endl;
			continue;
		}
		auto filesize = f.tellg();
		f.close();

		int parts = filesize/(maxPermittedAlloc/10) + 1;
		float maxVal = (i == 308 ? 2 : 10); //308 is special case
		float chunkSize = maxVal/parts;
		cout << "File: " << i << endl
			<< "File size: " << filesize << endl
			<< "Split it in: " << parts << " parts" << endl;

		map<string, WriteStack<float>*> streamsMap;

		//sprit in into smaller chunks
		f.open(to_string(i));
		char valbuffer[4];
		float val;
		int part;
		string filename;
		while (f.read((char*)&val, sizeof(float)))
		{
			part = max(1, (int)ceil((val-1)/chunkSize));
			filename = to_string(i)+"-"+to_string(part);

			auto fileKey = streamsMap.find(filename);
			WriteStack<float> *stack;
			if( fileKey == streamsMap.end() ){
				stack = new WriteStack<float>(maxPermittedAlloc/parts, filename);
				streamsMap[filename] = stack;
			}
			else
				stack = fileKey->second;

			stack->push(val);
		}

		for(auto key : streamsMap){
			key.second->commit();
			delete key.second;
		}

		f.close();
	}
}

void MainClass::sort_cached_files(){
	cout << "Begin sorting..." << endl;
	
	int buffSize = maxPermittedAlloc/sizeof(float);
	float *buffer = new float[buffSize];

	for(int powi = minimumValuesPow; powi <= maximumValuesPow;powi++){
		cout << "Sort: " << powi << endl;
		for(int chunki = 1;;chunki++){
			string filename = to_string(powi)+"-"+to_string(chunki);
			ifstream ifs(filename, std::ifstream::ate | std::ifstream::binary);
			if(!ifs.good())
				break;

			int filesize = ifs.tellg();
			ifs.close();
			ifs.open(filename);

			ifs.read((char*)buffer, filesize);
			ifs.close();
			std::sort(buffer, buffer + filesize/sizeof(float), std::less<float>());
			ofstream ofs(filename);
			ofs.write((char*)buffer, filesize);
			ofs.close();
		}
	}

	delete[] buffer;
}

void MainClass::restore_file_from_cache(){
	cout << "Begin target file assembly..." << endl;

	WriteStack<char> stack(maxPermittedAlloc, outputFileName);
	float val;
	string sval;
	for(int powi = minimumValuesPow; powi <= maximumValuesPow;powi++){
		for(int chunki = 1;;chunki++){
			string filename = to_string(powi)+"-"+to_string(chunki);
			ifstream f(filename);
			if(!f.good())
				break;

			while (f.read((char*)&val, sizeof(float))){
				sval = to_string(val)+"e+"+to_string(powi)+'\n';
				stack.push(sval.c_str(), sval.length());
			}

		}
	}
}

template <class T>
WriteStack<T>::WriteStack(int maxAlloc, std::string filename, bool clearExisting) : 
	filename(filename)
{
	bufferSize = maxAlloc/sizeof(T);
	buffer = new T[bufferSize];

	if(clearExisting)
		remove(filename.c_str());

	hasDataInBuffer = false;
	bufferPos = 0;
}
template <class T>
WriteStack<T>::~WriteStack(){
	if(hasDataInBuffer)
		commit();

	delete[] buffer;
}

template <class T>
void WriteStack<T>::push(T data){
	hasDataInBuffer = true;
	buffer[bufferPos++] = data;

	if(bufferPos%bufferSize == 0)
		commit();
}
template <class T>
void WriteStack<T>::push(const T *data, int length){
	for(int i = 0;i < length;i++)
		push(data[i]);
}

template <class T>
void WriteStack<T>::commit(){
	if(!hasDataInBuffer) return;

	if(useWByte){
		std::wofstream wofs;
		wofs.open(filename, std::ios::binary | ios::app);
		wofs.imbue(
				std::locale(
					wofs.getloc(),
					new std::codecvt_utf16<wchar_t, 0x10ffff, std::consume_header>));
		wofs.write((wchar_t*)buffer, bufferPos*sizeof(T));

		wofs.flush();
		wofs.close();
	}
	else{
		ofstream f(filename, ios::binary | ios::app);
		f.write((char*)buffer, bufferPos*sizeof(T));
		f.flush();
		f.close();
	}

	bufferPos = 0;
	hasDataInBuffer = false;
}
