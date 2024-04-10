#include "words.h"
#include <string>
using namespace std;

int randomNumberX(double start, double end) {
    random_device rd; // Thiết bị ngẫu nhiên, được sử dụng để tạo ra các giá trị ngẫu nhiên
    mt19937 mt(rd()); // Tham số của pp tạo số ngẫu nhiên
    uniform_real_distribution<double> dist(start, end); //Đây là phân phối đồng đều, dùng để tạo ra các giá trị ngẫu nhiên trong 1 phạm vi cụ thể
    return dist(mt);
}
int randomNumberY(double start, double end) {
    random_device rd; // Thiết bị ngẫu nhiên, được sử dụng để tạo ra các giá trị ngẫu nhiên
    mt19937 mt(rd()); // Tham số của pp tạo số ngẫu nhiên
    uniform_real_distribution<double> dist(start, end); //Đây là phân phối đồng đều, dùng để tạo ra các giá trị ngẫu nhiên trong 1 phạm vi cụ thể
    return dist(mt) + 2;
}


string getLine() {
    string line;
    int random = 0;
    int numOfLines = 0;
    ifstream file(WORD_PATH);
    vector<string> lines(200);

    while (file.is_open()) {
        for (auto i = 0; i < lines.size(); i++)
            getline(file, lines[i]);
        file.close();
    }

    return lines[randomNumberX(0, 200)];
}
