#include <string>
#include <iostream>
#include <fstream>

using std::string;
using std::cout;
using std::cin;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::endl;

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

// function templates
void setFileParameters(ifstream& fin, decompressedFile& outFile);
void createHuffmanTable(tableNode* huffmanTable, decompressedFile& outFile, ifstream& fin);

void main()
{
	string fileName;

	// query user for .huf file to be decompressed
	cout << "File to decompress: ";
	cin >> fileName;

	// open .huf file for reading
	ifstream fin(fileName, ios::in | ios::binary);

	if (fin.is_open())
	{
		// create decompressedFile object
		decompressedFile outFile;

		setFileParameters(fin, outFile);
		cout << outFile.fileNameLength << endl << outFile.fileName << endl << outFile.entriesInTable << endl;
		// create output file with the given original file name
		ofstream fout(outFile.fileName, ios::out | ios::binary);

		// allocate memory for huffman table
		tableNode* huffmanTable = new tableNode[outFile.entriesInTable];

		createHuffmanTable(huffmanTable, outFile, fin);

		fout.close();
		fin.close();
	}
}

/*
	this function takes the ifstream and decompressed objects by
	reference and sets the file name length, file name, and number
	of entries in the huffman table.

	the .huf file will have a consistent order of the first line:
	<length of name> - <file name (with original extension)> - <size of huffman table>
*/
void setFileParameters(ifstream& fin, decompressedFile& outFile)
{
	fin.read((char*)&outFile.fileNameLength, sizeof(outFile.fileNameLength));

	outFile.fileName = new char[outFile.fileNameLength + 1];
	fin.read(outFile.fileName, outFile.fileNameLength);
	//  place a null terminator at the end of the filename, otherwise
	// the filename will have junk at the end
	outFile.fileName[outFile.fileNameLength] = '\0';

	fin.read((char*)&outFile.entriesInTable, sizeof outFile.entriesInTable);
}

/*
	this function receives the empty huffman table, and both the ofstream and ifstream all
	by reference.  it loops through table in the .huf file and populates the huffman table
	in the decompressed file, starting at the 0th node up to the number represented by the
	total entries.  it will read one table entry at a time by using the sizeof function.
*/

void createHuffmanTable(tableNode* huffmanTable, decompressedFile& outFile, ifstream& fin)
{
	for (int currentNode = 0; currentNode < outFile.entriesInTable; currentNode++)
	{		
		tableNode currentTableNode;
		fin.read((char*)&currentTableNode, sizeof(currentTableNode));
		huffmanTable[currentNode].glyph = currentTableNode.glyph;
		huffmanTable[currentNode].leftChild = currentTableNode.leftChild;
		huffmanTable[currentNode].rightChild = currentTableNode.rightChild;
	}
}
