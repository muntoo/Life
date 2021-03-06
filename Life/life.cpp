// Authors:
//     Mateen Ulhaq,    301250828, <mulhaq@sfu.ca>
//     Greyson Wang,    301249759, <greysonw@sfu.ca>
//     Kismen Sneddon,  301265599, <ksneddon@sfu.ca>
//     Dayton Pukanich, 301252869, <dpukanic@sfu.ca>

// Start Date: 4 November 2014
// Completion Date: 13 November 2014

// Description: Computer simulation of Conway's Game of Life using an original board contained in a data file specified by the user.


// Includes
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ostream>
#include <string>
#include <cmath>


// Namespaces
using namespace std;


// Constants
const int MIN_ARRAY_SIZE = 0;
const int MAX_ARRAY_SIZE = 40;
const int MIN_GENERATIONS = 1;

const char DEAD_CELL = '.';
const char LIVE_CELL = 'X';

const int NEIGHBORS_OVERCROWDING = 4;  // Too many neighbors
const int NEIGHBORS_LONELINESS = 2;    // Too few neighbors
const int NEIGHBORS_BIRTH = 3;         // Enough neighbors


// Prototypes
bool ReadGen  (char lifeBoard[][MAX_ARRAY_SIZE], istream &inStream,          int &numRowsInBoard, int &numColsInBoard, int &generationNum);
void NextGen  (char lifeBoard[][MAX_ARRAY_SIZE],                             int numRowsInBoard,  int numColsInBoard);
void PrintGen (char lifeBoard[][MAX_ARRAY_SIZE], ostream &outStream,         int numRowsInBoard,  int numColsInBoard);
void PrintRow (char lifeRow[MAX_ARRAY_SIZE],     ostream &outStream,                              int numColsInBoard);
void CopyBoard(char dest[][MAX_ARRAY_SIZE],      char src[][MAX_ARRAY_SIZE], int numRowsInBoard,  int numColsInBoard);


// Main
int main()
{
	// Variables
	char lifeBoard[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE] = {DEAD_CELL};  // working array

	string inputFilename = "";
	string outputFilename = "";

	ifstream inputFile;
	ofstream outputFile;

	int numRowsInBoard = 0;
	int numColsInBoard = 0;
	int generationNum = 0;

	// Ask user for file names
	cout << "Enter the name of the input file: ";
	cin >> inputFilename;
	
	cout << "Enter the name of the output file: ";
	cin >> outputFilename;

	// Open files
	inputFile.open(inputFilename);

	if(inputFile.fail())
	{
		cerr << "ERROR: input file not opened correctly" << endl;
		return 1;
	}

	outputFile.open(outputFilename);
	
	if(outputFile.fail())
	{
		cerr << "ERROR: output file not opened correctly" << endl;
		inputFile.close();
		return 2;
	}

	// Read from file
	if(ReadGen(lifeBoard, inputFile, numRowsInBoard, numColsInBoard, generationNum))
	{
		// Error happened! End program.
		inputFile.close();
		outputFile.close();
		return 3;
	}

	// Output initial board
	cout << "LIFE initial game board\n" << endl;
	PrintGen(lifeBoard, cout, numRowsInBoard, numColsInBoard);
	cout << endl;

	outputFile << "LIFE initial game board\n" << endl;
	PrintGen(lifeBoard, outputFile, numRowsInBoard, numColsInBoard);
	outputFile << endl;

	// Loop through each generation
	for(int n = 0; n < generationNum; ++n)
	{
		// Calculate next generation
		NextGen(lifeBoard, numRowsInBoard, numColsInBoard);

		// Output new generation to cout
		// only for first and last generations
		if((n == 0) || (n == (generationNum - 1)))
		{
			cout << "\n\n\n" << "LIFE gameboard: generation " << (n + 1) << endl;
			PrintGen(lifeBoard, cout, numRowsInBoard, numColsInBoard);
			cout << endl;
		}

		// Output new generation to outputFile
		outputFile << "\n\n\n" << "LIFE gameboard: generation " << (n + 1) << endl;
		PrintGen(lifeBoard, outputFile, numRowsInBoard, numColsInBoard);
		outputFile << endl;
	}

	// Close files
	inputFile.close();
	outputFile.close();

	return 0;
}


// Copies src board and writes it into dest board.
void CopyBoard(char dest[][MAX_ARRAY_SIZE], char src[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE], int numRowsInBoard, int numColsInBoard)
{
	for(int j = 0; j < numRowsInBoard; ++j)
	{
		for(int i = 0; i < numColsInBoard; ++i)
		{
			dest[j][i] = src[j][i];
		}
	}
}


