#include "stdio.h"
#include <algorithm>  // For shuffle  
#include <random>     // For default_random_engine  
#include <chrono>     // For chrono::system_clock  
#include <map>  
#include <unordered_map>  
#include <omp.h>
#include <cstdlib>  
#include <iostream>  

using namespace std;

vector<vector<int>> fs_reels = {
	  {1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 11, 11, 11},      // fs_reel1  
	  {1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 6, 6, 6, 6, 6, 6, 6, 7, 7, 8, 8, 8, 9, 9, 10, 10, 10, 10, 10, 11}, // fs_reel2  
	  {2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 9, 9, 10}, // fs_reel3  
	  {2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 8, 9, 9, 10, 11, 11}, // fs_reel4  
	  {1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 8, 8, 10, 10, 10, 10, 10, 11}, // fs_reel5  
	  {2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 8, 9, 10, 10, 11, 11, 11, 11}  // fs_reel6  
};

vector<vector<int>> main_reels = {
	{ 1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,4,4,4,4,4,5,5,5,5,5,5,6,6,6,6,7,7,7,7,8,8,8,9,9,10,10 },
	{ 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 10, 10, 10 },
	{ 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 10, 10, 10, 10 },
	{ 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 10, 10 },
	{ 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 9, 9, 10, 10 },
	{ 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 8, 8, 9, 9, 9, 10 }
};


map<int, map<int, double>> payTable{
	{1, {{4, 5}, {5, 10}, {6, 25}, {7, 25}, {8, 25}, {9, 25}, {10, 25}, {11, 25}, {12, 25}}},
	{2, {{8, 0.4}, {9, 0.4}, {10, 1.5}, {11, 1.5}, {12, 5}}},
	{3, {{8, 0.5}, {9, 0.5}, {10, 2}, {11, 2}, {12, 10}}},
	{4, {{8, 1}, {9, 1}, {10, 5}, {11, 5}, {12, 10}}},
	{5, {{8, 1.5}, {9, 1.5}, {10, 5}, {11, 5}, {12, 15}}},
	{6, {{8, 2.5}, {9, 2.5}, {10, 12}, {11, 12}, {12, 25}}},
	{7, { {8, 3}, {9, 3}, {10, 15}, {11, 15}, {12, 30}}},
	{8, { {8, 5}, {9, 5}, {10, 20}, {11, 20}, {12, 40} }},
	{9, { {8, 8}, {9, 8}, {10, 25}, {11, 25}, {12, 50} }},
	{10, { {8, 10}, {9, 10}, {10, 35}, {11, 35}, {12, 60} }},
};

int bomb[] = {
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
	12, 12, 12, 12, 12, 12, 12, 12,
	15, 15, 15, 15, 15, 15,
	20, 20, 20, 20,
	25, 25,
	50
};


int freeSpinRemain = 0;
int gameMode = 0;
double totalWin = 0;

double normalwin = 0;
double freewin = 0;
double freebase = 0;
int freewincount = 0;

vector<vector<int>> symbolRates = {
	{0,0,0,0,0,0,0,0,0} ,
	{0,0,0,0,0},
	{0,0,0,0,0},
	{0,0,0,0,0},
	{0,0,0,0,0},
	{0,0,0,0,0},
	{0,0,0,0,0},
	{0,0,0,0,0},
	{0,0,0,0,0},
	{0,0,0,0,0},
};

struct SymbolData {
	int count;
	vector<pair<int, int>> positions;
};

struct WinResult {
	vector<pair<int, int>> wins; // Positions of wins  
	double winValue; // Value of the win  
};

int randomInt(int min, int max) {
	static random_device rd;
	static mt19937 eng(rd());

	// Create a distribution in the range [min, max-1]  
	uniform_int_distribution<> distr(min, max - 1);

	return distr(eng);
}

void shuffleReels(vector<vector<int>>& reels) {
	// Create random engine and seed it  
	default_random_engine engine(static_cast<unsigned int>(time(0)));

	// Shuffle each row independently  
	for (auto& reel : reels) {
		shuffle(reel.begin(), reel.end(), engine);
	}

	// Alternatively, to shuffle the rows themselves, uncomment the following line:  
	// shuffle(reels.begin(), reels.end(), engine);  
}

