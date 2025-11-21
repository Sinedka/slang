#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

int main() {
    ifstream fin("utf8.txt", ios::binary); // открываем в бинарном режиме
    if (!fin) {
        cerr << "Не удалось открыть файл\n";
        return 1;
    }

    // Используем stringstream для чтения всего файла в одну строку
    stringstream buffer;
    buffer << fin.rdbuf();  

    string utf8_text = buffer.str(); // здесь вся строка UTF-8
    fin.close();

    // Выводим в консоль
    cout << utf8_text << endl;
}

