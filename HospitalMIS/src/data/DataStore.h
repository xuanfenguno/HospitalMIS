#ifndef DATA_STORE_H
#define DATA_STORE_H

#include <string>
#include <vector>
#include "models/Models.h"

// ============================================================================
// 数据存储层 DataStore
// 基于"管道分隔文本文件"的轻量级持久化实现，不依赖任何第三方库，可直接编译。
// 设计目的：让 MVP 跑通"建档-挂号-开方-扣库存-收费"的完整流程。
// 后续升级路径：继承同一接口，内部改用 SQLite/SQL Server，上层代码无需改动。
//
// 文件布局（dataDir 目录下）：
//   patients.dat       每行一个患者
//   departments.dat    每行一个科室
//   doctors.dat        每行一个医生
//   drugs.dat          每行一个药品
//   registrations.dat  每行一个挂号
//   prescriptions.dat  每个处方多行（头 + 明细 + <END>）
//   billings.dat       每行一个收费记录
//   counters.dat       各类 ID 自增计数器
// ============================================================================

namespace hos {

class DataStore {
public:
    explicit DataStore(const std::string& dataDir);

    // 初始化数据目录与基础数据（首次运行调用）
    void init();

    // ---------------- 患者 ----------------
    std::vector<Patient> loadAllPatients();
    Patient* findPatient(const std::string& id);      // 在内存缓存中查找
    bool savePatient(const Patient& p);               // 新增或更新
    bool deletePatient(const std::string& id);
    std::string nextPatientId();

    // ---------------- 科室 ----------------
    std::vector<Department> loadAllDepartments();
    Department* findDepartment(const std::string& id);
    bool saveDepartment(const Department& d);
    std::string nextDepartmentId();

    // ---------------- 医生 ----------------
    std::vector<Doctor> loadAllDoctors();
    Doctor* findDoctor(const std::string& id);
    bool saveDoctor(const Doctor& d);
    bool deleteDoctor(const std::string& id);
    std::string nextDoctorId();

    // ---------------- 药品 ----------------
    std::vector<Drug> loadAllDrugs();
    Drug* findDrug(const std::string& id);
    bool saveDrug(const Drug& d);                     // 新增或更新（含库存变更）
    std::string nextDrugId();

    // ---------------- 挂号 ----------------
    std::vector<Registration> loadAllRegistrations();
    Registration* findRegistration(const std::string& id);
    bool saveRegistration(const Registration& r);
    bool updateRegistrationStatus(const std::string& id, int status);
    std::string nextRegistrationId();

    // ---------------- 处方 ----------------
    std::vector<Prescription> loadAllPrescriptions();
    Prescription* findPrescription(const std::string& id);
    bool savePrescription(const Prescription& rx);    // 新增或更新
    bool updatePrescriptionStatus(const std::string& id, int status);
    std::string nextPrescriptionId();

    // ---------------- 收费 ----------------
    std::vector<BillingRecord> loadAllBillings();
    BillingRecord* findBilling(const std::string& id);
    bool saveBilling(const BillingRecord& b);
    std::string nextBillingId();

    // 从磁盘重新加载所有数据到内存缓存
    void reload();

private:
    std::string dataDir_;

    // 内存缓存（程序启动时一次性载入，避免频繁文件 IO）
    std::vector<Patient>       patients_;
    std::vector<Department>    departments_;
    std::vector<Doctor>        doctors_;
    std::vector<Drug>          drugs_;
    std::vector<Registration>  registrations_;
    std::vector<Prescription>  prescriptions_;
    std::vector<BillingRecord> billings_;

    // ID 计数器
    struct Counters {
        int patient = 0, department = 0, doctor = 0, drug = 0;
        int registration = 0, prescription = 0, billing = 0;
    } counters_;

    // ---- 文件 IO 工具 ----
    std::vector<std::string> readLines(const std::string& file);
    bool writeLines(const std::string& file, const std::vector<std::string>& lines);
    bool appendLine(const std::string& file, const std::string& line);

    // ---- 各类持久化 ----
    void persistPatients();
    void persistDoctors();
    void persistDrugs();
    void persistRegistrations();
    void persistPrescriptions();
    void persistBillings();

    void loadCounters();
    void saveCounters();

    // 生成定长 ID，如 prefix=P, num=5, width=4 -> P0005
    static std::string makeId(const std::string& prefix, int num, int width = 4);

    // 初始化演示数据（科室、医生、药品）
    void seedDemoData();
};

} // namespace hos

#endif // DATA_STORE_H
