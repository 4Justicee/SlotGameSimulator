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

struct ResultData {
	double total_win;
	std::vector<std::vector<int>> win_symbols;

	// Default constructor  
	ResultData() : total_win(0), win_symbols({
		{0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0}
		}) {}
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

vector<pair<int, int>> checkViewAndGetWins(const vector<vector<int>>& view, int &gameMode, ResultData* result) {
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
				result->total_win += win;
				result->win_symbols[symbol - 1][data.count - threshold]++;

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

void tumble(vector<vector<int>>& view, vector<int>& bottomNumber, int &gameMode, vector<vector<int>> reelset, ResultData* result) {
	bool anyWins;

	do {
		vector<pair<int, int>> wins = checkViewAndGetWins(view, gameMode, result);
		anyWins = !wins.empty();
		if (anyWins) {
			removeWinsFromView(view, wins);
			dropSymbols(view, bottomNumber, reelset);
		}
	} while (anyWins && gameMode == 0);
}

void baseModePlay(int & gameMode, vector<vector<int>> reelset, ResultData *result) {
	const int rows = 5;
	const int reels = 6;
	vector<vector<int>> view(rows, vector<int>(reels));
	vector<int> bottomNumber(reels);

	// Precompute the sizes of each reel  
	vector<int> reelSizes(reels);
	for (int j = 0; j < reels; ++j) {
		reelSizes[j] = reelset[j].size();
		bottomNumber[j] = randomInt(0, reelSizes[j] - 1); // Select a random start index for each reel  
	}

	// Populate the view  
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < reels; ++j) {
			int index = (bottomNumber[j] + i) % reelSizes[j];
			view[rows - 1 - i][j] = reelset[j][index];
		}
	}

	// Update bottom numbers for all reels  
	for (int j = 0; j < reels; ++j) {
		bottomNumber[j] = (bottomNumber[j] + rows) % reelSizes[j];
	}

	tumble(view, bottomNumber, gameMode, reelset, result);  // Ensure this function is optimized as well  
}

WinResult freeCheckWin(const vector<vector<int>>& view, int & free_spin_remain, ResultData * p) {
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
			free_spin_remain += 5;
		}
		int threshold = specialThreshold.count(symbol) ? specialThreshold.at(symbol) : defaultThreshold;

		if (data.count >= threshold) {
			result.wins.insert(result.wins.end(), data.positions.begin(), data.positions.end());
			if (payTable[symbol][data.count] != 0) {
				double win = static_cast<double>(payTable[symbol][data.count]);
				result.winValue += win;				
				p->win_symbols[symbol-1][data.count - threshold]++;
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

void freeTumble(vector<vector<int>>& view, vector<int>& bottomNumber, int &free_spin_remain, vector<vector<int>>  reelset, ResultData *result) {
	bool anyWins = false;
	double winValue = 0;

	do {
		WinResult winResult = freeCheckWin(view, free_spin_remain, result); // Evaluate the current view for wins  
		const auto& wins = winResult.wins;
		const double win = winResult.winValue;

		anyWins = !wins.empty(); // Check if there are any wins  
		if (anyWins) {
			removeWinsFromView(view, wins); // Remove win positions by setting them to a predefined null value e.g., -1  
			dropSymbols(view, bottomNumber, reelset); // Let symbols above drop down  
			winValue += win; // Accumulate win value  
		}
	} while (anyWins);  // If there were wins, recheck the new view state  

	// Apply multiplier and update total win if there were any wins  
	if (winValue != 0) {
		double multiplier = determineMultiFromView(view); // Calculate a multiplier based on the current view
		result->total_win += multiplier * winValue; // Add to the total win counter 		
	}
}


void freeModePlay(vector<vector<int>>  reelset, ResultData * result) {
	int totalPlayed = 0;
	int rows = 5;
	int reels = 6;
	int free_spin_remain = 10;

	while (free_spin_remain > 0 && totalPlayed < 500) {

		vector<vector<int>> view(rows, vector<int>(reels));
		vector<int> bottomNumber(reels);
		for (int j = 0; j < reels; j++) {
			bottomNumber[j] = randomInt(0, reelset[j].size());
		}

		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < reels; j++) {
				int idx = (bottomNumber[j] + i) % reelset[j].size();
				view[rows - 1 - i][j] = reelset[j][idx];
			}
		}

		for (int i = 0; i < reels; i++) {
			bottomNumber[i] = (bottomNumber[i] + rows) % reelset[i].size();
		}
		totalPlayed++;
		freeTumble(view, bottomNumber, free_spin_remain, reelset, result);
		free_spin_remain--;
	}
}

void mergeResults(ResultData* finalResult, ResultData* threadResult) {
	// Sum up the total wins  
	finalResult->total_win += threadResult->total_win;

	// Sum up the win symbols by iterating over the matrix  
	for (size_t i = 0; i < threadResult->win_symbols.size(); ++i) {
		for (size_t j = 0; j < threadResult->win_symbols[i].size(); ++j) {
			finalResult->win_symbols[i][j] += threadResult->win_symbols[i][j];
		}
	}
}

void simulateGame(int total) {


		int gameMode = 0;
		ResultData result;

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

		
		

		for (int playCount = 0; playCount < total; playCount++) {

			if (gameMode == 0) {
				shuffleReels(main_reels);
				baseModePlay(gameMode, main_reels, &result);
			}
			else {
				shuffleReels(fs_reels);
				freeModePlay(fs_reels, &result);
				gameMode = 0;
			}
		}


	cout << result.total_win << '\n';
}

int main(int argc, char ** argv) 
{
	int totalPlayCount = 1000000;	

	double totaltime = 0;
	auto start = chrono::high_resolution_clock::now();
	
	simulateGame(totalPlayCount);

	auto finish = chrono::high_resolution_clock::now();
	chrono::duration<double, milli> elapsed = finish - start;
	cout << elapsed.count();
	return 0;
}