#include "ui/ConsoleUI.h"
#include "utils/Utils.h"
#include <iostream>
#include <iomanip>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

namespace hos {

ConsoleUI::ConsoleUI(const std::string& dataDir)
    : store_(dataDir)
    , patientSvc_(store_)
    , doctorSvc_(store_)
    , drugSvc_(store_)
    , regSvc_(store_, patientSvc_, doctorSvc_)
    , rxSvc_(store_, drugSvc_)
    , billSvc_(store_, drugSvc_)
{
#ifdef _WIN32
    // 设置控制台 UTF-8，解决中文乱码
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    store_.init();
}

ConsoleUI::~ConsoleUI() = default;

void ConsoleUI::run() {
    while (true) {
        showMainMenu();
        int choice = utils::readInt("\n请选择功能 [0-6]: ", 0, 6);
        switch (choice) {
            case 0:
                std::cout << "\n感谢使用医院收费管理系统，再见！\n";
                return;
            case 1: showRegistrationMenu(); break;
            case 2: showDoctorMenu();       break;
            case 3: showBillingMenu();      break;
            case 4: showPharmacyMenu();     break;
            case 5: showDataMenu();         break;
            case 6: showQueryMenu();        break;
        }
    }
}

void ConsoleUI::showMainMenu() {
    utils::clearScreen();
    std::cout << "\n";
    utils::printLine('=');
    std::cout << "    医 院 收 费 管 理 系 统 (HIS-MVP)\n";
    std::cout << "          Hospital Management System\n";
    utils::printLine('=');
    std::cout << "  【1】门诊挂号      患者建档、挂号、退号\n";
    std::cout << "  【2】医生工作站    开处方、查看处方\n";
    std::cout << "  【3】收费结算      缴费、退费、票据\n";
    std::cout << "  【4】药房管理      药品库存、入库出库\n";
    std::cout << "  【5】基础数据      科室、医生、药品维护\n";
    std::cout << "  【6】查询统计      挂号/处方/收费查询\n";
    std::cout << "  【0】退出系统\n";
    utils::printLine('=');
}

// ---------------- 门诊挂号子菜单 ----------------
void ConsoleUI::showRegistrationMenu() {
    while (true) {
        utils::clearScreen();
        std::cout << "\n========== 门诊挂号 ==========\n";
        std::cout << "  1. 新增挂号\n";
        std::cout << "  2. 查看今日挂号\n";
        std::cout << "  3. 查看所有挂号\n";
        std::cout << "  4. 退号\n";
        std::cout << "  0. 返回主菜单\n";
        int c = utils::readInt("请选择: ", 0, 4);
        switch (c) {
            case 0: return;
            case 1: regSvc_.registerPatient();         break;
            case 2: regSvc_.listTodayRegistrations();   break;
            case 3: regSvc_.listRegistrations();        break;
            case 4: regSvc_.cancelRegistration();       break;
        }
    }
}

// ---------------- 医生工作站子菜单 ----------------
void ConsoleUI::showDoctorMenu() {
    while (true) {
        utils::clearScreen();
        std::cout << "\n========== 医生工作站 ==========\n";
        std::cout << "  1. 开处方（看诊）\n";
        std::cout << "  2. 查看处方列表\n";
        std::cout << "  3. 查看处方明细\n";
        std::cout << "  0. 返回主菜单\n";
        int c = utils::readInt("请选择: ", 0, 3);
        switch (c) {
            case 0: return;
            case 1: rxSvc_.createPrescription();  break;
            case 2: rxSvc_.listPrescriptions();   break;
            case 3: rxSvc_.viewPrescription();    break;
        }
    }
}

// ---------------- 收费结算子菜单 ----------------
void ConsoleUI::showBillingMenu() {
    while (true) {
        utils::clearScreen();
        std::cout << "\n========== 收费结算 ==========\n";
        std::cout << "  1. 收费（缴费+发药）\n";
        std::cout << "  2. 查看收费记录\n";
        std::cout << "  3. 退费\n";
        std::cout << "  0. 返回主菜单\n";
        int c = utils::readInt("请选择: ", 0, 3);
        switch (c) {
            case 0: return;
            case 1: billSvc_.collect();       break;
            case 2: billSvc_.listBillings();   break;
            case 3: billSvc_.refund();         break;
        }
    }
}

// ---------------- 药房管理子菜单 ----------------
void ConsoleUI::showPharmacyMenu() {
    while (true) {
        utils::clearScreen();
        std::cout << "\n========== 药房管理 ==========\n";
        std::cout << "  1. 药品库存查询\n";
        std::cout << "  2. 入库/出库\n";
        std::cout << "  3. 录入新药品\n";
        std::cout << "  0. 返回主菜单\n";
        int c = utils::readInt("请选择: ", 0, 3);
        switch (c) {
            case 0: return;
            case 1: drugSvc_.listDrugs();     break;
            case 2: drugSvc_.adjustStock();    break;
            case 3: drugSvc_.addDrug();        break;
        }
    }
}

// ---------------- 基础数据维护子菜单 ----------------
void ConsoleUI::showDataMenu() {
    while (true) {
        utils::clearScreen();
        std::cout << "\n========== 基础数据维护 ==========\n";
        std::cout << "  1. 患者建档\n";
        std::cout << "  2. 患者列表\n";
        std::cout << "  3. 科室列表\n";
        std::cout << "  4. 医生列表\n";
        std::cout << "  5. 录入医生\n";
        std::cout << "  0. 返回主菜单\n";
        int c = utils::readInt("请选择: ", 0, 5);
        switch (c) {
            case 0: return;
            case 1: patientSvc_.createPatient();  break;
            case 2: patientSvc_.listPatients();    break;
            case 3: doctorSvc_.listDepartments();  break;
            case 4: doctorSvc_.listDoctors();       break;
            case 5: doctorSvc_.addDoctor();         break;
        }
    }
}

// ---------------- 查询统计子菜单 ----------------
void ConsoleUI::showQueryMenu() {
    while (true) {
        utils::clearScreen();
        std::cout << "\n========== 查询统计 ==========\n";
        std::cout << "  1. 挂号记录查询\n";
        std::cout << "  2. 处方记录查询\n";
        std::cout << "  3. 收费记录查询\n";
        std::cout << "  4. 今日挂号统计\n";
        std::cout << "  0. 返回主菜单\n";
        int c = utils::readInt("请选择: ", 0, 4);
        switch (c) {
            case 0: return;
            case 1: regSvc_.listRegistrations();         break;
            case 2: rxSvc_.listPrescriptions();           break;
            case 3: billSvc_.listBillings();              break;
            case 4: regSvc_.listTodayRegistrations();     break;
        }
    }
}

void ConsoleUI::showAbout() {
    utils::clearScreen();
    std::cout << "\n";
    utils::printLine('=');
    std::cout << "  医院收费管理系统 HIS-MVP v1.0\n";
    utils::printLine('=');
    std::cout << "  开发语言: C++17\n";
    std::cout << "  开发工具: Visual Studio 2022\n";
    std::cout << "  架构模式: 控制台 C/S 单机版\n";
    std::cout << "  数据存储: 文件持久化（可扩展 SQLite/SQL Server）\n";
    utils::printLine();
    std::cout << "  核心流程: 患者建档 -> 挂号 -> 开处方 -> 收费 -> 发药(扣库存)\n";
    utils::printLine('=');
    utils::pause();
}

} // namespace hos