// Reads number parameters and board from given stream and checks to ensure original board conforms to expected standards.
bool ReadGen(char lifeBoard[][MAX_ARRAY_SIZE], istream &inStream, int &numRowsInBoard, int &numColsInBoard, int &generationNum)
{
	// Checks to ensure validity of number parameters in file
	if(!(inStream >> numRowsInBoard))
	{
		cerr << "ERROR: Cannot read number of numRowsInBoard";
		return 1;
	}

	if((numRowsInBoard <= MIN_ARRAY_SIZE) || (numRowsInBoard >= MAX_ARRAY_SIZE))
	{
		cerr << "ERROR: Read an illegal number of numRowsInBoard for the board";
		return 1;
	}

	if(!(inStream >> numColsInBoard))
	{
		cerr << "ERROR: Cannot read number of columns";
		return 1;
	}

	if((numColsInBoard <= MIN_ARRAY_SIZE) || (numColsInBoard >= MAX_ARRAY_SIZE))
	{
		cerr << "ERROR: Read an illegal number of columns for the board";
		return 1;
	}

	if(!(inStream >> generationNum))
	{
		cerr << "ERROR: Cannot read the number of generations";
		return 1;
	}

	if(generationNum < MIN_GENERATIONS)
	{
		cerr << "ERROR: Read an illegal number of generations";
		return 1;
	}

	// Build array
	for(int j = 0; j < numRowsInBoard; ++j)
	{
		for(int i = 0; i < numColsInBoard; ++i)
		{
			// Read next character from file
			do
			{
				if(!(inStream >> lifeBoard[j][i]))
				{
					cerr << "ERROR: Not enough data in the input file";
					return 1;
				}
			} while((lifeBoard[j][i] == ' ') || (lifeBoard[j][i] == '\t'));

			// Check if character is valid
			if((lifeBoard[j][i] != LIVE_CELL) && (lifeBoard[j][i] != DEAD_CELL))
			{
				cerr << "ERROR: Input data for initial board is incorrect" << endl;
				cerr << "Location (" << j << ", " << i << ") is not valid";
				return 1;
			}
		}
	}

	// Checks to ensure borders of initial board (generation 0) have no live organisms
	for(int j = 0; j < numRowsInBoard; ++j)
	{
		// Left column
		if(lifeBoard[j][0] != DEAD_CELL)
		{
			cerr << "ERROR: organisms are present in the border of the board, please correct your input file";
			return 1;
		}

		// Right column
		if(lifeBoard[j][numColsInBoard - 1] != DEAD_CELL)
		{
			cerr << "ERROR: organisms are present in the border of the board, please correct your input file";
			return 1;
		}
	}

	for(int i = 0; i < numColsInBoard; ++i)
	{
		// Top row
		if(lifeBoard[0][i] != DEAD_CELL)
		{
			cerr << "ERROR: organisms are present in the border of the board, please correct your input file";
			return 1;
		}

		// Bottom row
		if(lifeBoard[numRowsInBoard - 1][i] != DEAD_CELL)
		{
			cerr << "ERROR: organisms are present in the border of the board, please correct your input file";
			return 1;
		}
	}

	return 0;
}


// Outputs row to stream.
void PrintRow(char lifeRow[MAX_ARRAY_SIZE], ostream &outStream, int numColsInBoard)
{
	for(int i = 0; i < (numColsInBoard - 1); ++i)
	{
		outStream << lifeRow[i] << ' ';
	}

	outStream << lifeRow[numColsInBoard - 1];
}


// Outputs board to stream.
void PrintGen(char lifeBoard[][MAX_ARRAY_SIZE], ostream &outStream, int numRowsInBoard, int numColsInBoard)
{
	for(int j = 0; j < (numRowsInBoard - 1); ++j)
	{
		PrintRow(lifeBoard[j], outStream, numColsInBoard);
		outStream << endl;
	}

	PrintRow(lifeBoard[numRowsInBoard - 1], outStream, numColsInBoard);
}


// Calculates next board
void NextGen(char lifeBoard[][MAX_ARRAY_SIZE], int numRowsInBoard, int numColsInBoard)
{
	static char nextBoard[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE] = {DEAD_CELL};
	int counter = 0;

	for(int j = 1; j < (numRowsInBoard - 1); ++j)
	{
		for(int i = 1; i < (numColsInBoard - 1); ++i)
		{
			// Reset counter
			counter = 0;

			// Count number of alive neighbors

			if(lifeBoard[j + 1][i + 1] == LIVE_CELL)
			{
				counter++;
			}

			if(lifeBoard[j + 1][i] == LIVE_CELL)
			{
				counter++;
			}

			if(lifeBoard[j + 1][i - 1] == LIVE_CELL)
			{
				counter++;
			}

			if(lifeBoard[j][i - 1] == LIVE_CELL)
			{
				counter++;
			}

			if(lifeBoard[j][i + 1] == LIVE_CELL)
			{
				counter++;
			}

			if(lifeBoard[j - 1][i - 1] == LIVE_CELL)
			{
				counter++;
			}

			if(lifeBoard[j - 1][i] == LIVE_CELL)
			{
				counter++;
			}

			if(lifeBoard[j - 1][i + 1] == LIVE_CELL)
			{
				counter++;
			}
			
			// Determine if next generation is alive or dead based on number of neighbors

			if(lifeBoard[j][i] == LIVE_CELL)
			{
				// Death by Overcrowding
				if(counter >= NEIGHBORS_OVERCROWDING)
				{
					nextBoard[j][i] = DEAD_CELL;
				}
				// Death by Loneliness
				else if(counter < NEIGHBORS_LONELINESS)
				{
					nextBoard[j][i] = DEAD_CELL;
				}
				// Survival
				// IT LIIIVEEES!!!
				else
				{
					nextBoard[j][i] = LIVE_CELL;
				}
			}
			else if(lifeBoard[j][i] == DEAD_CELL)
			{
				// Birth
				if(counter == NEIGHBORS_BIRTH)
				{
					nextBoard[j][i] = LIVE_CELL;
				}
				// Remains dead
				else
				{
					nextBoard[j][i] = DEAD_CELL;
				}
			}
		}
	}

	// Fill borders with DEAD_CELL
	for(int j = 0; j < numRowsInBoard; j++)
	{
		nextBoard[j][0] = DEAD_CELL;        // left column
		nextBoard[j][numColsInBoard - 1] = DEAD_CELL; // right column
	}

	for(int i = 0; i < numColsInBoard; i++)
	{
		nextBoard[0][i] = DEAD_CELL;        // top row
		nextBoard[numRowsInBoard - 1][i] = DEAD_CELL; // bottom row
	}

	// Copy data lifeBoard = nextBoard
	CopyBoard(lifeBoard, nextBoard, numRowsInBoard, numColsInBoard);
}

