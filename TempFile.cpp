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

#include "TempFile.h"
#include <sstream>
#include <cassert>
#include <cstring>

using namespace std;
//---------------------------------------------------------------------------
/// The next id
unsigned TempFile::id = 0;
//---------------------------------------------------------------------------
string TempFile::newSuffix()
// Construct a new suffix
{
	stringstream buffer;
	buffer << '.' << (id++);
	return buffer.str();
}
string TempFile::newSuffix(unsigned suffix)
// Construct a new suffix
{
	stringstream buffer;
	buffer << '.' << suffix;
	return buffer.str();
}
//---------------------------------------------------------------------------
TempFile::TempFile(const string& baseName) :
	baseName(baseName), fileName(baseName + newSuffix()), out(fileName.c_str(), ios::out | ios::binary | ios::trunc), writePointer(0)
// Constructor
{
}


//---------------------------------------------------------------------------
TempFile::~TempFile()
// Destructor
{
//	discard();
}
//---------------------------------------------------------------------------
void TempFile::flush()
// Flush the file
{
	if (writePointer) {
		out.write(writeBuffer, writePointer);
		writePointer = 0;
	}
	out.flush();
}
//---------------------------------------------------------------------------
void TempFile::close()
// Close the file
{
	flush();
	out.close();
}
//---------------------------------------------------------------------------
void TempFile::discard()
// Discard the file
{
	close();
	remove(fileName.c_str());//删除文件或目录
}

//---------------------------------------------------------------------------
void TempFile::write(unsigned len, const char* data)
// Raw write
{
	// Fill the buffer
	if (writePointer + len > bufferSize) {
		unsigned remaining = bufferSize - writePointer;
		memcpy(writeBuffer + writePointer, data, remaining);
		out.write(writeBuffer, bufferSize);
		writePointer = 0;
		len -= remaining;
		data += remaining;
	}
	// Write big chunks if any
	if (writePointer + len > bufferSize) {
		assert(writePointer==0);
		unsigned chunks = len / bufferSize;
		out.write(data, chunks * bufferSize);
		len -= chunks * bufferSize;
		data += chunks * bufferSize;
	}
	// And fill the rest
	memcpy(writeBuffer + writePointer, data, len);
	writePointer += len;
}
