#include "services/DrugService.h"
#include "utils/Utils.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

namespace hos {

void DrugService::listDrugs() {
    utils::clearScreen();
    std::cout << "========== 药品库存列表 ==========\n";
    auto drugs = store_.loadAllDrugs();
    if (drugs.empty()) {
        std::cout << "暂无药品。\n"; utils::pause(); return;
    }
    utils::printLine();
    std::cout << std::left
              << std::setw(8)  << "编号"
              << std::setw(16) << "名称"
              << std::setw(14) << "规格"
              << std::setw(6)  << "单位"
              << std::setw(10) << "单价"
              << std::setw(8)  << "库存"
              << std::setw(16) << "厂家"
              << "\n";
    utils::printLine();
    for (const auto& d : drugs) {
        std::cout << std::left
                  << std::setw(8)  << d.id
                  << std::setw(16) << d.name
                  << std::setw(14) << d.specification
                  << std::setw(6)  << d.unit
                  << std::setw(10) << utils::formatMoney(d.price)
                  << std::setw(8)  << d.stock
                  << std::setw(16) << d.manufacturer
                  << "\n";
    }
    utils::printLine();
    std::cout << "共 " << drugs.size() << " 种药品。\n";
    utils::pause();
}

Drug* DrugService::selectDrug() {
    auto drugs = store_.loadAllDrugs();
    if (drugs.empty()) {
        std::cout << "暂无药品。\n";
        return nullptr;
    }
    std::cout << "\n--- 药品列表 ---\n";
    for (size_t i = 0; i < drugs.size(); ++i) {
        std::cout << i + 1 << ". [" << drugs[i].id << "] "
                  << drugs[i].name << " " << drugs[i].specification
                  << " " << utils::formatMoney(drugs[i].price) << "/" << drugs[i].unit
                  << " 库存:" << drugs[i].stock << "\n";
    }
    int idx = utils::readInt("请选择药品序号: ", 1, (int)drugs.size());
    return store_.findDrug(drugs[idx - 1].id);
}

void DrugService::addDrug() {
    utils::clearScreen();
    std::cout << "========== 录入药品 ==========\n";
    Drug d;
    d.id = store_.nextDrugId();
    d.name = utils::readLine("药品名称: ");
    if (d.name.empty()) { std::cout << "名称不能为空。\n"; utils::pause(); return; }
    d.specification = utils::readLine("规格(如0.25g*24粒): ");
    d.unit = utils::readLine("单位(盒/瓶/支): ");
    d.price = utils::readDouble("单价: ");
    d.stock = utils::readInt("初始库存数量: ", 0, 999999);
    d.manufacturer = utils::readLine("生产厂家: ");

    store_.saveDrug(d);
    std::cout << "\n录入成功！药品编号: " << d.id << "\n";
    utils::pause();
}

void DrugService::adjustStock() {
    utils::clearScreen();
    std::cout << "========== 库存调整 ==========\n";
    listDrugs();
    auto drugs = store_.loadAllDrugs();
    if (drugs.empty()) return;

    std::string id = utils::readLine("请输入药品编号: ");
    Drug* d = store_.findDrug(id);
    if (!d) {
        std::cout << "未找到该药品。\n"; utils::pause(); return;
    }
    std::cout << "当前库存: " << d->stock << "\n";
    std::cout << "1. 入库(增加)\n2. 出库(减少)\n";
    int op = utils::readInt("请选择: ", 1, 2);
    int qty = utils::readInt("数量: ", 1, 999999);
    if (op == 1) {
        d->stock += qty;
        std::cout << "入库成功，当前库存: " << d->stock << "\n";
    } else {
        if (qty > d->stock) {
            std::cout << "库存不足！当前仅有 " << d->stock << "。\n";
            utils::pause();
            return;
        }
        d->stock -= qty;
        std::cout << "出库成功，当前库存: " << d->stock << "\n";
    }
    store_.saveDrug(*d);
    utils::pause();
}

bool DrugService::checkStock(const std::vector<PrescriptionItem>& items) {
    for (const auto& it : items) {
        Drug* d = store_.findDrug(it.drugId);
        if (!d) {
            std::cout << "药品 " << it.drugId << " 不存在！\n";
            return false;
        }
        if (d->stock < it.quantity) {
            std::cout << "药品 [" << d->name << "] 库存不足！"
                      << "需 " << it.quantity << " " << d->unit
                      << "，库存仅 " << d->stock << "。\n";
            return false;
        }
    }
    return true;
}

bool DrugService::dispense(const std::vector<PrescriptionItem>& items) {
    // 先检查库存
    if (!checkStock(items)) return false;
    // 再扣减库存
    for (const auto& it : items) {
        Drug* d = store_.findDrug(it.drugId);
        d->stock -= it.quantity;
        store_.saveDrug(*d);
    }
    return true;
}

} // namespace hos
