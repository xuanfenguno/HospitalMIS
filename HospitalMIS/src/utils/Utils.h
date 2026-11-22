#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace hos {

// 工具函数集合：输入处理、时间、格式化
namespace utils {

// 获取当前时间，格式 YYYY-MM-DD HH:MM:SS
std::string now();

// 获取当前日期，格式 YYYY-MM-DD
std::string today();

// 控制台清屏
void clearScreen();

// 暂停等待用户按回车
void pause();

// 打印分隔线
void printLine(char ch = '-', int len = 60);

// 读取一行输入（带提示）
std::string readLine(const std::string& prompt);

// 读取整数（带提示与范围校验）
int readInt(const std::string& prompt, int minVal = 0, int maxVal = 999999);

// 读取浮点数
double readDouble(const std::string& prompt);

// 读取字符（M/F）
char readGender(const std::string& prompt);

// 金额格式化，保留2位小数
std::string formatMoney(double v);

// Windows 控制台 UTF-8 设置（非 Windows 平台为空操作）
void setupConsoleUtf8();

} // namespace utils

} // namespace hos

#endif // UTILS_H
