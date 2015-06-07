#ifndef KWAYMERGESORT_H
#define KWAYMERGESORT_H

#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <vector>
#include <queue>
#include <cstdio>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h> //for basename()
using namespace std;


typedef string T;

class MERGE_DATA {

public:
	// data
	T data;
	istream *stream;
	bool (*compFunc)(const T &a, const T &b);

	// constructor
	MERGE_DATA(const T &data, istream *stream,
			bool (*compFunc)(const T &a, const T &b)) :
			data(data), stream(stream), compFunc(compFunc) {
	}

	// comparison operator for maps keyed on this structure
	bool operator <(const MERGE_DATA &a) const {
		// recall that priority queues try to sort from
		// highest to lowest. thus, we need to negate.
		return !(compFunc(data, a.data));
	}
};

//************************************************
// DECLARATION
// Class methods and elements
//************************************************

class KwayMergeSort {

public:

	// constructor, using custom comparison function
	/*KwayMergeSort(const string &inFile, ostream *out,
	 bool (*compareFunction)(const T &a, const T &b),
	 int maxBufferSize ,
	 string tempPath );

	 // constructor, using T's overloaded < operator.  Must be defined.
	 KwayMergeSort(const string &inFile, ostream *out, int maxBufferSize , string tempPath );
	 */
	KwayMergeSort(const string &inFile, ostream *out,
			bool (*compareFunction)(const T &a, const T &b)=NULL,
			int maxBufferSize = 1000000, string tempPath = "./") :
			_inFile(inFile), _out(out), _compareFunction(compareFunction), _tempPath(
					tempPath), _maxBufferSize(maxBufferSize) {
	}

	// constructor

	KwayMergeSort(const string &inFile, ostream *out,
			int maxBufferSize = 1000000, string tempPath = "./") :
			_inFile(inFile), _out(out), _compareFunction(NULL), _tempPath(
					tempPath), _maxBufferSize(maxBufferSize) {
	}
	KwayMergeSort() {
	}

	// destructor
	~KwayMergeSort(void);

	void Sort();            // Sort the data
	void SetBufferSize(int bufferSize);   // change the buffer size
	void SetComparison(bool (*compareFunction)(const T &a, const T &b)); // change the sort criteria

private:
	string _inFile;
	bool (*_compareFunction)(const T &a, const T &b);
	string _tempPath;
	vector<string> _vTempFileNames;
	vector<ifstream*> _vTempFiles;
	unsigned int _maxBufferSize;
	bool _tempFileUsed;
	ostream *_out;

	// drives the creation of sorted sub-files stored on disk.
	void DivideAndSort();

	// drives the merging of the sorted temp files.
	// final, sorted and merged output is written to "out".
	void Merge();

	void WriteToTempFile(const vector<T> &lines);
	void OpenTempFiles();
	void CloseTempFiles();
};

#endif /* KWAYMERGESORT_H */

