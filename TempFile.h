#ifndef _TEMPFILE_H_
#define _TEMPFILE_H_

//---------------------------------------------------------------------------
// TripleBit
// (c) 2011 Massive Data Management Group @ SCTS & CGCL. 
//     Web site: http://grid.hust.edu.cn/triplebit
//
// This work is licensed under the Creative Commons
// Attribution-Noncommercial-Share Alike 3.0 Unported License. To view a copy
// of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300,
// San Francisco, California, 94105, USA.
//---------------------------------------------------------------------------

#include <fstream>
#include <string>
using namespace std;
typedef int ID;
//---------------------------------------------------------------------------
#if defined(_MSC_VER)
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif
//---------------------------------------------------------------------------
/// A temporary file
class TempFile {
private:
	/// The next id
	static unsigned id;

	/// The base file name
	std::string baseName;
	/// The file name
	std::string fileName;
	/// The output
	std::ofstream out;

	/// The buffer size
	static const unsigned bufferSize = 16384; //存放4k个ID
	/// The write buffer
	char writeBuffer[bufferSize];
	/// The write pointer
	unsigned writePointer;

	/// Construct a new suffix
	static std::string newSuffix();
	static std::string newSuffix(unsigned);

public:
	/// Constructor
	TempFile(const std::string& baseName);

	/// Destructor
	~TempFile();


	/// Get the file name
	const std::string& getFile() const {
		return fileName;
	}

	/// Flush the file
	void flush();
	/// Close the file
	void close();
	/// Discard the file
	void discard();



	/// Raw write
	void write(unsigned len, const char* data);


	bool isEmpty() {
		if (writePointer == 0)
			return true;
		else
			return false;
	}
};
#endif
