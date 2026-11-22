#ifndef REGISTRATION_SERVICE_H
#define REGISTRATION_SERVICE_H

#include "data/DataStore.h"
#include "services/PatientService.h"
#include "services/DoctorService.h"

namespace hos {

// 挂号服务
class RegistrationService {
public:
    explicit RegistrationService(DataStore& store,
                                 PatientService& ps,
                                 DoctorService& ds)
        : store_(store), ps_(ps), ds_(ds) {}

    // 执行挂号流程，返回挂号号
    std::string registerPatient();

    // 列出挂号记录（可按状态筛选）
    void listRegistrations();

    // 查看今日挂号
    void listTodayRegistrations();

    // 退号
    void cancelRegistration();

private:
    DataStore&       store_;
    PatientService&  ps_;
    DoctorService&   ds_;
};

} // namespace hos
#endif
