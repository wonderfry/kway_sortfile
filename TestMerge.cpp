#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>
// local includes
#include "kwaymergesort.h"
using namespace std;

class KwayMergeSort;

// comparison functions for the ext. merge sort	
bool alphaAsc(const string &a, const string &b) {
	return a < b;
}

int main(int argc, char* argv[]) {

	const string inFile = argv[1];
	int bufferSize = 100000; // allow the sorter to use 100Kb (base 10) of memory for sorting.
							 // once full, it will dump to a temp file and grab another chunk.
	string tempPath = "./"; // allows you to write the intermediate files anywhere you want.
	ofstream outfile("sortstringFile");
	KwayMergeSort *sorter = new KwayMergeSort(inFile, &outfile, alphaAsc,bufferSize, tempPath);


	sorter->Sort();
}
