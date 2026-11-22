#ifndef BILLING_SERVICE_H
#define BILLING_SERVICE_H

#include "data/DataStore.h"
#include "services/DrugService.h"

namespace hos {

// 收费结算服务
class BillingService {
public:
    explicit BillingService(DataStore& store,
                            DrugService& gs)
        : store_(store), gs_(gs) {}

    // 收费：选择待缴费处方 -> 计算费用 -> 收费 -> 标记已缴费 -> 发药扣库存
    void collect();

    // 列出收费记录
    void listBillings();

    // 退费
    void refund();

private:
    DataStore&   store_;
    DrugService& gs_;
};

} // namespace hos
#endif