vector<pair<int, int>> checkViewAndGetWins(const vector<vector<int>>& view) {
	const int defaultThreshold = 8;
	unordered_map<int, int> specialThreshold = { {1, 4} };

	vector<pair<int, int>> wins;
	map<int, SymbolData> symbolCount;

	// Initialize map to track count and positions for each symbol  
	for (int value = 1; value < 11; ++value) {
		symbolCount[value] = { 0, {} };
	}

	// Traverse grid and populate map with counts and positions  
	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 6; ++j) {
			int value = view[i][j];
			if (value >= 1 && value < 11) {
				SymbolData& record = symbolCount[value];
				record.count++;
				record.positions.emplace_back(i, j);
				if (record.count > 12) {
					record.count = 12;
				}
			}
		}
	}

	for (auto& entry : symbolCount) {
		int symbol = entry.first;
		SymbolData& data = entry.second;
		int threshold = specialThreshold.count(symbol) ? specialThreshold.at(symbol) : defaultThreshold;
		if (data.count >= threshold) {
			if (symbol != 1) {
				wins.insert(wins.end(), data.positions.begin(), data.positions.end());
			}

			if (payTable[symbol][data.count] != 0) {
				double win = payTable[symbol][data.count];
				totalWin += win;
				normalwin += win;
				symbolRates[symbol - 1][data.count - threshold]++;

				if (symbol == 1) {
					gameMode = 1;
				}
			}
		}
	}

	return wins;
}

void removeWinsFromView(vector<vector<int>>& view, const vector<pair<int, int>>& wins) {
	for (const auto& win : wins) {
		view[win.first][win.second] = -1; // Set winning positions to -1 (or another 'null' equivalent)  
	}
}

void dropSymbols(vector<vector<int>>& view, vector<int>& bottomNumber, const vector<vector<int>>& reelset) {
	int height = 5;
	int width = 6; // Assuming the grid is not empty and all rows are the same width  

	for (int col = 0; col < width; col++) {
		int pointer = height - 1;

		// Move non-null symbols down  
		for (int row = height - 1; row >= 0; row--) {
			if (view[row][col] != -1) { // Assuming -1 is the null value  
				view[pointer][col] = view[row][col];
				if (pointer != row) {
					view[row][col] = -1;
				}
				pointer--;
			}
		}

		// Fill the top empty spaces with new symbols from the reel  
		int indexInReel = bottomNumber[col];
		const vector<int>& reel = reelset[col];

		while (pointer >= 0) {
			view[pointer][col] = reel[indexInReel];
			pointer--;

			indexInReel = (indexInReel + 1) % reel.size(); // Ensure the index loops around the reel  
		}

		// Update bottomNumber[col] for the next drop  
		bottomNumber[col] = indexInReel;
	}
}

void tumble(vector<vector<int>>& view, vector<int>& bottomNumber) {
	bool anyWins;

	do {
		vector<pair<int, int>> wins = checkViewAndGetWins(view);
		anyWins = !wins.empty();
		if (anyWins) {
			removeWinsFromView(view, wins);
			dropSymbols(view, bottomNumber, main_reels);
		}
	} while (anyWins && gameMode == 0);
}

void baseModePlay() {
	const int rows = 5;
	const int reels = 6;
	vector<vector<int>> view(rows, vector<int>(reels));
	vector<int> bottomNumber(reels);

	// Precompute the sizes of each reel  
	vector<int> reelSizes(reels);
	for (int j = 0; j < reels; ++j) {
		reelSizes[j] = main_reels[j].size();
		bottomNumber[j] = randomInt(0, reelSizes[j] - 1); // Select a random start index for each reel  
	}

	// Populate the view  
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < reels; ++j) {
			int index = (bottomNumber[j] + i) % reelSizes[j];
			view[rows - 1 - i][j] = main_reels[j][index];
		}
	}

	// Update bottom numbers for all reels  
	for (int j = 0; j < reels; ++j) {
		bottomNumber[j] = (bottomNumber[j] + rows) % reelSizes[j];
	}

	tumble(view, bottomNumber);  // Ensure this function is optimized as well  
}

