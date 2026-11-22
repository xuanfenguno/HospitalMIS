#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include <memory>
#include "data/DataStore.h"
#include "services/PatientService.h"
#include "services/DoctorService.h"
#include "services/DrugService.h"
#include "services/RegistrationService.h"
#include "services/PrescriptionService.h"
#include "services/BillingService.h"

namespace hos {

// 控制台 UI 主控：负责菜单展示与各模块调度
class ConsoleUI {
public:
    explicit ConsoleUI(const std::string& dataDir);
    ~ConsoleUI();

    // 启动系统主循环
    void run();

private:
    DataStore                 store_;
    PatientService            patientSvc_;
    DoctorService             doctorSvc_;
    DrugService               drugSvc_;
    RegistrationService       regSvc_;
    PrescriptionService       rxSvc_;
    BillingService            billSvc_;

    void showMainMenu();
    void showRegistrationMenu();    // 门诊挂号子菜单
    void showDoctorMenu();          // 医生工作站子菜单
    void showBillingMenu();         // 收费结算子菜单
    void showPharmacyMenu();        // 药房管理子菜单
    void showDataMenu();            // 基础数据维护子菜单
    void showQueryMenu();           // 查询统计子菜单
    void showAbout();               // 关于
};

} // namespace hos
#endif
