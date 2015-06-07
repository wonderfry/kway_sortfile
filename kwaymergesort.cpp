/*
 * kwaymergesort.cpp
 *
 *  Created on: 2015年6月7日
 *      Author: wonder
 */

#include "kwaymergesort.h"
#include "TempFile.h"

//************************************************
// IMPLEMENTATION
// Class methods and elements
//************************************************


bool isRegularFile(const string& filename) {
	struct stat buf;
	int i;
	//cout<<"file name is"<<filename<<endl;
	i = stat(filename.c_str(), &buf);
	if (i != 0) {
		cerr << "Error: can't determine file type of '" << filename << "': "
				<< strerror(errno) << endl;
		exit(1);
	}
	if (S_ISREG(buf.st_mode))
		return true;

	return false;
}
// destructor
KwayMergeSort::~KwayMergeSort(void) {
}

// API for sorting.

void KwayMergeSort::Sort() {
	DivideAndSort();
	Merge();
}

// change the buffer size used for sorting

void KwayMergeSort::SetBufferSize(int bufferSize) {
	_maxBufferSize = bufferSize;
}

// change the sorting criteria

void KwayMergeSort::SetComparison(
		bool (*compareFunction)(const T &a, const T &b)) {
	_compareFunction = compareFunction;
}

void KwayMergeSort::DivideAndSort() {

	istream *input = new ifstream(_inFile.c_str(), ios::in);
	// gzipped
	// if ((isGzipFile(_inFile) == true) && (isRegularFile(_inFile) == true)) {
	//     delete input;
	//     input = new igzstream(_inFile.c_str(), ios::in);
	// }

	// bail unless the file is legit
	if (input->good() == false) {
		cerr << "Error: The requested input file (" << _inFile
				<< ") could not be opened. Exiting!" << endl;
		exit(1);
	}
	vector<T> lineBuffer;
	lineBuffer.reserve(100000);
	unsigned int totalBytes = 0;  // track the number of bytes consumed so far.

	// track whether or not we actually had to use a temp
	// file based on the memory that was allocated
	_tempFileUsed = false;

	// keep reading until there is no more input data
	T line;
	/* while (getline(*input,line)) {

	 // add the current line to the buffer and track the memory used.
	 lineBuffer.push_back(line);
	 totalBytes += sizeof(line);  // buggy?

	 // sort the buffer and write to a temp file if we have filled up our quota
	 if (totalBytes > _maxBufferSize) {
	 if (_compareFunction != NULL)
	 sort(lineBuffer.begin(), lineBuffer.end(), *_compareFunction);
	 else
	 sort(lineBuffer.begin(), lineBuffer.end());
	 // write the sorted data to a temp file
	 WriteToTempFile(lineBuffer);
	 // clear the buffer for the next run
	 lineBuffer.clear();
	 _tempFileUsed = true;
	 totalBytes = 0;
	 }
	 }*/

	while (getline(*input, line)) {

		// sort the buffer and write to a temp file if we have filled up our quota
		if (totalBytes > _maxBufferSize) {
			if (_compareFunction != NULL)
				sort(lineBuffer.begin(), lineBuffer.end(), *_compareFunction);
			else
				sort(lineBuffer.begin(), lineBuffer.end());
			// write the sorted data to a temp file
			WriteToTempFile(lineBuffer);
			// clear the buffer for the next run
			lineBuffer.clear();
			_tempFileUsed = true;
			totalBytes = 0;
		}
		// add the current line to the buffer and track the memory used.
		lineBuffer.push_back(line);
		//totalBytes += strlen(line.c_str());  // buggy?
		totalBytes += sizeof(line);
	}

	// handle the run (if any) from the last chunk of the input file.
	if (lineBuffer.empty() == false) {
		// write the last "chunk" to the tempfile if
		// a temp file had to be used (i.e., we exceeded the memory)
		if (_tempFileUsed == true) {
			if (_compareFunction != NULL)
				sort(lineBuffer.begin(), lineBuffer.end(), *_compareFunction);
			else
				sort(lineBuffer.begin(), lineBuffer.end());
			// write the sorted data to a temp file
			WriteToTempFile(lineBuffer);

		}
		// otherwise, the entire file fit in the memory given,
		// so we can just dump to the output.
		else {
			if (_compareFunction != NULL)
				sort(lineBuffer.begin(), lineBuffer.end(), *_compareFunction);
			else
				sort(lineBuffer.begin(), lineBuffer.end());
			for (size_t i = 0; i < lineBuffer.size(); ++i)
				*_out << lineBuffer[i] << endl;
		}
	}
	input->clear();
	delete input;
}