WinResult freeCheckWin(const vector<vector<int>>& view) {
	int rows = 5;
	int reels = 6;
	const int defaultThreshold = 8;
	unordered_map<int, int> specialThreshold = { {1, 4} };
	WinResult result;
	result.winValue = 0;

	vector<pair<int, int>> wins;
	map<int, SymbolData> symbolCount;

	// Initialize map to track count and positions for each symbol  
	for (int value = 1; value < 11; ++value) {
		symbolCount[value] = { 0, {} };
	}

	// Traverse grid and populate map with counts and positions  
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < reels; ++j) {
			int value = view[i][j];
			if (value >= 1 && value < 11) {
				SymbolData& record = symbolCount[value];
				record.count++;
				record.positions.emplace_back(i, j);
				if (record.count > 12) {
					record.count = 12;
				}
			}
		}
	}

	for (auto& item : symbolCount) {
		int symbol = item.first;
		SymbolData& data = item.second;
		if (symbol == 1 && data.count >= 3) {
			freeSpinRemain += 5;
		}
		int threshold = specialThreshold.count(symbol) ? specialThreshold.at(symbol) : defaultThreshold;

		if (data.count >= threshold) {
			result.wins.insert(result.wins.end(), data.positions.begin(), data.positions.end());
			if (payTable[symbol][data.count] != 0) {
				double win = static_cast<double>(payTable[symbol][data.count]);
				result.winValue += win;				
				freebase += win;
				symbolRates[symbol-1][data.count - threshold]++;
			}
		}
	}

	return result;
}


double determineMultiFromView(const vector<vector<int>>& view) {
	int multiplier = 0;
	int rows = 5;
	int reels = 6;
	

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < reels; ++j) {
			if (view[i][j] == 11) {
				
				int randomIndex = randomInt(0, 257);				
				multiplier += bomb[randomIndex];			

			}
		}
	}

	return (multiplier == 0) ? 1 : multiplier; // Return 1 if no multiSymbols are found; otherwise, return the accumulated multiplier  
}

void freeTumble(vector<vector<int>>& view, vector<int>& bottomNumber) {
	bool anyWins = false;
	double winValue = 0;

	do {
		WinResult winResult = freeCheckWin(view); // Evaluate the current view for wins  
		const auto& wins = winResult.wins;
		const double win = winResult.winValue;

		anyWins = !wins.empty(); // Check if there are any wins  
		if (anyWins) {
			removeWinsFromView(view, wins); // Remove win positions by setting them to a predefined null value e.g., -1  
			dropSymbols(view, bottomNumber, fs_reels); // Let symbols above drop down  
			winValue += win; // Accumulate win value  
		}
	} while (anyWins);  // If there were wins, recheck the new view state  

	// Apply multiplier and update total win if there were any wins  
	if (winValue != 0) {
		freewincount++;
		double multiplier = determineMultiFromView(view); // Calculate a multiplier based on the current view
	
		totalWin += multiplier * winValue; // Add to the total win counter 
		freewin += multiplier * winValue;
	}
}


void freeModePlay() {
	int totalPlayed = 0;
	int rows = 5;
	int reels = 6;
	freeSpinRemain = 10;

	while (freeSpinRemain > 0 && totalPlayed < 500) {

		vector<vector<int>> view(rows, vector<int>(reels));
		vector<int> bottomNumber(reels);
		for (int j = 0; j < reels; j++) {
			bottomNumber[j] = randomInt(0, fs_reels[j].size());
		}

		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < reels; j++) {
				int idx = (bottomNumber[j] + i) % fs_reels[j].size();
				view[rows - 1 - i][j] = fs_reels[j][idx];
			}
		}

		for (int i = 0; i < reels; i++) {
			bottomNumber[i] = (bottomNumber[i] + rows) % fs_reels[i].size();
		}
		totalPlayed++;
		freeTumble(view, bottomNumber);
		freeSpinRemain--;
	}
}

void simulateGame(const int totalPlayCount) {
	#pragma omp parallel  
	{
		vector<vector<int>> local_main_reels = main_reels;
		vector<vector<int>> local_fs_reels = fs_reels;
		#pragma omp for reduction(+:totalWin,normalwin,freewin,freebase,freewincount) private(gameMode)

		for (int playCount = 0; playCount < totalPlayCount; playCount++) {
			if (gameMode == 0) {
				shuffleReels(local_main_reels);
				baseModePlay();
		
			}
			else {
				shuffleReels(local_fs_reels);
				freeModePlay();
				gameMode = 0;
	
			}
		}
	}
}
int main(int argc, char ** argv) 
{
	int totalPlayCount = 100000;
	int playCount = 0;
	omp_set_num_threads(omp_get_num_procs());

	
	//#pragma omp parallel for
	double totaltime = 0;
	auto start = chrono::high_resolution_clock::now();
	simulateGame(100000);
	auto finish = chrono::high_resolution_clock::now();
	chrono::duration<double, milli> elapsed = finish - start;
	cout << elapsed.count();
	return 0;
}