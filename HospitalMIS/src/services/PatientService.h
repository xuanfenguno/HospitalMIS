#ifndef PATIENT_SERVICE_H
#define PATIENT_SERVICE_H

#include <string>
#include <vector>
#include "data/DataStore.h"

namespace hos {

// 患者服务：建档、查询
class PatientService {
public:
    explicit PatientService(DataStore& store) : store_(store) {}

    // 新建患者建档，返回患者ID
    std::string createPatient();

    // 列出所有患者
    void listPatients();

    // 按ID或姓名查找患者，返回选中患者指针（nullptr 表示未找到）
    Patient* selectPatient();

private:
    DataStore& store_;
};

} // namespace hos
#endif
