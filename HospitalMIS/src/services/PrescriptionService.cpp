#include "services/PrescriptionService.h"
#include "utils/Utils.h"
#include <iostream>
#include <iomanip>

namespace hos {

std::string PrescriptionService::createPrescription() {
    utils::clearScreen();
    std::cout << "========== 医生工作站 - 开处方 ==========\n";

    // 1. 选择挂号记录（必须是待就诊状态）
    std::string regId = utils::readLine("请输入挂号号: ");
    Registration* reg = store_.findRegistration(regId);
    if (!reg) {
        std::cout << "未找到该挂号记录。\n"; utils::pause(); return "";
    }
    if (reg->status != static_cast<int>(RegStatus::Registered)) {
        std::cout << "该挂号状态不允许开处方（已就诊/已退号）。\n"; utils::pause(); return "";
    }

    Patient* patient = store_.findPatient(reg->patientId);
    Doctor*  doctor  = store_.findDoctor(reg->doctorId);
    if (!patient || !doctor) {
        std::cout << "患者或医生信息缺失。\n"; utils::pause(); return "";
    }

    std::cout << "\n患者: " << patient->name << "  " << patient->age << "岁  "
              << (patient->gender == 'M' ? "男" : "女") << "\n";
    std::cout << "医生: " << doctor->name << " " << doctor->title << "\n";

    // 2. 录入诊断
    std::string diagnosis = utils::readLine("\n诊断: ");

    // 3. 添加药品明细
    std::vector<PrescriptionItem> items;
    while (true) {
        std::cout << "\n--- 添加药品 ---\n";
        Drug* drug = gs_.selectDrug();
        if (!drug) break;
        int qty = utils::readInt("数量: ", 1, 9999);
        if (drug->stock < qty) {
            std::cout << "库存不足！当前库存: " << drug->stock << "\n";
            std::cout << "1.使用最大库存 2.重新选择 3.跳过\n";
            int c = utils::readInt("请选择: ", 1, 3);
            if (c == 1) qty = drug->stock;
            else if (c == 3) continue;
            else continue;
        }

        PrescriptionItem item;
        item.drugId = drug->id;
        item.drugName = drug->name;
        item.quantity = qty;
        item.unitPrice = drug->price;
        item.subtotal = qty * drug->price;
        items.push_back(item);

        std::cout << "已添加: " << drug->name << " x" << qty
                  << " = " << utils::formatMoney(item.subtotal) << " 元\n";

        std::cout << "继续添加？\n1.继续 2.完成\n";
        int c = utils::readInt("请选择: ", 1, 2);
        if (c == 2) break;
    }

    if (items.empty()) {
        std::cout << "处方无药品，已取消。\n"; utils::pause(); return "";
    }

    // 4. 生成处方
    Prescription rx;
    rx.id = store_.nextPrescriptionId();
    rx.registrationId = reg->id;
    rx.patientId = patient->id;
    rx.patientName = patient->name;
    rx.doctorId = doctor->id;
    rx.doctorName = doctor->name;
    rx.diagnosis = diagnosis;
    rx.items = items;
    rx.calcTotal();
    rx.status = static_cast<int>(RxStatus::Pending);
    rx.createTime = utils::now();

    store_.savePrescription(rx);

    // 标记挂号为已就诊
    reg->status = static_cast<int>(RegStatus::Seen);
    store_.saveRegistration(*reg);

    // 打出处方单
    utils::printLine('=');
    std::cout << "              处  方  单\n";
    utils::printLine('=');
    std::cout << "  处方号:   " << rx.id << "\n";
    std::cout << "  挂号号:   " << rx.registrationId << "\n";
    std::cout << "  患  者:   " << rx.patientName << "\n";
    std::cout << "  医  生:   " << rx.doctorName << "\n";
    std::cout << "  诊  断:   " << rx.diagnosis << "\n";
    std::cout << "  时  间:   " << rx.createTime << "\n";
    utils::printLine();
    std::cout << std::left
              << std::setw(16) << "药品"
              << std::setw(8)  << "数量"
              << std::setw(10) << "单价"
              << std::setw(10) << "小计"
              << "\n";
    utils::printLine();
    for (const auto& it : rx.items) {
        std::cout << std::left
                  << std::setw(16) << it.drugName
                  << std::setw(8)  << it.quantity
                  << std::setw(10) << utils::formatMoney(it.unitPrice)
                  << std::setw(10) << utils::formatMoney(it.subtotal)
                  << "\n";
    }
    utils::printLine();
    std::cout << "  药品总额: " << utils::formatMoney(rx.totalAmount) << " 元\n";
    utils::printLine('=');
    std::cout << "请患者凭此处方到收费处缴费。\n";
    utils::pause();
    return rx.id;
}

void PrescriptionService::listPrescriptions() {
    utils::clearScreen();
    std::cout << "========== 处方列表 ==========\n";
    auto rxs = store_.loadAllPrescriptions();
    if (rxs.empty()) {
        std::cout << "暂无处方。\n"; utils::pause(); return;
    }
    utils::printLine();
    std::cout << std::left
              << std::setw(8)  << "处方号"
              << std::setw(8)  << "患者"
              << std::setw(8)  << "医生"
              << std::setw(16) << "诊断"
              << std::setw(10) << "金额"
              << std::setw(10) << "状态"
              << "\n";
    utils::printLine();
    for (const auto& rx : rxs) {
        const char* statusStr[] = {"待缴费", "已缴费", "已发药", "已退费"};
        std::cout << std::left
                  << std::setw(8)  << rx.id
                  << std::setw(8)  << rx.patientName
                  << std::setw(8)  << rx.doctorName
                  << std::setw(16) << rx.diagnosis
                  << std::setw(10) << utils::formatMoney(rx.totalAmount)
                  << std::setw(10) << statusStr[rx.status]
                  << "\n";
    }
    utils::printLine();
    std::cout << "共 " << rxs.size() << " 张处方。\n";
    utils::pause();
}

void PrescriptionService::viewPrescription() {
    utils::clearScreen();
    std::cout << "========== 查看处方明细 ==========\n";
    std::string id = utils::readLine("请输入处方号: ");
    Prescription* rx = store_.findPrescription(id);
    if (!rx) {
        std::cout << "未找到该处方。\n"; utils::pause(); return;
    }
    utils::printLine('=');
    std::cout << "  处方号: " << rx->id << "\n";
    std::cout << "  挂号号: " << rx->registrationId << "\n";
    std::cout << "  患  者: " << rx->patientName << "\n";
    std::cout << "  医  生: " << rx->doctorName << "\n";
    std::cout << "  诊  断: " << rx->diagnosis << "\n";
    std::cout << "  时  间: " << rx->createTime << "\n";
    utils::printLine();
    std::cout << std::left
              << std::setw(16) << "药品"
              << std::setw(8)  << "数量"
              << std::setw(10) << "单价"
              << std::setw(10) << "小计"
              << "\n";
    utils::printLine();
    for (const auto& it : rx->items) {
        std::cout << std::left
                  << std::setw(16) << it.drugName
                  << std::setw(8)  << it.quantity
                  << std::setw(10) << utils::formatMoney(it.unitPrice)
                  << std::setw(10) << utils::formatMoney(it.subtotal)
                  << "\n";
    }
    utils::printLine();
    std::cout << "  药品总额: " << utils::formatMoney(rx->totalAmount) << " 元\n";
    const char* statusStr[] = {"待缴费", "已缴费", "已发药", "已退费"};
    std::cout << "  状  态:   " << statusStr[rx->status] << "\n";
    utils::printLine('=');
    utils::pause();
}

} // namespace hos
