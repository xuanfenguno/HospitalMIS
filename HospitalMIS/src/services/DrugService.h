#ifndef DRUG_SERVICE_H
#define DRUG_SERVICE_H

#include <vector>
#include "data/DataStore.h"
#include "models/Models.h"

namespace hos {

// 药品与库存服务
class DrugService {
public:
    explicit DrugService(DataStore& store) : store_(store) {}

    void listDrugs();

    // 选择药品（列出后让用户选），返回指针
    Drug* selectDrug();

    // 录入新药品
    void addDrug();

    // 调整库存（入库/出库）
    void adjustStock();

    // 检查库存是否充足（按处方明细）
    bool checkStock(const std::vector<PrescriptionItem>& items);

    // 根据处方明细扣减库存（发药）
    bool dispense(const std::vector<PrescriptionItem>& items);

private:
    DataStore& store_;
};

} // namespace hos
#endif
