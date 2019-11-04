// huff.cpp
// Author: Jeremy Campbell
// This program will compress a file using 
// the Huffman compression algorithm.
#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <stack>
#include <string>

using namespace std;

const short MAX_FILE_NAME = 80;
const short MAX_HUFFMAN_TABLE = 513;
const int END_OF_FILE = 256;
const int MIN_HEAP_SIZE = 1;
const int ROOT = 0;
const int INVALID = -1;
const int BYTE_SIZE = 8;
const char* LEFT_HUFF_VALUE = "0";
const char* RIGHT_HUFF_VALUE = "1";
const string HUFF_EXT = "huf";

struct HuffmanNode {
	int glyph;
	int frequency = 0;
	int leftChildIndex = INVALID;
	int rightChildIndex = INVALID;
	string bitstring = "";
};

struct MinHuffmanNode {
	int glyph;
	int leftChildIndex = INVALID;
	int rightChildIndex = INVALID;
};

// This function will sort the list in acending order with an exception: 
// All nodes with a frequency of zero appear at the end of the list.  
bool sortHuffmanTable(const HuffmanNode& node1, const HuffmanNode& node2) {
	if (node1.frequency == 0)
		return false;
	if (node2.frequency == 0)
		return true;

	return node1.frequency < node2.frequency;
}

