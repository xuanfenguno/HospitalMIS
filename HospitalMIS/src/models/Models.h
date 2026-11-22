#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <vector>
#include <sstream>

// ============================================================================
// 医院信息系统（HIS）核心数据模型
// 本头文件定义所有实体：患者、科室、医生、药品、挂号、处方、收费记录
// 每个实体提供 toLine() / fromLine() 用于基于文件的持久化存储
// 后续可替换为 SQLite/SQL Server，只需重写 DataStore 层即可
// ============================================================================

namespace hos {

// ----------------------------- 患者信息 -----------------------------
struct Patient {
    std::string id;          // 患者编号，如 P0001
    std::string name;        // 姓名
    char        gender = 'M';// 性别 M/F
    int         age = 0;     // 年龄
    std::string idCard;      // 身份证号
    std::string phone;       // 联系电话
    std::string address;     // 住址
    std::string createTime;  // 建档时间

    std::string toLine() const {
        std::ostringstream os;
        os << id << "|" << name << "|" << gender << "|" << age << "|"
           << idCard << "|" << phone << "|" << address << "|" << createTime;
        return os.str();
    }
    static Patient fromLine(const std::string& line) {
        Patient p; std::istringstream is(line);
        std::string g, a;
        std::getline(is, p.id, '|');
        std::getline(is, p.name, '|');
        std::getline(is, g, '|');       p.gender = g.empty() ? 'M' : g[0];
        std::getline(is, a, '|');       p.age = a.empty() ? 0 : std::stoi(a);
        std::getline(is, p.idCard, '|');
        std::getline(is, p.phone, '|');
        std::getline(is, p.address, '|');
        std::getline(is, p.createTime, '|');
        return p;
    }
};

// ----------------------------- 科室 -----------------------------
struct Department {
    std::string id;          // 科室编号，如 D001
    std::string name;        // 科室名称
    std::string description; // 简介

    std::string toLine() const {
        return id + "|" + name + "|" + description;
    }
    static Department fromLine(const std::string& line) {
        Department d; std::istringstream is(line);
        std::getline(is, d.id, '|');
        std::getline(is, d.name, '|');
        std::getline(is, d.description, '|');
        return d;
    }
};

// ----------------------------- 医生 -----------------------------
struct Doctor {
    std::string id;          // 医生编号，如 T0001
    std::string name;        // 姓名
    std::string departmentId;// 所属科室
    std::string title;       // 职称：主任医师/副主任医师/主治医师/住院医师
    double      registrationFee = 0.0; // 该医生的挂号费
    std::string phone;       // 联系电话

    std::string toLine() const {
        std::ostringstream os;
        os << id << "|" << name << "|" << departmentId << "|" << title << "|"
           << registrationFee << "|" << phone;
        return os.str();
    }
    static Doctor fromLine(const std::string& line) {
        Doctor d; std::istringstream is(line);
        std::string fee;
        std::getline(is, d.id, '|');
        std::getline(is, d.name, '|');
        std::getline(is, d.departmentId, '|');
        std::getline(is, d.title, '|');
        std::getline(is, fee, '|'); d.registrationFee = fee.empty() ? 0.0 : std::stod(fee);
        std::getline(is, d.phone, '|');
        return d;
    }
};

// ----------------------------- 药品 -----------------------------
struct Drug {
    std::string id;          // 药品编号，如 G0001
    std::string name;        // 药品名称
    std::string specification;// 规格，如 0.25g*24片
    std::string unit;        // 单位，如 盒/瓶/支
    double      price = 0.0; // 单价
    int         stock = 0;   // 库存数量
    std::string manufacturer;// 生产厂家

    std::string toLine() const {
        std::ostringstream os;
        os << id << "|" << name << "|" << specification << "|" << unit << "|"
           << price << "|" << stock << "|" << manufacturer;
        return os.str();
    }
    static Drug fromLine(const std::string& line) {
        Drug d; std::istringstream is(line);
        std::string p, s;
        std::getline(is, d.id, '|');
        std::getline(is, d.name, '|');
        std::getline(is, d.specification, '|');
        std::getline(is, d.unit, '|');
        std::getline(is, p, '|'); d.price = p.empty() ? 0.0 : std::stod(p);
        std::getline(is, s, '|'); d.stock = s.empty() ? 0 : std::stoi(s);
        std::getline(is, d.manufacturer, '|');
        return d;
    }
};

// ----------------------------- 挂号记录 -----------------------------
// 状态：0=已挂号待诊  1=已就诊  2=已退号
enum class RegStatus { Registered = 0, Seen = 1, Cancelled = 2 };

struct Registration {
    std::string id;          // 挂号号，如 R0001
    std::string patientId;   // 患者编号
    std::string patientName; // 患者姓名（冗余，便于显示）
    std::string doctorId;    // 接诊医生
    std::string doctorName;  // 医生姓名（冗余）
    std::string departmentId;// 科室
    double      fee = 0.0;   // 挂号费
    int         status = 0;  // RegStatus
    std::string registerTime;// 挂号时间

