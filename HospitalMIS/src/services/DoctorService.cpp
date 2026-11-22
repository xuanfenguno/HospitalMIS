#include "services/DoctorService.h"
#include "utils/Utils.h"
#include <iostream>
#include <iomanip>

namespace hos {

void DoctorService::listDepartments() {
    utils::clearScreen();
    std::cout << "========== 科室列表 ==========\n";
    auto depts = store_.loadAllDepartments();
    if (depts.empty()) {
        std::cout << "暂无科室。\n"; utils::pause(); return;
    }
    utils::printLine();
    std::cout << std::left << std::setw(8) << "编号" << std::setw(12) << "名称" << "简介\n";
    utils::printLine();
    for (const auto& d : depts) {
        std::cout << std::left << std::setw(8) << d.id << std::setw(12) << d.name << d.description << "\n";
    }
    utils::printLine();
    utils::pause();
}

void DoctorService::listDoctors() {
    utils::clearScreen();
    std::cout << "========== 医生列表 ==========\n";
    auto docs = store_.loadAllDoctors();
    if (docs.empty()) {
        std::cout << "暂无医生。\n"; utils::pause(); return;
    }
    utils::printLine();
    std::cout << std::left
              << std::setw(8)  << "编号"
              << std::setw(10) << "姓名"
              << std::setw(8)  << "科室"
              << std::setw(14) << "职称"
              << std::setw(10) << "挂号费"
              << std::setw(14) << "电话"
              << "\n";
    utils::printLine();
    for (const auto& d : docs) {
        Department* dept = store_.findDepartment(d.departmentId);
        std::string deptName = dept ? dept->name : d.departmentId;
        std::cout << std::left
                  << std::setw(8)  << d.id
                  << std::setw(10) << d.name
                  << std::setw(8)  << deptName
                  << std::setw(14) << d.title
                  << std::setw(10) << utils::formatMoney(d.registrationFee)
                  << std::setw(14) << d.phone
                  << "\n";
    }
    utils::printLine();
    std::cout << "共 " << docs.size() << " 位医生。\n";
    utils::pause();
}

void DoctorService::listDoctorsByDepartment(const std::string& deptId) {
    auto docs = store_.loadAllDoctors();
    int idx = 0;
    std::cout << "\n该科室医生:\n";
    for (const auto& d : docs) {
        if (d.departmentId == deptId) {
            std::cout << ++idx << ". " << d.id << " " << d.name
                      << " " << d.title
                      << " 挂号费:" << utils::formatMoney(d.registrationFee) << "\n";
        }
    }
    if (idx == 0) {
        std::cout << "  该科室暂无医生。\n";
    }
}

Department* DoctorService::selectDepartment() {
    auto depts = store_.loadAllDepartments();
    if (depts.empty()) {
        std::cout << "暂无科室。\n"; return nullptr;
    }
    std::cout << "\n--- 科室列表 ---\n";
    for (size_t i = 0; i < depts.size(); ++i) {
        std::cout << i + 1 << ". " << depts[i].id << " " << depts[i].name
                  << " - " << depts[i].description << "\n";
    }
    int idx = utils::readInt("请选择科室序号: ", 1, (int)depts.size());
    return store_.findDepartment(depts[idx - 1].id);
}

Doctor* DoctorService::selectDoctor(const std::string& deptId) {
    auto docs = store_.loadAllDoctors();
    std::vector<Doctor*> inDept;
    for (auto& d : docs) {
        if (d.departmentId == deptId) inDept.push_back(store_.findDoctor(d.id));
    }
    if (inDept.empty()) {
        std::cout << "该科室暂无医生。\n";
        return nullptr;
    }
    std::cout << "\n--- 医生列表 ---\n";
    for (size_t i = 0; i < inDept.size(); ++i) {
        std::cout << i + 1 << ". " << inDept[i]->id << " " << inDept[i]->name
                  << " " << inDept[i]->title
                  << " 挂号费:" << utils::formatMoney(inDept[i]->registrationFee) << "\n";
    }
    int idx = utils::readInt("请选择医生序号: ", 1, (int)inDept.size());
    return inDept[idx - 1];
}

void DoctorService::addDoctor() {
    utils::clearScreen();
    std::cout << "========== 录入医生 ==========\n";
    Doctor d;
    d.id = store_.nextDoctorId();
    d.name = utils::readLine("姓名: ");
    if (d.name.empty()) { std::cout << "姓名不能为空。\n"; utils::pause(); return; }

    // 选择科室
    Department* dept = selectDepartment();
    if (!dept) { std::cout << "取消录入。\n"; utils::pause(); return; }
    d.departmentId = dept->id;

    d.title = utils::readLine("职称[主任医师/副主任医师/主治医师/住院医师]: ");
    d.registrationFee = utils::readDouble("挂号费: ");
    d.phone = utils::readLine("联系电话: ");

    store_.saveDoctor(d);
    std::cout << "\n录入成功！医生编号: " << d.id << "\n";
    utils::pause();
}

} // namespace hos
