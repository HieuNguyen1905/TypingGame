#include "highscore.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <string>
using namespace std;
int saveHighScore(int score)
{
    fstream in(SAVE_PATH);

	if (in.is_open()) {
		in >> highscore;
		in.close();
	}
	if (score > highscore) {
		highscore = score;
		ofstream out;
		out.open(SAVE_PATH);
		if (out.is_open()) {
			out << highscore;
			out.close();
		}
	}
    return 0;
}
void readHighScore(int &HIGHSCORE) {
	fstream in(SAVE_PATH);

	if (in.is_open()) {
		in >> highscore;
		HIGHSCORE = highscore;
		in.close();
	}

}
