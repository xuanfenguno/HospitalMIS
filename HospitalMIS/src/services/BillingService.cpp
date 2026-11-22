#include "services/BillingService.h"
#include "utils/Utils.h"
#include <iostream>
#include <iomanip>

namespace hos {

void BillingService::collect() {
    utils::clearScreen();
    std::cout << "========== 收费结算 ==========\n";

    // 1. 输入处方号
    std::string rxId = utils::readLine("请输入处方号: ");
    Prescription* rx = store_.findPrescription(rxId);
    if (!rx) {
        std::cout << "未找到该处方。\n"; utils::pause(); return;
    }
    if (rx->status != static_cast<int>(RxStatus::Pending)) {
        std::cout << "该处方状态非\"待缴费\"，无法收费。\n"; utils::pause(); return;
    }

    Registration* reg = store_.findRegistration(rx->registrationId);
    if (!reg) {
        std::cout << "关联挂号记录丢失。\n"; utils::pause(); return;
    }

    // 2. 显示费用明细
    utils::printLine('=');
    std::cout << "              费 用 清 单\n";
    utils::printLine('=');
    std::cout << "  处方号:   " << rx->id << "\n";
    std::cout << "  挂号号:   " << rx->registrationId << "\n";
    std::cout << "  患  者:   " << rx->patientName << "\n";
    std::cout << "  医  生:   " << rx->doctorName << "\n";
    utils::printLine();
    std::cout << "  1. 挂号费:        " << utils::formatMoney(reg->fee) << " 元\n";
    std::cout << "  2. 药品费明细:\n";
    for (const auto& it : rx->items) {
        std::cout << "     " << it.drugName << " x" << it.quantity
                  << " = " << utils::formatMoney(it.subtotal) << " 元\n";
    }
    std::cout << "     药品小计:      " << utils::formatMoney(rx->totalAmount) << " 元\n";
    utils::printLine();
    double total = reg->fee + rx->totalAmount;
    std::cout << "  应收合计:        " << utils::formatMoney(total) << " 元\n";
    utils::printLine('=');

    // 3. 收款
    double paid = utils::readDouble("\n实收金额: ");
    if (paid < total) {
        std::cout << "实收金额不足！还需 " << utils::formatMoney(total - paid) << " 元。\n";
        utils::pause();
        return;
    }
    double change = paid - total;
    std::cout << "找零: " << utils::formatMoney(change) << " 元\n";

    // 4. 生成收费记录
    BillingRecord bill;
    bill.id = store_.nextBillingId();
    bill.registrationId = reg->id;
    bill.prescriptionId = rx->id;
    bill.patientId = rx->patientId;
    bill.patientName = rx->patientName;
    bill.registrationFee = reg->fee;
    bill.drugFee = rx->totalAmount;
    bill.totalAmount = total;
    bill.status = static_cast<int>(BillStatus::Paid);
    bill.payTime = utils::now();
    bill.operatorId = "admin";

    store_.saveBilling(bill);

    // 5. 更新处方状态为已缴费
    rx->status = static_cast<int>(RxStatus::Paid);
    store_.savePrescription(*rx);

    // 6. 发药：扣减库存
    std::cout << "\n正在发药，扣减库存...\n";
    if (gs_.dispense(rx->items)) {
        rx->status = static_cast<int>(RxStatus::Dispensed);
        store_.savePrescription(*rx);
        std::cout << "发药成功！库存已扣减。\n";
    } else {
        std::cout << "警告：库存不足，发药失败！请补货后手动发药。\n";
    }

    // 7. 打印票据
    utils::printLine('=');
    std::cout << "            收 费 票 据\n";
    utils::printLine('=');
    std::cout << "  票据号:   " << bill.id << "\n";
    std::cout << "  处方号:   " << bill.prescriptionId << "\n";
    std::cout << "  挂号号:   " << bill.registrationId << "\n";
    std::cout << "  患  者:   " << bill.patientName << "\n";
    std::cout << "  挂号费:   " << utils::formatMoney(bill.registrationFee) << " 元\n";
    std::cout << "  药品费:   " << utils::formatMoney(bill.drugFee) << " 元\n";
    std::cout << "  合  计:   " << utils::formatMoney(bill.totalAmount) << " 元\n";
    std::cout << "  实  收:   " << utils::formatMoney(paid) << " 元\n";
    std::cout << "  找  零:   " << utils::formatMoney(change) << " 元\n";
    std::cout << "  时  间:   " << bill.payTime << "\n";
    std::cout << "  收费员:   " << bill.operatorId << "\n";
    utils::printLine('=');
    std::cout << "请患者凭票据到药房取药。\n";
    utils::pause();
}

void BillingService::listBillings() {
    utils::clearScreen();
    std::cout << "========== 收费记录 ==========\n";
    auto bills = store_.loadAllBillings();
    if (bills.empty()) {
        std::cout << "暂无收费记录。\n"; utils::pause(); return;
    }
    utils::printLine();
    std::cout << std::left
              << std::setw(8)  << "票据号"
              << std::setw(8)  << "患者"
              << std::setw(10) << "挂号费"
              << std::setw(10) << "药费"
              << std::setw(10) << "合计"
              << std::setw(10) << "状态"
              << std::setw(22) << "时间"
              << "\n";
    utils::printLine();
    double totalRevenue = 0.0;
    for (const auto& b : bills) {
        const char* statusStr[] = {"已收费", "已退费"};
        std::cout << std::left
                  << std::setw(8)  << b.id
                  << std::setw(8)  << b.patientName
                  << std::setw(10) << utils::formatMoney(b.registrationFee)
                  << std::setw(10) << utils::formatMoney(b.drugFee)
                  << std::setw(10) << utils::formatMoney(b.totalAmount)
                  << std::setw(10) << statusStr[b.status]
                  << std::setw(22) << b.payTime
                  << "\n";
        if (b.status == static_cast<int>(BillStatus::Paid))
            totalRevenue += b.totalAmount;
    }
    utils::printLine();
    std::cout << "共 " << bills.size() << " 笔记录，总收入: "
              << utils::formatMoney(totalRevenue) << " 元\n";
    utils::pause();
}

void BillingService::refund() {
    utils::clearScreen();
    std::cout << "========== 退费处理 ==========\n";
    std::string id = utils::readLine("请输入票据号: ");
    BillingRecord* bill = store_.findBilling(id);
    if (!bill) {
        std::cout << "未找到该收费记录。\n"; utils::pause(); return;
    }
    if (bill->status == static_cast<int>(BillStatus::Refunded)) {
        std::cout << "该记录已退费。\n"; utils::pause(); return;
    }

    std::cout << "票据号: " << bill->id << "  患者: " << bill->patientName
              << "  金额: " << utils::formatMoney(bill->totalAmount) << " 元\n";
    std::cout << "确认退费？\n1.确认 2.取消\n";
    int c = utils::readInt("请选择: ", 1, 2);
    if (c != 1) {
        std::cout << "已取消。\n"; utils::pause(); return;
    }

    // 退费：标记票据为已退费
    bill->status = static_cast<int>(BillStatus::Refunded);
    store_.saveBilling(*bill);

    // 处方标记为已退费
    Prescription* rx = store_.findPrescription(bill->prescriptionId);
    if (rx) {
        rx->status = static_cast<int>(RxStatus::Refunded);
        store_.savePrescription(*rx);

        // 若已发药，需归还库存
        std::cout << "该处方药品是否已发出？\n1.已发出(归还库存) 2.未发出\n";
        int sent = utils::readInt("请选择: ", 1, 2);
        if (sent == 1) {
            for (const auto& it : rx->items) {
                Drug* d = store_.findDrug(it.drugId);
                if (d) {
                    d->stock += it.quantity;
                    store_.saveDrug(*d);
                }
            }
            std::cout << "库存已归还。\n";
        }
    }

    std::cout << "退费成功，退款 " << utils::formatMoney(bill->totalAmount) << " 元。\n";
    utils::pause();
}

} // namespace hos