    std::string toLine() const {
        std::ostringstream os;
        os << id << "|" << patientId << "|" << patientName << "|" << doctorId << "|"
           << doctorName << "|" << departmentId << "|" << fee << "|" << status << "|"
           << registerTime;
        return os.str();
    }
    static Registration fromLine(const std::string& line) {
        Registration r; std::istringstream is(line);
        std::string f, s;
        std::getline(is, r.id, '|');
        std::getline(is, r.patientId, '|');
        std::getline(is, r.patientName, '|');
        std::getline(is, r.doctorId, '|');
        std::getline(is, r.doctorName, '|');
        std::getline(is, r.departmentId, '|');
        std::getline(is, f, '|'); r.fee = f.empty() ? 0.0 : std::stod(f);
        std::getline(is, s, '|'); r.status = s.empty() ? 0 : std::stoi(s);
        std::getline(is, r.registerTime, '|');
        return r;
    }
};

// ----------------------------- 处方明细 -----------------------------
struct PrescriptionItem {
    std::string drugId;
    std::string drugName;
    int         quantity = 0;
    double      unitPrice = 0.0;
    double      subtotal = 0.0; // quantity * unitPrice
};

// ----------------------------- 处方 -----------------------------
// 状态：0=待缴费  1=已缴费  2=已发药  3=已退费
enum class RxStatus { Pending = 0, Paid = 1, Dispensed = 2, Refunded = 3 };

struct Prescription {
    std::string id;          // 处方号，如 RX0001
    std::string registrationId; // 关联的挂号号
    std::string patientId;
    std::string patientName;
    std::string doctorId;
    std::string doctorName;
    std::string diagnosis;   // 诊断
    std::vector<PrescriptionItem> items; // 处方明细
    double      totalAmount = 0.0; // 药品总额
    int         status = 0;  // RxStatus
    std::string createTime;

    // 计算药品总额
    void calcTotal() {
        totalAmount = 0.0;
        for (auto& it : items) {
            it.subtotal = it.quantity * it.unitPrice;
            totalAmount += it.subtotal;
        }
    }

    // 处方序列化：首行为处方头，后续每行一个明细项（以 \t 开头标识）
    std::string toLine() const {
        std::ostringstream os;
        os << id << "|" << registrationId << "|" << patientId << "|" << patientName << "|"
           << doctorId << "|" << doctorName << "|" << diagnosis << "|"
           << totalAmount << "|" << status << "|" << createTime << "\n";
        for (const auto& it : items) {
            os << "\t" << it.drugId << "|" << it.drugName << "|" << it.quantity << "|"
               << it.unitPrice << "|" << it.subtotal << "\n";
        }
        os << "\t<END>"; // 明细结束标记
        return os.str();
    }

    // 从多行文本解析处方（lines 为该处方占用的所有行）
    static Prescription fromLines(const std::vector<std::string>& lines) {
        Prescription rx; std::istringstream is(lines[0]);
        std::string ta, st;
        std::getline(is, rx.id, '|');
        std::getline(is, rx.registrationId, '|');
        std::getline(is, rx.patientId, '|');
        std::getline(is, rx.patientName, '|');
        std::getline(is, rx.doctorId, '|');
        std::getline(is, rx.doctorName, '|');
        std::getline(is, rx.diagnosis, '|');
        std::getline(is, ta, '|'); rx.totalAmount = ta.empty() ? 0.0 : std::stod(ta);
        std::getline(is, st, '|'); rx.status = st.empty() ? 0 : std::stoi(st);
        std::getline(is, rx.createTime, '|');

        for (size_t i = 1; i < lines.size(); ++i) {
            const std::string& l = lines[i];
            if (l.empty() || l == "\t<END>") continue;
            std::string body = (l.size() > 0 && l[0] == '\t') ? l.substr(1) : l;
            if (body == "<END>") continue;
            std::istringstream is2(body);
            PrescriptionItem it;
            std::string q, up, sub;
            std::getline(is2, it.drugId, '|');
            std::getline(is2, it.drugName, '|');
            std::getline(is2, q, '|');    it.quantity = q.empty() ? 0 : std::stoi(q);
            std::getline(is2, up, '|');   it.unitPrice = up.empty() ? 0.0 : std::stod(up);
            std::getline(is2, sub, '|');  it.subtotal = sub.empty() ? 0.0 : std::stod(sub);
            rx.items.push_back(it);
        }
        return rx;
    }
};

// ----------------------------- 收费记录 -----------------------------
// 状态：0=已收费  1=已退费
enum class BillStatus { Paid = 0, Refunded = 1 };

struct BillingRecord {
    std::string id;          // 票据号，如 B0001
    std::string registrationId;
    std::string prescriptionId;
    std::string patientId;
    std::string patientName;
    double      registrationFee = 0.0; // 挂号费
    double      drugFee = 0.0;         // 药费
    double      totalAmount = 0.0;     // 合计
    int         status = 0;  // BillStatus
    std::string payTime;     // 收费时间
    std::string operatorId;  // 收费员

    std::string toLine() const {
        std::ostringstream os;
        os << id << "|" << registrationId << "|" << prescriptionId << "|" << patientId << "|"
           << patientName << "|" << registrationFee << "|" << drugFee << "|" << totalAmount << "|"
           << status << "|" << payTime << "|" << operatorId;
        return os.str();
    }
    static BillingRecord fromLine(const std::string& line) {
        BillingRecord b; std::istringstream is(line);
        std::string rf, df, ta, st;
        std::getline(is, b.id, '|');
        std::getline(is, b.registrationId, '|');
        std::getline(is, b.prescriptionId, '|');
        std::getline(is, b.patientId, '|');
        std::getline(is, b.patientName, '|');
        std::getline(is, rf, '|'); b.registrationFee = rf.empty() ? 0.0 : std::stod(rf);
        std::getline(is, df, '|'); b.drugFee = df.empty() ? 0.0 : std::stod(df);
        std::getline(is, ta, '|'); b.totalAmount = ta.empty() ? 0.0 : std::stod(ta);
        std::getline(is, st, '|'); b.status = st.empty() ? 0 : std::stoi(st);
        std::getline(is, b.payTime, '|');
        std::getline(is, b.operatorId, '|');
        return b;
    }
};

} // namespace hos

#endif // MODELS_H
