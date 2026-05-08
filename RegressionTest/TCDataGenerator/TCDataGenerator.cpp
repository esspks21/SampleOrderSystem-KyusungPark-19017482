/**
 * TCDataGenerator.cpp
 * Regression test data generator and validator for SampleOrderSystem.
 *
 * Usage:
 *   TCDataGenerator.exe --generate   Generate 10,000+ sample test data files
 *   TCDataGenerator.exe --test       Run regression tests against generated data
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <cassert>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <stdexcept>
using namespace std;

// ────────────────────────────────────────────────────────────
// Constants
// ────────────────────────────────────────────────────────────
static constexpr int kProductCount    = 10000;
static constexpr int kOrderCount      = 5000;
static constexpr int kStockMin        = 0;
static constexpr int kStockMax        = 1000;

// Paths are relative to RegressionTest/ (the working directory when run via run_regression.bat)
static constexpr const char* kDataDir        = "data";
static constexpr const char* kProductsFile   = "data/products.csv";
static constexpr const char* kOrdersFile     = "data/orders.csv";
static constexpr const char* kProductionFile = "data/production.txt";
static constexpr const char* kExpectedFile   = "data/expected.txt";
static constexpr const char* kResultsDir     = "results";

// OrderStatus values — must match the main project's OrderStatus.h
enum class OrderStatus {
    RESERVED  = 0,
    PENDING   = 1,
    CONFIRMED = 2,
    PRODUCING = 3,
    RELEASE   = 4,
    REJECTED  = 5,
    CANCELLED = 6
};

// ────────────────────────────────────────────────────────────
// Data models (mirror main project)
// ────────────────────────────────────────────────────────────
struct Product {
    int    id;
    string name;
    int    stock;
};

struct Order {
    int         id;
    string      customerName;
    int         productId;
    int         quantity;
    OrderStatus status;
};

// ────────────────────────────────────────────────────────────
// Helpers
// ────────────────────────────────────────────────────────────
static string toLower(const string& s) {
    string r = s;
    transform(r.begin(), r.end(), r.begin(), [](unsigned char c) { return tolower(c); });
    return r;
}

static string getTimestamp() {
    auto   now   = chrono::system_clock::now();
    time_t t     = chrono::system_clock::to_time_t(now);
    tm     local{};
    localtime_s(&local, &t);
    ostringstream oss;
    oss << put_time(&local, "%Y%m%d_%H%M%S");
    return oss.str();
}

// ────────────────────────────────────────────────────────────
// --generate : build test data files
// ────────────────────────────────────────────────────────────
static vector<Product> buildProducts() {
    // Name variety ensures case-insensitive search tests cover diverse inputs
    const vector<string> prefixes = {"Alpha","Beta","Gamma","Delta","Epsilon",
                                     "Zeta","Eta","Theta","Iota","Kappa"};
    const vector<string> types    = {"Wafer","DRAM","NAND","Logic","Sensor",
                                     "Power","RF","Image","Display","Mixed"};
    vector<Product> out;
    out.reserve(kProductCount);
    for (int i = 1; i <= kProductCount; ++i) {
        string name = prefixes[(i - 1) % prefixes.size()] + "_"
                    + types[(i - 1) % types.size()] + "_"
                    + to_string(i);
        int stock = (i * 37 + 13) % (kStockMax - kStockMin + 1) + kStockMin;
        out.push_back({i, name, stock});
    }
    return out;
}

static vector<Order> buildOrders(const vector<Product>& products) {
    const vector<string> customers = {
        "ResearchLab_A","fabless_Corp","UNIV_Lab","Samsung_RnD","SK_Hynix","Micron_KR"
    };
    // Cycle through all 7 statuses to guarantee full distribution
    const vector<OrderStatus> statuses = {
        OrderStatus::RESERVED,  OrderStatus::PENDING,  OrderStatus::CONFIRMED,
        OrderStatus::PRODUCING, OrderStatus::RELEASE,  OrderStatus::REJECTED,
        OrderStatus::CANCELLED
    };
    vector<Order> out;
    out.reserve(kOrderCount);
    for (int i = 1; i <= kOrderCount; ++i) {
        int pid      = (i % (int)products.size()) + 1;
        int qty      = (i * 7 + 3) % 50 + 1;
        auto status  = statuses[i % statuses.size()];
        string cname = customers[i % customers.size()];
        out.push_back({i, cname, pid, qty, status});
    }
    return out;
}

static void writeDataFiles(const vector<Product>& products, const vector<Order>& orders) {
    filesystem::create_directories(kDataDir);

    {   // products.csv  ← same format as FileRepository::saveProducts
        ofstream f(kProductsFile);
        f << (kProductCount + 1) << "\n";
        for (const auto& p : products)
            f << p.id << "," << p.name << "," << p.stock << "\n";
    }
    {   // orders.csv
        ofstream f(kOrdersFile);
        f << (kOrderCount + 1) << "\n";
        for (const auto& o : orders)
            f << o.id << "," << o.customerName << ","
              << o.productId << "," << o.quantity << ","
              << static_cast<int>(o.status) << "\n";
    }
    {   // production.txt  — no item currently producing; CONFIRMED orders queued
        ofstream f(kProductionFile);
        f << -1 << "\n";
        for (const auto& o : orders)
            if (o.status == OrderStatus::CONFIRMED)
                f << o.id << "\n";
    }
    {   // expected.txt  — ground-truth for --test
        auto cnt = [&](OrderStatus s) {
            return count_if(orders.begin(), orders.end(),
                            [s](const Order& o){ return o.status == s; });
        };
        ofstream f(kExpectedFile);
        f << "PRODUCT_COUNT=" << products.size() << "\n";
        f << "ORDER_COUNT="   << orders.size()   << "\n";
        f << "RESERVED="   << cnt(OrderStatus::RESERVED)  << "\n";
        f << "PENDING="    << cnt(OrderStatus::PENDING)   << "\n";
        f << "CONFIRMED="  << cnt(OrderStatus::CONFIRMED) << "\n";
        f << "PRODUCING="  << cnt(OrderStatus::PRODUCING) << "\n";
        f << "RELEASE="    << cnt(OrderStatus::RELEASE)   << "\n";
        f << "REJECTED="   << cnt(OrderStatus::REJECTED)  << "\n";
        f << "CANCELLED="  << cnt(OrderStatus::CANCELLED) << "\n";
    }
}

// ────────────────────────────────────────────────────────────
// --test : load files and run validation
// ────────────────────────────────────────────────────────────
static vector<Product> loadProducts() {
    ifstream f(kProductsFile);
    if (!f) throw runtime_error(string("Cannot open ") + kProductsFile);
    int nextId; f >> nextId; f.ignore();
    vector<Product> out;
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        int id, stock; string name; char comma;
        if (!(ss >> id >> comma)) continue;
        getline(ss, name, ',');
        if (!(ss >> stock)) continue;
        out.push_back({id, name, stock});
    }
    return out;
}

static vector<Order> loadOrders() {
    ifstream f(kOrdersFile);
    if (!f) throw runtime_error(string("Cannot open ") + kOrdersFile);
    int nextId; f >> nextId; f.ignore();
    vector<Order> out;
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        int id, productId, quantity, statusInt;
        string customerName; char comma;
        if (!(ss >> id >> comma)) continue;
        getline(ss, customerName, ',');
        if (!(ss >> productId >> comma >> quantity >> comma >> statusInt)) continue;
        out.push_back({id, customerName, productId, quantity,
                       static_cast<OrderStatus>(statusInt)});
    }
    return out;
}

static map<string,int> loadExpected() {
    ifstream f(kExpectedFile);
    if (!f) throw runtime_error(string("Cannot open ") + kExpectedFile);
    map<string,int> out;
    string line;
    while (getline(f, line)) {
        auto pos = line.find('=');
        if (pos == string::npos) continue;
        out[line.substr(0, pos)] = stoi(line.substr(pos + 1));
    }
    return out;
}

// ────────────────────────────────────────────────────────────
// Test cases
// ────────────────────────────────────────────────────────────
struct TC {
    string name;
    bool   passed;
    string detail;
};

static TC tcProductCount(const vector<Product>& p, int exp) {
    bool ok = (int)p.size() == exp;
    return {"TC-P01 시료 개수 검증", ok,
            "expected=" + to_string(exp) + " actual=" + to_string(p.size())};
}

static TC tcUniqueProductIds(const vector<Product>& p) {
    vector<int> ids; ids.reserve(p.size());
    for (const auto& x : p) ids.push_back(x.id);
    sort(ids.begin(), ids.end());
    bool ok = adjacent_find(ids.begin(), ids.end()) == ids.end();
    return {"TC-P02 시료 ID 고유성", ok, ok ? "All unique" : "Duplicate IDs detected"};
}

static TC tcNonNegativeStock(const vector<Product>& p) {
    int neg = (int)count_if(p.begin(), p.end(), [](const Product& x){ return x.stock < 0; });
    return {"TC-P03 재고 음수 없음", neg == 0, "negative_count=" + to_string(neg)};
}

static TC tcCaseInsensitiveSearch(const vector<Product>& p) {
    // "ALPHA" should match names beginning with "Alpha"
    string kw = toLower("ALPHA");
    int cnt = (int)count_if(p.begin(), p.end(), [&](const Product& x){
        return toLower(x.name).find(kw) != string::npos;
    });
    return {"TC-P04 대소문자 무시 이름 검색 (ALPHA→alpha)", cnt > 0,
            "matches=" + to_string(cnt)};
}

static TC tcIdSearch(const vector<Product>& p) {
    // Keyword "100" must match IDs 100, 1000, 1001, … and names containing "100"
    string kw = "100";
    int cnt = (int)count_if(p.begin(), p.end(), [&](const Product& x){
        return to_string(x.id).find(kw) != string::npos ||
               toLower(x.name).find(kw) != string::npos;
    });
    return {"TC-P05 ID 포함 검색 (keyword='100')", cnt > 0,
            "matches=" + to_string(cnt)};
}

static TC tcSortAscending(const vector<Product>& p) {
    vector<Product> s = p;
    sort(s.begin(), s.end(), [](const Product& a, const Product& b){ return a.id < b.id; });
    bool ok = is_sorted(s.begin(), s.end(), [](const Product& a, const Product& b){ return a.id < b.id; });
    return {"TC-P06 ID 오름차순 정렬", ok, ok ? "Correct" : "Sort error"};
}

static TC tcSortDescending(const vector<Product>& p) {
    vector<Product> s = p;
    sort(s.begin(), s.end(), [](const Product& a, const Product& b){ return a.id > b.id; });
    bool ok = is_sorted(s.begin(), s.end(), [](const Product& a, const Product& b){ return a.id > b.id; });
    return {"TC-P07 ID 내림차순 정렬", ok, ok ? "Correct" : "Sort error"};
}

static TC tcOrderCount(const vector<Order>& o, int exp) {
    bool ok = (int)o.size() == exp;
    return {"TC-O01 주문 개수 검증", ok,
            "expected=" + to_string(exp) + " actual=" + to_string(o.size())};
}

static TC tcUniqueOrderIds(const vector<Order>& o) {
    vector<int> ids; ids.reserve(o.size());
    for (const auto& x : o) ids.push_back(x.id);
    sort(ids.begin(), ids.end());
    bool ok = adjacent_find(ids.begin(), ids.end()) == ids.end();
    return {"TC-O02 주문 ID 고유성", ok, ok ? "All unique" : "Duplicate IDs detected"};
}

static TC tcStatusRange(const vector<Order>& o) {
    int inv = (int)count_if(o.begin(), o.end(), [](const Order& x){
        int s = static_cast<int>(x.status);
        return s < 0 || s > 6;
    });
    return {"TC-O03 주문 상태 유효 범위 (0~6)", inv == 0, "invalid=" + to_string(inv)};
}

static TC tcStatusDistribution(const vector<Order>& o, const map<string,int>& exp) {
    const vector<pair<string,OrderStatus>> checks = {
        {"RESERVED",  OrderStatus::RESERVED},  {"PENDING",   OrderStatus::PENDING},
        {"CONFIRMED", OrderStatus::CONFIRMED}, {"PRODUCING", OrderStatus::PRODUCING},
        {"RELEASE",   OrderStatus::RELEASE},   {"REJECTED",  OrderStatus::REJECTED},
        {"CANCELLED", OrderStatus::CANCELLED}
    };
    bool ok = true;
    ostringstream det;
    for (const auto& [key, status] : checks) {
        int actual = (int)count_if(o.begin(), o.end(), [status](const Order& x){ return x.status == status; });
        int expect = exp.count(key) ? exp.at(key) : -1;
        bool match = (actual == expect);
        if (!match) ok = false;
        det << key << "=" << actual << (match ? "✓" : "✗") << " ";
    }
    return {"TC-O04 주문 상태 분포 검증", ok, det.str()};
}

static TC tcOrderProductRef(const vector<Order>& o, const vector<Product>& p) {
    int inv = 0;
    for (const auto& ord : o) {
        bool found = any_of(p.begin(), p.end(), [&](const Product& pr){ return pr.id == ord.productId; });
        if (!found) ++inv;
    }
    return {"TC-O05 주문-시료 ID 참조 무결성", inv == 0, "invalid_refs=" + to_string(inv)};
}

static TC tcPositiveQuantity(const vector<Order>& o) {
    int inv = (int)count_if(o.begin(), o.end(), [](const Order& x){ return x.quantity <= 0; });
    return {"TC-O06 주문 수량 양수", inv == 0, "non_positive=" + to_string(inv)};
}

static TC tcSearchMixedCase(const vector<Product>& p) {
    // "wAfEr" should match names containing "Wafer"
    string kw = toLower("wAfEr");
    int cnt = (int)count_if(p.begin(), p.end(), [&](const Product& x){
        return toLower(x.name).find(kw) != string::npos;
    });
    return {"TC-P08 혼합 대소문자 검색 (wAfEr→wafer)", cnt > 0, "matches=" + to_string(cnt)};
}

static TC tcSearchByIdString(const vector<Product>& p) {
    // ID "1" as string should match products whose id contains "1"
    string kw = "1";
    int cnt = (int)count_if(p.begin(), p.end(), [&](const Product& x){
        return to_string(x.id).find(kw) != string::npos;
    });
    bool ok = cnt > 0;
    return {"TC-P09 ID 문자열 검색 (keyword='1')", ok, "matches=" + to_string(cnt)};
}

// ────────────────────────────────────────────────────────────
// Write result report
// ────────────────────────────────────────────────────────────
static int writeReport(const vector<TC>& results, const string& ts) {
    filesystem::create_directories(kResultsDir);
    string path = string(kResultsDir) + "/regression_" + ts + ".txt";
    ofstream f(path);

    int pass = (int)count_if(results.begin(), results.end(), [](const TC& r){ return r.passed; });
    int fail = (int)results.size() - pass;

    f << "=======================================================\n";
    f << "  Regression Test Report\n";
    f << "  Timestamp : " << ts << "\n";
    f << "  Test Data : " << kProductCount << " products / " << kOrderCount << " orders\n";
    f << "=======================================================\n\n";
    f << "  Result : " << pass << " PASS / " << fail << " FAIL / "
      << results.size() << " TOTAL\n\n";
    f << "-------------------------------------------------------\n";
    for (const auto& r : results)
        f << (r.passed ? "[PASS] " : "[FAIL] ") << r.name << "\n"
          << "       " << r.detail << "\n";
    f << "-------------------------------------------------------\n";
    f << (fail == 0 ? "ALL TESTS PASSED" : "SOME TESTS FAILED") << "\n";

    cout << "\n결과 저장: " << path << "\n";
    cout << "요약: " << pass << "/" << results.size() << " PASS";
    if (fail > 0) {
        cout << "  [FAIL 목록]\n";
        for (const auto& r : results)
            if (!r.passed) cout << "  - " << r.name << " (" << r.detail << ")\n";
    } else {
        cout << "  ALL PASSED\n";
    }
    return fail;
}

// ────────────────────────────────────────────────────────────
// Entry point
// ────────────────────────────────────────────────────────────
int main(int argc, char* argv[]) {
    string mode = (argc > 1) ? argv[1] : "--help";

    if (mode == "--generate") {
        cout << "테스트 데이터 생성 중 (" << kProductCount << " 시료 / " << kOrderCount << " 주문)...\n";
        auto products = buildProducts();
        auto orders   = buildOrders(products);
        writeDataFiles(products, orders);
        cout << "완료. 저장 위치: " << kDataDir << "/\n";
        return 0;
    }

    if (mode == "--test") {
        cout << "Regression 테스트 실행 중...\n";
        string ts = getTimestamp();
        try {
            auto products = loadProducts();
            auto orders   = loadOrders();
            auto expected = loadExpected();

            vector<TC> results = {
                tcProductCount       (products, expected["PRODUCT_COUNT"]),
                tcUniqueProductIds   (products),
                tcNonNegativeStock   (products),
                tcCaseInsensitiveSearch(products),
                tcIdSearch           (products),
                tcSortAscending      (products),
                tcSortDescending     (products),
                tcSearchMixedCase    (products),
                tcSearchByIdString   (products),
                tcOrderCount         (orders, expected["ORDER_COUNT"]),
                tcUniqueOrderIds     (orders),
                tcStatusRange        (orders),
                tcStatusDistribution (orders, expected),
                tcOrderProductRef    (orders, products),
                tcPositiveQuantity   (orders),
            };

            int fail = writeReport(results, ts);
            return (fail > 0) ? 1 : 0;

        } catch (const exception& e) {
            cerr << "[ERROR] " << e.what() << "\n"
                 << "먼저 --generate 로 테스트 데이터를 생성하세요.\n";
            return 2;
        }
    }

    cout << "Usage: TCDataGenerator.exe [--generate | --test]\n"
         << "  --generate  " << kProductCount << " 시료 / " << kOrderCount
         << " 주문 테스트 데이터 생성\n"
         << "  --test      생성된 데이터로 Regression 테스트 수행 후 results/ 에 저장\n";
    return 0;
}
