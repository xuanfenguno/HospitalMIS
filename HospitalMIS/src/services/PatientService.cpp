#include "services/PatientService.h"
#include "utils/Utils.h"
#include <iostream>
#include <iomanip>

namespace hos {

std::string PatientService::createPatient() {
    utils::clearScreen();
    std::cout << "========== 患者建档 ==========\n";

    Patient p;
    p.id = store_.nextPatientId();
    p.name = utils::readLine("姓名: ");
    p.gender = utils::readGender("性别 [M男/F女]: ");
    p.age = utils::readInt("年龄: ", 0, 150);
    p.idCard = utils::readLine("身份证号: ");
    p.phone = utils::readLine("联系电话: ");
    p.address = utils::readLine("住址: ");
    p.createTime = utils::now();

    if (p.name.empty()) {
        std::cout << "姓名不能为空，建档取消。\n";
        utils::pause();
        return "";
    }

    store_.savePatient(p);
    std::cout << "\n建档成功！患者编号: " << p.id << "\n";
    std::cout << "请记住此编号，后续挂号时使用。\n";
    utils::pause();
    return p.id;
}

void PatientService::listPatients() {
    utils::clearScreen();
    std::cout << "========== 患者列表 ==========\n";
    auto patients = store_.loadAllPatients();
    if (patients.empty()) {
        std::cout << "暂无患者记录。\n";
        utils::pause();
        return;
    }
    utils::printLine();
    std::cout << std::left
              << std::setw(8)  << "编号"
              << std::setw(10) << "姓名"
              << std::setw(6)  << "性别"
              << std::setw(6)  << "年龄"
              << std::setw(20) << "身份证号"
              << std::setw(14) << "电话"
              << "\n";
    utils::printLine();
    for (const auto& p : patients) {
        std::cout << std::left
                  << std::setw(8)  << p.id
                  << std::setw(10) << p.name
                  << std::setw(6)  << (p.gender == 'M' ? "男" : "女")
                  << std::setw(6)  << p.age
                  << std::setw(20) << p.idCard
                  << std::setw(14) << p.phone
                  << "\n";
    }
    utils::printLine();
    std::cout << "共 " << patients.size() << " 位患者。\n";
    utils::pause();
}

Patient* PatientService::selectPatient() {
    auto patients = store_.loadAllPatients();
    if (patients.empty()) {
        std::cout << "暂无患者，请先建档。\n";
        utils::pause();
        return nullptr;
    }
    std::cout << "\n--- 选择患者 ---\n";
    std::cout << "1. 按编号查找\n";
    std::cout << "2. 按姓名查找\n";
    int choice = utils::readInt("请选择: ", 1, 2);

    if (choice == 1) {
        std::string id = utils::readLine("请输入患者编号: ");
        Patient* p = store_.findPatient(id);
        if (!p) {
            std::cout << "未找到编号 " << id << " 的患者。\n";
            utils::pause();
        }
        return p;
    } else {
        std::string name = utils::readLine("请输入患者姓名: ");
        std::vector<Patient*> matches;
        for (auto& p : patients) {
            if (p.name == name) matches.push_back(store_.findPatient(p.id));
        }
        if (matches.empty()) {
            std::cout << "未找到姓名为 " << name << " 的患者。\n";
            utils::pause();
            return nullptr;
        }
        if (matches.size() == 1) return matches[0];
        std::cout << "找到多位同名患者，请选择:\n";
        for (size_t i = 0; i < matches.size(); ++i) {
            std::cout << i + 1 << ". " << matches[i]->id << " "
                      << matches[i]->name << " "
                      << matches[i]->age << "岁 "
                      << matches[i]->phone << "\n";
        }
        int idx = utils::readInt("请选择: ", 1, (int)matches.size());
        return matches[idx - 1];
    }
}

} // namespace hos
