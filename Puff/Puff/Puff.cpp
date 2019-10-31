#include <string>
#include <iostream>
#include <fstream>
#include <bitset>
#include <vector>

using std::string;
using std::cout;
using std::cin;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::endl;
using std::streampos;
using std::bitset;
using std::vector;

/*
	each node in the reconstructed huffman table will consist
	of a glyph, left child indicator and right child indicator.
	glyph frequency is not included, as it does not matter
	in the case of decompression.
*/
struct tableNode
{
	int glyph, leftChild, rightChild;
};

/*
	the decompressed file will consist of the following data
	in order: the length in bytes of the file name, the actual
	file name with the original file extension, the number of 
	entries in the huffman table (max. of 513), and the original
	file data before it was compressed.
*/
struct decompressedFile
{
	int fileNameLength = 0;
	char* fileName;
	int entriesInTable = 0;
	tableNode huffTable[513];
	string fileOutput;
};

void main()
{
	string fileName;

	// query user for .huf file to be decompressed
	cout << "File to decompress: ";
	cin >> fileName;

	// open .huf file for reading
	ifstream fin(fileName, ios::in | ios::binary);

	/*streampos begin, end;
	begin = fin.tellg();
	fin.seekg(0, ios::end);
	end = fin.tellg();
	int fileSize = end - begin;
	fin.seekg(0, ios::beg);

	vector <char> hufFile;*/
	

	if (fin.is_open())
	{
		// create decompressedFile object
		decompressedFile outFile;

		// the .huf file will have a consistent order of the first line :
		// <length of name> -<file name(with original extension)> -<size of huffman table>

		// populate the file name length of the decompressed file object
		fin.read((char*)&outFile.fileNameLength, sizeof(outFile.fileNameLength));

		// populate the file name (with original file extension) of the decompressed file object
		outFile.fileName = new char[outFile.fileNameLength + 1];
		fin.read(outFile.fileName, outFile.fileNameLength);
		//  place a null terminator at the end of the filename, otherwise
		// the filename will have junk at the end
		outFile.fileName[outFile.fileNameLength] = '\0';

		// populate the number of entries in the huffman table of the decompressed file object
		fin.read((char*)&outFile.entriesInTable, sizeof outFile.entriesInTable);

		// create output file with the given original file name
		ofstream fout(outFile.fileName, ios::out | ios::binary);

		// loop through the huffman table in the .huf file and populate the huffman table
		// in the decompressed file, starting at the 0th node up to the number represented
		// by the total entries.  it will read one table entry at a time by using the sizeof function.
		for (int currentNode = 0; currentNode < outFile.entriesInTable; currentNode++)
		{
			tableNode currentTableNode;
			fin.read((char*)&currentTableNode, sizeof(currentTableNode));
			outFile.huffTable[currentNode].glyph = currentTableNode.glyph;
			outFile.huffTable[currentNode].leftChild = currentTableNode.leftChild;
			outFile.huffTable[currentNode].rightChild = currentTableNode.rightChild;
		}

		// create a string object which will contain the original data from
		// the uncompressed file
		string originalFileData = "";
		bool endOfFile = false;
		
		unsigned char currentByte;
		
		// create a vector to hold the encoded data (remainder of
		// .huf file) and populate the vector with the data
		vector<unsigned char> encodedData;
		int currentVectorPosition = 0;
		
		// get first byte of the original file data
		fin.read((char*)&currentByte, sizeof currentByte);
		// read in one byte at a time until the end of file and
		// add each byte to the end of the encodedData vector
		while (!fin.eof())
		{
			encodedData.push_back(currentByte);
			fin.read((char*)&currentByte, sizeof currentByte);
		}

		// loop through the encodedData vector that was created in the 
		// previous step and use right to left decoding to get the original
		// file data
		currentVectorPosition = 0;
		int huffTablePosition = 0;
		unsigned char currentVectorValue;
		do
		{
			// loop through one byte at a time
			for (int bitPos = 0; bitPos < 8; bitPos++)
			{
				currentVectorValue = encodedData[currentVectorPosition];
				// check to see if current position is the end of file
				if (outFile.huffTable[huffTablePosition].glyph == 256)
					endOfFile = true;
				// check to see if current position is a glyph
				else if (outFile.huffTable[huffTablePosition].glyph != 256 &&
					outFile.huffTable[huffTablePosition].glyph != -1)
				{
					originalFileData += (char)outFile.huffTable[huffTablePosition].glyph;
					// start back at the root of the huff table
					huffTablePosition = 0;
					// if this is the last bit, go on to the next byte
					if (bitPos == 8)
					{
						bitPos = 0;
						currentVectorPosition++;
					}
				}
				// if the current position is not a glyph or end of file,
				// it must be a merge node
				else
				{
					// if the bit we're looking at is 1, move to right child in huffman table
					if (currentVectorValue & (unsigned char)pow(2.0, bitPos))
						huffTablePosition = outFile.huffTable[huffTablePosition].rightChild;
					// if the bit is 0, move to the left child in huffman table
					else
						huffTablePosition = outFile.huffTable[huffTablePosition].leftChild;
					if (bitPos == 8)
					{
						bitPos = 0;
						currentVectorPosition++;
					}
				}
			}
		} while (!endOfFile);
		
		// no suitable conversion function from string to const char * exists
		/*fout.write(originalFileData);*/
			   
		fout.close();
		fin.close();
	}
}