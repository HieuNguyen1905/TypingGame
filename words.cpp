﻿#include "words.h"
#include <string>
using namespace std;

int randomNumberX(double start, double end) {
    random_device rd; 
    mt19937 mt(rd()); 
    uniform_real_distribution<double> dist(start, end); 
    return dist(mt);
}
int randomNumberY(double start, double end) {
    random_device rd; 
    mt19937 mt(rd()); 
    uniform_real_distribution<double> dist(start, end);
    return dist(mt);
}


string getLine() {
    string line;
    int random = 0;
    int numOfLines = 0;
    ifstream file(WORD_PATH);
    vector<string> lines(500);

    while (file.is_open()) {
        for (auto i = 0; i < lines.size(); i++)
            getline(file, lines[i]);
        file.close();
    }

    return lines[randomNumberX(0, 500)];
}
