// ============================================================================
// 医院收费管理系统 (HIS-MVP) 主程序入口
// 开发环境: Visual Studio 2022, C++17, Windows 控制台
// 功能: 患者建档 -> 挂号 -> 开处方 -> 收费 -> 发药(扣库存) 完整流程
// ============================================================================
#include "ui/ConsoleUI.h"
#include <iostream>
#include <string>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    // Windows 控制台 UTF-8 编码设置，确保中文正常显示
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    try {
        // 数据目录：与可执行文件同级的 data 文件夹
        // 也可改为绝对路径如 "D:/HospitalData"
        std::string dataDir = "data";

        std::cout << "正在初始化医院收费管理系统...\n";
        hos::ConsoleUI app(dataDir);
        std::cout << "初始化完成！\n\n";
        std::cout << "按回车键进入系统...";
        std::cin.get();

        app.run();
    } catch (const std::exception& e) {
        std::cerr << "系统发生错误: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "系统发生未知错误。\n";
        return 1;
    }
    return 0;
}