void KwayMergeSort::WriteToTempFile(const vector<T> &lineBuffer) {


	TempFile *temp = new TempFile(_tempPath + _inFile);
	// write the contents of the current buffer to the temp file
	for (size_t i = 0; i < lineBuffer.size(); ++i) {

		temp->write(strlen(lineBuffer[i].c_str()), lineBuffer[i].c_str());
		temp->write(1, "\n");
	}

	temp->close();
	_vTempFileNames.push_back(temp->getFile());
	delete temp;

}

//---------------------------------------------------------
// MergeDriver()
//
// Merge the sorted temp files.
// uses a priority queue, with the values being a pair of
// the record from the file, and the stream from which the record came.
// SEE: http://stackoverflow.com/questions/2290518/c-n-way-merge-for-external-sort, post from Eric Lippert.
//----------------------------------------------------------

void KwayMergeSort::Merge() {

	// we can skip this step if there are no temp files to
	// merge.  That is, the entire inout file fit in memory
	// and thus we just dumped to stdout.
	if (_tempFileUsed == false)
		return;

	// open the sorted temp files up for merging.
	// loads ifstream pointers into _vTempFiles
	OpenTempFiles();

	// priority queue for the buffer.
	priority_queue<MERGE_DATA> outQueue;    //优先队列，类似于堆

	// extract the first line from each temp file
	T line;
	for (size_t i = 0; i < _vTempFiles.size(); ++i) {
		getline(*_vTempFiles[i], line);    //应该要允许空格
		outQueue.push(MERGE_DATA(line, _vTempFiles[i], _compareFunction));
	}

	// keep working until the queue is empty
	while (outQueue.empty() == false) {
		// grab the lowest element, print it, then ditch it.
		MERGE_DATA lowest = outQueue.top();
		// write the entry from the top of the queue
		*_out << lowest.data << endl;

		// remove this record from the queue
		outQueue.pop();
		// add the next line from the lowest stream (above) to the queue
		getline(*(lowest.stream), line);    //应该要允许空格
		if (*(lowest.stream))
			outQueue.push(MERGE_DATA(line, lowest.stream, _compareFunction));
	}
	// clean up the temp files.
	CloseTempFiles();
}

void KwayMergeSort::OpenTempFiles() {
	for (size_t i = 0; i < _vTempFileNames.size(); ++i) {

		ifstream *file;

		if (isRegularFile(_vTempFileNames[i]) == true) {
			file = new ifstream(_vTempFileNames[i].c_str(), ios::in);
		}
		// gzipped
		//else if ((isGzipFile(_vTempFileNames[i]) == true) && (isRegularFile(_vTempFileNames[i]) == true)) {
		//    file = new igzstream(_vTempFileNames[i].c_str(), ios::in);
		//}

		if (file->good() == true) {
			// add a pointer to the opened temp file to the list
			_vTempFiles.push_back(file);
		} else {
			cerr << "Unable to open temp file (" << _vTempFileNames[i]
					<< ").  I suspect a limit on number of open file handles.  Exiting."
					<< endl;
			exit(1);
		}
	}
}

void KwayMergeSort::CloseTempFiles() {
	// delete the pointers to the temp files.
	for (size_t i = 0; i < _vTempFiles.size(); ++i) {
		_vTempFiles[i]->close();
		delete _vTempFiles[i];
	}
	// delete the temp files from the file system.
	for (size_t i = 0; i < _vTempFileNames.size(); ++i) {
		 remove(_vTempFileNames[i].c_str());  // remove = UNIX "rm"
	}
}

