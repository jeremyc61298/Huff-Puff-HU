// huff.cpp
// Author: Jeremy Campbell
// This program will compress a file using 
// the Huffman compression algorithm.
#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>

using namespace std;

const short MAX_FILE_NAME = 80;
const short MAX_HUFFMAN_TABLE = 513;
const int END_OF_FILE = 256;

// TODO: Remove temp constant

struct HuffmanNode {
	int glyph;
	int frequency = 0;
	int leftChildIndex, rightChildIndex = -1;
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
	//cout << "File to compress: ";
	//cin >> filename;

	// START the clock
	start = clock();

	// Assuming the file exists
	ifstream fin(filename, ios::binary | ios::in | ios::ate);

	// Assuming that the file is smaller than RAM
	long long finSize = fin.tellg();
	unsigned char* contents = new unsigned char[finSize];
	fin.seekg(0, ios::beg);
	fin.read((char*)contents, finSize);

	// Find frequencies of all of the glyphs in the file
	HuffmanNode huffmanTable[MAX_HUFFMAN_TABLE];

	for (int i = 0; i < finSize; i++) {
		int index = (int)contents[i];
		huffmanTable[index].frequency++;
		huffmanTable[index].glyph = index;
	}

	// Add EOF byte
	huffmanTable[END_OF_FILE].frequency++;
	huffmanTable[END_OF_FILE].glyph = END_OF_FILE;

	sort(huffmanTable, huffmanTable + MAX_HUFFMAN_TABLE, sortHuffmanTable);

	for (int i = 0; i < MAX_HUFFMAN_TABLE; i++) {
		if (huffmanTable[i].frequency == 0) {
			break;
		}

		cout << "Glyph: " << huffmanTable[i].glyph << ", freq: " << huffmanTable[i].frequency << endl;
	}

	// TODO: Huffman Algorithm


	// De-allocate dynamic memory
	delete[] contents;

	// END the clock
	end = clock();

	cout << "Time to compress: " << double((end - start) / CLOCKS_PER_SEC) << endl;
}