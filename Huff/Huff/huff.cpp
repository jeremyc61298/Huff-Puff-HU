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
const int MIN_HEAP_SIZE = 1;

// TODO: Remove temp constant

struct HuffmanNode {
	int glyph;
	int frequency = 0;
	int leftChildIndex = -1; 
	int rightChildIndex = -1;
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
	int currentElement;
	int leftChild;
	int rightChild;
	int currentFrequency;
	bool didReheap;
	for (int i = 0; i < numGlyphs - 1; i++) {
		// Mark whichever of the root's children have the lowest frequency
		marked = (endOfHeap <= MIN_HEAP_SIZE || huffmanTable[1].frequency <= huffmanTable[2].frequency) ? 1 : 2;
		huffmanTable[nextFreeSlot] = huffmanTable[marked];

		// Move last node in tree heap to marked slot
		if (marked < endOfHeap) {
			huffmanTable[marked] = huffmanTable[endOfHeap];

			currentElement = marked;
			currentFrequency = huffmanTable[marked].frequency;
			didReheap = false;

			while (!didReheap) {

				leftChild = (2 * currentElement) + 1;
				rightChild = (2 * currentElement) + 2;
				if (leftChild <= endOfHeap && huffmanTable[leftChild].frequency < currentFrequency) {
					swap(huffmanTable[leftChild], huffmanTable[currentElement]);
					currentElement = leftChild;
				}
				else if (rightChild <= endOfHeap && huffmanTable[rightChild].frequency < currentFrequency) {
					swap(huffmanTable[rightChild], huffmanTable[currentElement]);
					currentElement = rightChild;
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

		currentElement = 0;
		currentFrequency = huffmanTable[0].frequency;
		didReheap = false;

		while (!didReheap) {

			leftChild = (2 * currentElement) + 1;
			rightChild = (2 * currentElement) + 2;
			if (leftChild < endOfHeap && huffmanTable[leftChild].frequency < currentFrequency) {
				swap(huffmanTable[leftChild], huffmanTable[currentElement]);
				currentElement = leftChild;
			}
			else if (rightChild < endOfHeap && huffmanTable[rightChild].frequency < currentFrequency) {
				swap(huffmanTable[rightChild], huffmanTable[currentElement]);
				currentElement = rightChild;
			}
			else {
				didReheap = true;
			}
		}

		// Possibly speed this up with a reference to the root
		huffmanTable[currentElement].leftChildIndex = endOfHeap;
		huffmanTable[currentElement].rightChildIndex = nextFreeSlot;
		
		nextFreeSlot++;
		endOfHeap--;
	}

	/*cout << "---------------------------------------" << endl;*/

	/*numGlyphs = 0;
	for (int i = 0; i < MAX_HUFFMAN_TABLE; i++) {
		if (huffmanTable[i].frequency == 0) {
			numGlyphs = i;
			break;
		}

		cout << "Glyph: " << huffmanTable[i].glyph << dec << ", left: " << huffmanTable[i].leftChildIndex << ", right: " << huffmanTable[i].rightChildIndex << endl;
	}*/

#pragma endregion huffmanAlgorithm

	// De-allocate dynamic memory
	delete[] contents;

	// END the clock
	end = clock();

	cout << "Time to compress: " << (double(end - start) / CLOCKS_PER_SEC) << endl;
}