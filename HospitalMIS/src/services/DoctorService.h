#ifndef DOCTOR_SERVICE_H
#define DOCTOR_SERVICE_H

#include "data/DataStore.h"

namespace hos {

// 医生与科室服务
class DoctorService {
public:
    explicit DoctorService(DataStore& store) : store_(store) {}

    void listDepartments();
    void listDoctors();
    void listDoctorsByDepartment(const std::string& deptId);

    // 选择科室，返回科室指针
    Department* selectDepartment();

    // 在指定科室下选择医生
    Doctor* selectDoctor(const std::string& deptId);

    // 添加医生
    void addDoctor();

private:
    DataStore& store_;
};

} // namespace hos
#endif
