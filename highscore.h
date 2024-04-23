#ifndef HIGHSCORE_H
#include <fstream>
#include <string>
#include <filesystem>
#include <chrono>
#include <ctime>
#define SAVE_PATH "./resource/highscore.txt"
int saveHighScore(int score);
void readHighScore(int& HIGHSCORE);
static int highscore;
#endif

