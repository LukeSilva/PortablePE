#include <cassert>
#include <fstream>
#include <iostream>

using namespace std;

void showUsage(const string &exec)
{
	cout << "Usage: " << exec << " HEAD BODY OUTPUT" << endl;
	cout << "\t" "(body = head + tail)" << endl;
	exit(1);
}

long getSize(ifstream &stream)
{
	long pos = stream.tellg();
	stream.seekg(0, ifstream::end);
	long size = stream.tellg();
	stream.seekg(pos);
	return size;
}

void copy(ifstream &src, ofstream &dest, long size)
{
	char *buffer = new char[size];
	src.read(buffer, size);
	dest.write(buffer, size);
	delete[] buffer;
}

int main(int argc, char **argv)
{
	if (argc != 4)
		showUsage(argv[0]);
	
	ifstream headFile(argv[1], ifstream::binary);
	ifstream bodyFile(argv[2], ifstream::binary);
	
	if (!headFile || !bodyFile)
		showUsage(argv[0]);
	
	ofstream output(argv[3], ifstream::binary);
	
	long headSize = getSize(headFile);
	long bodySize = getSize(bodyFile);
	long tailSize = bodySize - headSize;
	assert(tailSize >= 0);
	
	copy(headFile, output, headSize);
	headFile.seekg(headSize);
	copy(bodyFile, output, tailSize);
	
	return 0;
}

