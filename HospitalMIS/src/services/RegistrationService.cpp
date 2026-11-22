#include "services/RegistrationService.h"
#include "utils/Utils.h"
#include <iostream>
#include <iomanip>

namespace hos {

std::string RegistrationService::registerPatient() {
    utils::clearScreen();
    std::cout << "========== 门诊挂号 ==========\n";

    // 1. 选择患者（已有则选择，没有则建档）
    Patient* patient = ps_.selectPatient();
    if (!patient) {
        std::cout << "是否新建患者建档？\n1.是 2.否\n";
        int c = utils::readInt("请选择: ", 1, 2);
        if (c == 1) {
            std::string pid = ps_.createPatient();
            if (pid.empty()) return "";
            patient = store_.findPatient(pid);
        } else {
            std::cout << "挂号取消。\n"; utils::pause();
            return "";
        }
    }
    if (!patient) return "";

    // 2. 选择科室
    Department* dept = ds_.selectDepartment();
    if (!dept) { std::cout << "挂号取消。\n"; utils::pause(); return ""; }

    // 3. 选择医生
    Doctor* doctor = ds_.selectDoctor(dept->id);
    if (!doctor) { std::cout << "挂号取消。\n"; utils::pause(); return ""; }

    // 4. 生成挂号记录
    Registration r;
    r.id = store_.nextRegistrationId();
    r.patientId = patient->id;
    r.patientName = patient->name;
    r.doctorId = doctor->id;
    r.doctorName = doctor->name;
    r.departmentId = dept->id;
    r.fee = doctor->registrationFee;
    r.status = static_cast<int>(RegStatus::Registered);
    r.registerTime = utils::now();

    store_.saveRegistration(r);

    // 打印挂号单
    utils::printLine('=');
    std::cout << "              挂  号  单\n";
    utils::printLine('=');
    std::cout << "  挂号号:   " << r.id << "\n";
    std::cout << "  患  者:   " << r.patientName << " (" << r.patientId << ")\n";
    std::cout << "  科  室:   " << dept->name << "\n";
    std::cout << "  医  生:   " << r.doctorName << " " << doctor->title << "\n";
    std::cout << "  挂号费:   " << utils::formatMoney(r.fee) << " 元\n";
    std::cout << "  时  间:   " << r.registerTime << "\n";
    std::cout << "  状  态:   待就诊\n";
    utils::printLine('=');
    std::cout << "请凭此挂号单到对应科室就诊。\n";
    utils::pause();
    return r.id;
}

void RegistrationService::listRegistrations() {
    utils::clearScreen();
    std::cout << "========== 挂号记录 ==========\n";
    auto regs = store_.loadAllRegistrations();
    if (regs.empty()) {
        std::cout << "暂无挂号记录。\n"; utils::pause(); return;
    }
    utils::printLine();
    std::cout << std::left
              << std::setw(8)  << "挂号号"
              << std::setw(8)  << "患者"
              << std::setw(8)  << "医生"
              << std::setw(10) << "挂号费"
              << std::setw(10) << "状态"
              << std::setw(22) << "时间"
              << "\n";
    utils::printLine();
    for (const auto& r : regs) {
        const char* statusStr[] = {"待就诊", "已就诊", "已退号"};
        std::cout << std::left
                  << std::setw(8)  << r.id
                  << std::setw(8)  << r.patientName
                  << std::setw(8)  << r.doctorName
                  << std::setw(10) << utils::formatMoney(r.fee)
                  << std::setw(10) << statusStr[r.status]
                  << std::setw(22) << r.registerTime
                  << "\n";
    }
    utils::printLine();
    std::cout << "共 " << regs.size() << " 条挂号记录。\n";
    utils::pause();
}

void RegistrationService::listTodayRegistrations() {
    utils::clearScreen();
    std::cout << "========== 今日挂号 ==========\n";
    std::string today = utils::today();
    auto regs = store_.loadAllRegistrations();
    int count = 0;
    utils::printLine();
    std::cout << std::left
              << std::setw(8)  << "挂号号"
              << std::setw(8)  << "患者"
              << std::setw(8)  << "医生"
              << std::setw(10) << "挂号费"
              << std::setw(10) << "状态"
              << "\n";
    utils::printLine();
    for (const auto& r : regs) {
        if (r.registerTime.compare(0, 10, today) == 0) {
            const char* statusStr[] = {"待就诊", "已就诊", "已退号"};
            std::cout << std::left
                      << std::setw(8)  << r.id
                      << std::setw(8)  << r.patientName
                      << std::setw(8)  << r.doctorName
                      << std::setw(10) << utils::formatMoney(r.fee)
                      << std::setw(10) << statusStr[r.status]
                      << "\n";
            count++;
        }
    }
    utils::printLine();
    std::cout << "今日共 " << count << " 人挂号。\n";
    utils::pause();
}

void RegistrationService::cancelRegistration() {
    utils::clearScreen();
    std::cout << "========== 退号处理 ==========\n";
    std::string id = utils::readLine("请输入挂号号: ");
    Registration* r = store_.findRegistration(id);
    if (!r) {
        std::cout << "未找到该挂号记录。\n"; utils::pause(); return;
    }
    if (r->status == static_cast<int>(RegStatus::Seen)) {
        std::cout << "该挂号已就诊，无法退号。\n"; utils::pause(); return;
    }
    if (r->status == static_cast<int>(RegStatus::Cancelled)) {
        std::cout << "该挂号已退号。\n"; utils::pause(); return;
    }
    std::cout << "挂号号: " << r->id << "  患者: " << r->patientName
              << "  医生: " << r->doctorName << "\n";
    std::cout << "确认退号？\n1.确认 2.取消\n";
    int c = utils::readInt("请选择: ", 1, 2);
    if (c == 1) {
        r->status = static_cast<int>(RegStatus::Cancelled);
        store_.saveRegistration(*r);
        std::cout << "退号成功。\n";
    } else {
        std::cout << "已取消操作。\n";
    }
    utils::pause();
}

} // namespace hos