int main() {
	clock_t start, end;
	char filename[MAX_FILE_NAME] = "test.txt";
	cout << "File to compress: ";
	cin >> filename;

	// START the clock
	start = clock();

#pragma region inputFileProcessing
	// Assuming the file exists
	ifstream fin(filename, ios::binary | ios::in | ios::ate);

	// Assuming that the file is smaller than RAM
	long long finSize = fin.tellg();
	unsigned char* contents = new unsigned char[finSize];
	fin.seekg(0, ios::beg);
	fin.read((char*)contents, finSize);

	fin.close();

	// Find frequencies of all of the glyphs in the file
	HuffmanNode huffmanTable[MAX_HUFFMAN_TABLE];
	MinHuffmanNode minHuffmanTable[MAX_HUFFMAN_TABLE];

	for (int i = 0; i < finSize; i++) {
		int index = (int)contents[i];
		huffmanTable[index].frequency++;
		huffmanTable[index].glyph = index;
	}


#pragma endregion inputFileProcessing

#pragma region huffmanAlgorithm
	// Add EOF byte
	huffmanTable[END_OF_FILE].frequency++;
	huffmanTable[END_OF_FILE].glyph = END_OF_FILE;

	sort(huffmanTable, huffmanTable + MAX_HUFFMAN_TABLE, sortHuffmanTable);

	int numGlyphs = 0;
	for (int i = 0; i < MAX_HUFFMAN_TABLE; i++) {
		if (huffmanTable[i].frequency == 0) {
			numGlyphs = i;
			break;
		}
	}

	// Huffman Algorithm
	int endOfHeap = numGlyphs - 1;
	int nextFreeSlot = numGlyphs;
	int marked;
	int currentElementIndex;
	int leftChildIndex;
	int rightChildIndex;
	int currentFrequency;
	bool didReheap;
	for (int i = 0; i < numGlyphs - 1; i++) {
		// Mark whichever of the root's children have the lowest frequency
		marked = (endOfHeap <= MIN_HEAP_SIZE || huffmanTable[1].frequency <= huffmanTable[2].frequency) ? 1 : 2;
		huffmanTable[nextFreeSlot] = huffmanTable[marked];

		// Move last node in tree heap to marked slot
		huffmanTable[marked] = huffmanTable[endOfHeap];
		if (marked < endOfHeap) {

			currentElementIndex = marked;
			currentFrequency = huffmanTable[marked].frequency;
			didReheap = false;

			while (!didReheap) {
				leftChildIndex = (2 * currentElementIndex) + 1;
				rightChildIndex = (2 * currentElementIndex) + 2;

				if (rightChildIndex < endOfHeap && 
						huffmanTable[rightChildIndex].frequency < huffmanTable[leftChildIndex].frequency && 
						currentFrequency > huffmanTable[rightChildIndex].frequency) {
					swap(huffmanTable[rightChildIndex], huffmanTable[currentElementIndex]);
					currentElementIndex = rightChildIndex;
				}
				else if (leftChildIndex < endOfHeap && currentFrequency > huffmanTable[leftChildIndex].frequency) {
					swap(huffmanTable[leftChildIndex], huffmanTable[currentElementIndex]);
					currentElementIndex = leftChildIndex;
				}
				else {
					didReheap = true;
				}
			}
		}

		// Move root node to endOfHeap
		huffmanTable[endOfHeap] = huffmanTable[0];

		// Possibly speed this up with a reference to the root
		huffmanTable[0].glyph = -1;
		huffmanTable[0].frequency = huffmanTable[endOfHeap].frequency + huffmanTable[nextFreeSlot].frequency;

		currentElementIndex = 0;
		currentFrequency = huffmanTable[0].frequency;
		didReheap = false;

		if (marked < endOfHeap) {
			while (!didReheap) {
				leftChildIndex = (2 * currentElementIndex) + 1;
				rightChildIndex = (2 * currentElementIndex) + 2;

				if (rightChildIndex < endOfHeap &&
					huffmanTable[rightChildIndex].frequency < huffmanTable[leftChildIndex].frequency &&
					currentFrequency > huffmanTable[rightChildIndex].frequency) {
					swap(huffmanTable[rightChildIndex], huffmanTable[currentElementIndex]);
					currentElementIndex = rightChildIndex;
				}
				else if (leftChildIndex < endOfHeap && currentFrequency > huffmanTable[leftChildIndex].frequency) {
					swap(huffmanTable[leftChildIndex], huffmanTable[currentElementIndex]);
					currentElementIndex = leftChildIndex;
				}
				else {
					didReheap = true;
				}
			}
		}

		// Possibly speed this up with a reference to the root
		huffmanTable[currentElementIndex].leftChildIndex = endOfHeap;
		huffmanTable[currentElementIndex].rightChildIndex = nextFreeSlot;
		
		nextFreeSlot++;
		endOfHeap--;
	}

	// Copy data into minHuffmanTable
	for (int i = 0; i < nextFreeSlot; i++) {
		minHuffmanTable[i].glyph = huffmanTable[i].glyph;
		minHuffmanTable[i].leftChildIndex = huffmanTable[i].leftChildIndex;
		minHuffmanTable[i].rightChildIndex = huffmanTable[i].rightChildIndex;
	}

#pragma endregion huffmanAlgorithm

#pragma region buildBitstrings
	string bitstrings[MAX_HUFFMAN_TABLE / 2 + 1];
	
	// Post-order traversal
	stack<HuffmanNode> nodeStack;
	HuffmanNode current = huffmanTable[ROOT];
	long long numBitsWhenCompressed = 0;
	
	nodeStack.push(current);

	while (!nodeStack.empty()) {
		current = nodeStack.top();
		nodeStack.pop();

		// Found a leaf
		if (current.glyph != INVALID) {
			bitstrings[current.glyph] = current.bitstring;
			numBitsWhenCompressed += current.bitstring.size() * current.frequency;
			continue;
		}
		
		if (current.leftChildIndex != INVALID) {
			huffmanTable[current.leftChildIndex].bitstring = current.bitstring + LEFT_HUFF_VALUE;
			nodeStack.push(huffmanTable[current.leftChildIndex]);
		}

		if (current.rightChildIndex != INVALID) {
			huffmanTable[current.rightChildIndex].bitstring = current.bitstring + RIGHT_HUFF_VALUE;
			nodeStack.push(huffmanTable[current.rightChildIndex]);
		}
	}

	long long numBytesWhenCompressed = ceil((double)numBitsWhenCompressed / (double)BYTE_SIZE);
#pragma endregion buildBitstrings

#pragma region outputFileProcessing
	// Create outFileName
	string inFileName = filename;
	string outFileName = "";
	size_t dotPos = inFileName.find_last_of(".");

	if (dotPos == string::npos) {
		// The inFileName does not contain a "."
		outFileName = inFileName + "." + HUFF_EXT;
	}
	else {
		// Replace all the characters after the "." with DMP_EXT
		// This effectively creates a new string from the inFileName that 
		// has the same "base" name but a dump file extension.
		outFileName = inFileName.substr(0, dotPos + 1) + HUFF_EXT;
	}

	string outContents(numBytesWhenCompressed, '\0');
	char currentOutByte = '\0';
	short bitCount = 0;
	long long currentOutByteIndex = 0;

	for (int i = 0; i <= finSize; i++) {
		const string& currentBitString = (i != finSize) ? bitstrings[contents[i]] : bitstrings[END_OF_FILE];

		for (int j = 0; j < currentBitString.size(); j++) {
			// Filled a byte
			if (bitCount == BYTE_SIZE) {
				outContents[currentOutByteIndex] = currentOutByte;
				bitCount = 0;
				currentOutByte = '\0';
				currentOutByteIndex++;
			}

			// This code is modified from code that Dr. Ragsdale gave to the class
			// is the bit "on"?
			if (currentBitString[j] == '1')
			{
				// turn the bit on using the OR bitwise operator
				currentOutByte = currentOutByte | (unsigned char)pow(2.0, bitCount);
			}
			bitCount++;
		}
	}

	// Move last byte into outContents 
	outContents[currentOutByteIndex] = currentOutByte;

	ofstream fout(outFileName, ios::binary);

	// Output name of file
	unsigned int fileNameSize = inFileName.size();
	fout.write((char*)& fileNameSize, sizeof(unsigned int));
	fout.write((char*) inFileName.c_str(), fileNameSize);

	// Output huffman tree
	fout.write((char*)& nextFreeSlot, sizeof(int));
	fout.write((char*) minHuffmanTable, sizeof(MinHuffmanNode) * nextFreeSlot);

	// Output compressed data
	fout.write((char*)outContents.c_str(), numBytesWhenCompressed);

	fout.close();

#pragma endregion outputFileProcessing
	// De-allocate dynamic memory
	delete[] contents;

	// END the clock
	end = clock();

	cout << setprecision(5) << fixed;
	cout << "Time to compress: " << (double(end - start) / CLOCKS_PER_SEC) << endl;
}