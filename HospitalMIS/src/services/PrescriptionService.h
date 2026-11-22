#ifndef PRESCRIPTION_SERVICE_H
#define PRESCRIPTION_SERVICE_H

#include "data/DataStore.h"
#include "services/DrugService.h"

namespace hos {

// 处方服务（医生工作站核心）
class PrescriptionService {
public:
    explicit PrescriptionService(DataStore& store,
                                 DrugService& gs)
        : store_(store), gs_(gs) {}

    // 医生开处方：选择挂号 -> 录入诊断 -> 添加药品明细 -> 生成处方
    std::string createPrescription();

    // 列出处方
    void listPrescriptions();

    // 查看处方明细
    void viewPrescription();

private:
    DataStore&    store_;
    DrugService&  gs_;
};

} // namespace hos
#endif
