#include "data/DataStore.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <sstream>

namespace fs = std::filesystem;

namespace hos {

// -------------------- 构造与初始化 --------------------

DataStore::DataStore(const std::string& dataDir) : dataDir_(dataDir) {}

void DataStore::init() {
    // 创建数据目录
    fs::create_directories(dataDir_);
    // 加载计数器
    loadCounters();
    // 加载所有数据到内存
    reload();
    // 若为空库，初始化演示数据
    if (departments_.empty()) {
        seedDemoData();
    }
}

void DataStore::reload() {
    patients_.clear();
    for (const auto& l : readLines(dataDir_ + "/patients.dat"))
        if (!l.empty()) patients_.push_back(Patient::fromLine(l));

    departments_.clear();
    for (const auto& l : readLines(dataDir_ + "/departments.dat"))
        if (!l.empty()) departments_.push_back(Department::fromLine(l));

    doctors_.clear();
    for (const auto& l : readLines(dataDir_ + "/doctors.dat"))
        if (!l.empty()) doctors_.push_back(Doctor::fromLine(l));

    drugs_.clear();
    for (const auto& l : readLines(dataDir_ + "/drugs.dat"))
        if (!l.empty()) drugs_.push_back(Drug::fromLine(l));

    registrations_.clear();
    for (const auto& l : readLines(dataDir_ + "/registrations.dat"))
        if (!l.empty()) registrations_.push_back(Registration::fromLine(l));

    // 处方为多行格式：遍历行，以非 \t 开头为新处方头，\t 开头为明细，<END> 为结束
    prescriptions_.clear();
    {
        auto lines = readLines(dataDir_ + "/prescriptions.dat");
        std::vector<std::string> block;
        for (const auto& l : lines) {
            if (l.empty()) continue;
            bool isHeader = (l[0] != '\t');
            if (isHeader) {
                if (!block.empty()) {
                    prescriptions_.push_back(Prescription::fromLines(block));
                    block.clear();
                }
                block.push_back(l);
            } else {
                block.push_back(l);
            }
        }
        if (!block.empty())
            prescriptions_.push_back(Prescription::fromLines(block));
    }

    billings_.clear();
    for (const auto& l : readLines(dataDir_ + "/billings.dat"))
        if (!l.empty()) billings_.push_back(BillingRecord::fromLine(l));
}

// -------------------- 文件 IO 工具 --------------------

std::vector<std::string> DataStore::readLines(const std::string& file) {
    std::vector<std::string> lines;
    std::ifstream in(file);
    if (!in.is_open()) return lines;
    std::string line;
    while (std::getline(in, line)) lines.push_back(line);
    return lines;
}

bool DataStore::writeLines(const std::string& file, const std::vector<std::string>& lines) {
    std::ofstream out(file, std::ios::trunc);
    if (!out.is_open()) return false;
    for (const auto& l : lines) out << l << "\n";
    return true;
}

bool DataStore::appendLine(const std::string& file, const std::string& line) {
    std::ofstream out(file, std::ios::app);
    if (!out.is_open()) return false;
    out << line << "\n";
    return true;
}

// -------------------- ID 生成 --------------------

std::string DataStore::makeId(const std::string& prefix, int num, int width) {
    std::ostringstream os;
    os << prefix << std::setw(width) << std::setfill('0') << num;
    return os.str();
}

void DataStore::loadCounters() {
    auto lines = readLines(dataDir_ + "/counters.dat");
    for (const auto& l : lines) {
        std::istringstream is(l);
        std::string key, val;
        if (std::getline(is, key, '|') && std::getline(is, val)) {
            int v = std::stoi(val);
            if (key == "patient") counters_.patient = v;
            else if (key == "department") counters_.department = v;
            else if (key == "doctor") counters_.doctor = v;
            else if (key == "drug") counters_.drug = v;
            else if (key == "registration") counters_.registration = v;
            else if (key == "prescription") counters_.prescription = v;
            else if (key == "billing") counters_.billing = v;
        }
    }
}

void DataStore::saveCounters() {
    std::vector<std::string> lines = {
        "patient|" + std::to_string(counters_.patient),
        "department|" + std::to_string(counters_.department),
        "doctor|" + std::to_string(counters_.doctor),
        "drug|" + std::to_string(counters_.drug),
        "registration|" + std::to_string(counters_.registration),
        "prescription|" + std::to_string(counters_.prescription),
        "billing|" + std::to_string(counters_.billing),
    };
    writeLines(dataDir_ + "/counters.dat", lines);
}

std::string DataStore::nextPatientId()       { counters_.patient++;        saveCounters(); return makeId("P", counters_.patient); }
std::string DataStore::nextDepartmentId()    { counters_.department++;     saveCounters(); return makeId("D", counters_.department); }
std::string DataStore::nextDoctorId()        { counters_.doctor++;         saveCounters(); return makeId("T", counters_.doctor); }
std::string DataStore::nextDrugId()          { counters_.drug++;           saveCounters(); return makeId("G", counters_.drug); }
std::string DataStore::nextRegistrationId()  { counters_.registration++;   saveCounters(); return makeId("R", counters_.registration); }
std::string DataStore::nextPrescriptionId()  { counters_.prescription++;   saveCounters(); return makeId("RX", counters_.prescription); }
std::string DataStore::nextBillingId()       { counters_.billing++;        saveCounters(); return makeId("B", counters_.billing); }

// -------------------- 患者操作 --------------------

std::vector<Patient> DataStore::loadAllPatients() { return patients_; }

Patient* DataStore::findPatient(const std::string& id) {
    for (auto& p : patients_) if (p.id == id) return &p;
    return nullptr;
}

bool DataStore::savePatient(const Patient& p) {
    for (auto& item : patients_) {
        if (item.id == p.id) { item = p; persistPatients(); return true; }
    }
    patients_.push_back(p);
    persistPatients();
    return true;
}

bool DataStore::deletePatient(const std::string& id) {
    auto it = std::remove_if(patients_.begin(), patients_.end(),
        [&](const Patient& p) { return p.id == id; });
    if (it == patients_.end()) return false;
    patients_.erase(it, patients_.end());
    persistPatients();
    return true;
}

void DataStore::persistPatients() {
    std::vector<std::string> lines;
    for (const auto& p : patients_) lines.push_back(p.toLine());
    writeLines(dataDir_ + "/patients.dat", lines);
}

// -------------------- 科室操作 --------------------

std::vector<Department> DataStore::loadAllDepartments() { return departments_; }

Department* DataStore::findDepartment(const std::string& id) {
    for (auto& d : departments_) if (d.id == id) return &d;
    return nullptr;
}

bool DataStore::saveDepartment(const Department& d) {
    for (auto& item : departments_) {
        if (item.id == d.id) { item = d; return true; }
    }
    departments_.push_back(d);
    std::vector<std::string> lines;
    for (const auto& x : departments_) lines.push_back(x.toLine());
    writeLines(dataDir_ + "/departments.dat", lines);
    return true;
}

// -------------------- 医生操作 --------------------

std::vector<Doctor> DataStore::loadAllDoctors() { return doctors_; }

Doctor* DataStore::findDoctor(const std::string& id) {
    for (auto& d : doctors_) if (d.id == id) return &d;
    return nullptr;
}

bool DataStore::saveDoctor(const Doctor& d) {
    for (auto& item : doctors_) {
        if (item.id == d.id) { item = d; persistDoctors(); return true; }
    }
    doctors_.push_back(d);
    persistDoctors();
    return true;
}

bool DataStore::deleteDoctor(const std::string& id) {
    auto it = std::remove_if(doctors_.begin(), doctors_.end(),
        [&](const Doctor& d) { return d.id == id; });
    if (it == doctors_.end()) return false;
    doctors_.erase(it, doctors_.end());
    persistDoctors();
    return true;
}

void DataStore::persistDoctors() {
    std::vector<std::string> lines;
    for (const auto& d : doctors_) lines.push_back(d.toLine());
    writeLines(dataDir_ + "/doctors.dat", lines);
}

// -------------------- 药品操作 --------------------

std::vector<Drug> DataStore::loadAllDrugs() { return drugs_; }

Drug* DataStore::findDrug(const std::string& id) {
    for (auto& d : drugs_) if (d.id == id) return &d;
    return nullptr;
}

bool DataStore::saveDrug(const Drug& d) {
    for (auto& item : drugs_) {
        if (item.id == d.id) { item = d; persistDrugs(); return true; }
    }
    drugs_.push_back(d);
    persistDrugs();
    return true;
}

void DataStore::persistDrugs() {
    std::vector<std::string> lines;
    for (const auto& d : drugs_) lines.push_back(d.toLine());
    writeLines(dataDir_ + "/drugs.dat", lines);
}

// -------------------- 挂号操作 --------------------

std::vector<Registration> DataStore::loadAllRegistrations() { return registrations_; }

Registration* DataStore::findRegistration(const std::string& id) {
    for (auto& r : registrations_) if (r.id == id) return &r;
    return nullptr;
}

bool DataStore::saveRegistration(const Registration& r) {
    for (auto& item : registrations_) {
        if (item.id == r.id) { item = r; persistRegistrations(); return true; }
    }
    registrations_.push_back(r);
    persistRegistrations();
    return true;
}

bool DataStore::updateRegistrationStatus(const std::string& id, int status) {
    for (auto& r : registrations_) {
        if (r.id == id) { r.status = status; persistRegistrations(); return true; }
    }
    return false;
}

void DataStore::persistRegistrations() {
    std::vector<std::string> lines;
    for (const auto& r : registrations_) lines.push_back(r.toLine());
    writeLines(dataDir_ + "/registrations.dat", lines);
}

// -------------------- 处方操作 --------------------

std::vector<Prescription> DataStore::loadAllPrescriptions() { return prescriptions_; }

Prescription* DataStore::findPrescription(const std::string& id) {
    for (auto& rx : prescriptions_) if (rx.id == id) return &rx;
    return nullptr;
}

bool DataStore::savePrescription(const Prescription& rx) {
    for (auto& item : prescriptions_) {
        if (item.id == rx.id) { item = rx; persistPrescriptions(); return true; }
    }
    prescriptions_.push_back(rx);
    persistPrescriptions();
    return true;
}

bool DataStore::updatePrescriptionStatus(const std::string& id, int status) {
    for (auto& rx : prescriptions_) {
        if (rx.id == id) { rx.status = status; persistPrescriptions(); return true; }
    }
    return false;
}

void DataStore::persistPrescriptions() {
    std::vector<std::string> lines;
    for (const auto& rx : prescriptions_) {
        // 处方头
        std::ostringstream head;
        head << rx.id << "|" << rx.registrationId << "|" << rx.patientId << "|"
             << rx.patientName << "|" << rx.doctorId << "|" << rx.doctorName << "|"
             << rx.diagnosis << "|" << rx.totalAmount << "|" << rx.status << "|"
             << rx.createTime;
        lines.push_back(head.str());
        // 明细行
        for (const auto& it : rx.items) {
            std::ostringstream body;
            body << "\t" << it.drugId << "|" << it.drugName << "|" << it.quantity << "|"
                 << it.unitPrice << "|" << it.subtotal;
            lines.push_back(body.str());
        }
        lines.push_back("\t<END>");
    }
    writeLines(dataDir_ + "/prescriptions.dat", lines);
}

// -------------------- 收费操作 --------------------

std::vector<BillingRecord> DataStore::loadAllBillings() { return billings_; }

BillingRecord* DataStore::findBilling(const std::string& id) {
    for (auto& b : billings_) if (b.id == id) return &b;
    return nullptr;
}

bool DataStore::saveBilling(const BillingRecord& b) {
    for (auto& item : billings_) {
        if (item.id == b.id) { item = b; persistBillings(); return true; }
    }
    billings_.push_back(b);
    persistBillings();
    return true;
}

void DataStore::persistBillings() {
    std::vector<std::string> lines;
    for (const auto& b : billings_) lines.push_back(b.toLine());
    writeLines(dataDir_ + "/billings.dat", lines);
}

// -------------------- 演示数据初始化 --------------------

void DataStore::seedDemoData() {
    // 科室
    saveDepartment({"D0001", "内科",   "诊治呼吸、消化、心血管等内科疾病"});
    saveDepartment({"D0002", "外科",   "处理各类外科手术与创伤"});
    saveDepartment({"D0003", "儿科",   "14岁以下儿童常见病诊疗"});
    saveDepartment({"D0004", "妇产科", "妇科疾病与产前产后保健"});
    // 修正计数器到 D0004
    counters_.department = 4; saveCounters();

    // 医生
    saveDoctor({"T0001", "张明华", "D0001", "主任医师",   50.0, "13800000001"});
    saveDoctor({"T0002", "李秀芳", "D0001", "副主任医师", 30.0, "13800000002"});
    saveDoctor({"T0003", "王建国", "D0002", "主任医师",   50.0, "13800000003"});
    saveDoctor({"T0004", "刘小燕", "D0003", "主治医师",   20.0, "13800000004"});
    counters_.doctor = 4; saveCounters();

    // 药品
    saveDrug({"G0001", "阿莫西林胶囊",   "0.25g*24粒",   "盒", 15.50, 200, "华北制药"});
    saveDrug({"G0002", "布洛芬缓释胶囊", "0.3g*20粒",    "盒", 22.00, 150, "中美史克"});
    saveDrug({"G0003", "复方甘草片",     "100片",        "瓶",  8.80, 300, "广州白云山"});
    saveDrug({"G0004", "板蓝根颗粒",     "10g*20袋",     "盒", 12.00, 250, "白云山药业"});
    saveDrug({"G0005", "盐酸氨溴索口服液","100ml",       "瓶", 18.60, 120, "上海恒瑞"});
    saveDrug({"G0006", "蒙脱石散",       "3g*15袋",      "盒", 25.80,  80, "博福益普生"});
    counters_.drug = 6; saveCounters();

    std::cout << "[系统] 已初始化演示数据：4个科室、4位医生、6种药品。\n";
}

} // namespace hos
