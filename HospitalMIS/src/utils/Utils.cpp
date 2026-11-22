#include "utils/Utils.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>
#include <ctime>
#include <cstdlib>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

namespace hos { namespace utils {

std::string now() {
    std::time_t t = std::time(nullptr);
    std::tm* lt = std::localtime(&t);
    std::ostringstream os;
    os << std::put_time(lt, "%Y-%m-%d %H:%M:%S");
    return os.str();
}

std::string today() {
    std::time_t t = std::time(nullptr);
    std::tm* lt = std::localtime(&t);
    std::ostringstream os;
    os << std::put_time(lt, "%Y-%m-%d");
    return os.str();
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause() {
    std::cout << "\n按回车键继续...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void printLine(char ch, int len) {
    std::cout << std::string(len, ch) << "\n";
}

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    // 去除首尾空格
    size_t a = s.find_first_not_of(" \t");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t");
    return s.substr(a, b - a + 1);
}

int readInt(const std::string& prompt, int minVal, int maxVal) {
    while (true) {
        std::cout << prompt;
        std::string s;
        std::getline(std::cin, s);
        try {
            int v = std::stoi(s);
            if (v < minVal || v > maxVal) {
                std::cout << "  输入超出范围 [" << minVal << "~" << maxVal << "]，请重输。\n";
                continue;
            }
            return v;
        } catch (...) {
            std::cout << "  请输入有效整数。\n";
        }
    }
}

double readDouble(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string s;
        std::getline(std::cin, s);
        try {
            return std::stod(s);
        } catch (...) {
            std::cout << "  请输入有效数字。\n";
        }
    }
}

char readGender(const std::string& prompt) {
    while (true) {
        std::string s = readLine(prompt);
        if (s.empty()) continue;
        char c = toupper(s[0]);
        if (c == 'M' || c == 'F' || c == '男' || c == '女') {
            if (c == '男') c = 'M';
            if (c == '女') c = 'F';
            return c;
        }
        std::cout << "  请输入 M(男) 或 F(女)。\n";
    }
}

std::string formatMoney(double v) {
    std::ostringstream os;
    os << std::fixed << std::setprecision(2) << v;
    return os.str();
}

// Windows 控制台 UTF-8 设置
#ifdef _WIN32
void setupConsoleUtf8() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}
#else
void setupConsoleUtf8() {}
#endif

}} // namespace hos::utils
