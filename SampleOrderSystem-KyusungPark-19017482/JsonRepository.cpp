#include "JsonRepository.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <cctype>
#include <windows.h>
using namespace std;

// exe 위치에서 위로 올라가며 Database/ 폴더가 있는 루트를 반환
static filesystem::path resolveDbRoot() {
    char buf[MAX_PATH];
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    auto dir = filesystem::path(buf).parent_path();

    for (int i = 0; i < 6; ++i) {
        if (filesystem::exists(dir / "Database"))
            return dir;
        auto parent = dir.parent_path();
        if (parent == dir) break;
        dir = parent;
    }
    // 못 찾으면 현재 작업 디렉터리 사용
    return filesystem::current_path();
}

// ── 미니 JSON 유틸 (외부 라이브러리 없이 자체 구현) ─────────────

// 문자열 JSON 이스케이프
static string jEsc(const string& s) {
    string r;
    for (char c : s) {
        if      (c == '"')  r += "\\\"";
        else if (c == '\\') r += "\\\\";
        else if (c == '\n') r += "\\n";
        else                r += c;
    }
    return r;
}

// "key":INTEGER 추출 (없으면 -1)
static int64_t jInt(const string& s, const string& key) {
    const string k = "\"" + key + "\":";
    auto pos = s.find(k);
    if (pos == string::npos) return -1;
    pos += k.size();
    while (pos < s.size() && s[pos] == ' ') pos++;
    auto end = pos;
    if (end < s.size() && s[end] == '-') end++;
    while (end < s.size() && isdigit((unsigned char)s[end])) end++;
    if (end == pos) return -1;
    return stoll(s.substr(pos, end - pos));
}

// "key":"STRING" 추출 (이스케이프 처리 포함)
static string jStr(const string& s, const string& key) {
    const string k = "\"" + key + "\":\"";
    auto pos = s.find(k);
    if (pos == string::npos) return "";
    pos += k.size();
    string result;
    while (pos < s.size() && s[pos] != '"') {
        if (s[pos] == '\\' && pos + 1 < s.size()) {
            pos++;
            if      (s[pos] == '"')  result += '"';
            else if (s[pos] == '\\') result += '\\';
            else if (s[pos] == 'n')  result += '\n';
            else                     result += s[pos];
        } else {
            result += s[pos];
        }
        pos++;
    }
    return result;
}

// "key":[{...},{...}] 에서 각 오브젝트 문자열 추출
static vector<string> jObjects(const string& s, const string& arrayKey) {
    vector<string> result;
    const string k = "\"" + arrayKey + "\":[";
    auto start = s.find(k);
    if (start == string::npos) return result;
    auto pos = s.find('[', start) + 1;

    while (pos < s.size()) {
        while (pos < s.size() &&
               (s[pos] == ',' || s[pos] == '\n' || s[pos] == '\r' || s[pos] == ' '))
            pos++;
        if (pos >= s.size() || s[pos] == ']') break;
        if (s[pos] != '{') { pos++; continue; }

        int depth = 0;
        auto objStart = pos;
        while (pos < s.size()) {
            if (s[pos] == '"') {
                pos++;
                while (pos < s.size() && s[pos] != '"') {
                    if (s[pos] == '\\') pos++;
                    pos++;
                }
            } else if (s[pos] == '{') {
                depth++;
            } else if (s[pos] == '}') {
                depth--;
                if (depth == 0) { pos++; break; }
            }
            pos++;
        }
        result.push_back(s.substr(objStart, pos - objStart));
    }
    return result;
}

// "key":[1,2,3] 에서 int 배열 추출
static vector<int> jIntArray(const string& s, const string& arrayKey) {
    vector<int> result;
    const string k = "\"" + arrayKey + "\":[";
    auto pos = s.find(k);
    if (pos == string::npos) return result;
    pos = s.find('[', pos) + 1;

    while (pos < s.size() && s[pos] != ']') {
        while (pos < s.size() && !isdigit((unsigned char)s[pos]) &&
               s[pos] != '-' && s[pos] != ']') pos++;
        if (pos >= s.size() || s[pos] == ']') break;
        auto numStart = pos;
        if (s[pos] == '-') pos++;
        while (pos < s.size() && isdigit((unsigned char)s[pos])) pos++;
        result.push_back(stoi(s.substr(numStart, pos - numStart)));
    }
    return result;
}

// ── 파일 전체 읽기 ──────────────────────────────────────────
static string readFile(const string& path) {
    ifstream f(path);
    if (!f) return {};
    return {istreambuf_iterator<char>(f), istreambuf_iterator<char>()};
}

// ── JsonRepository ─────────────────────────────────────────

JsonRepository::JsonRepository(ProductManager& pm, OrderManager& om, ProductionLine& pl)
    : pm_(pm), om_(om), pl_(pl)
{
    auto root = resolveDbRoot();
    dbDir_ = (root / "Database").string();
    fProd_ = (root / "Database" / "products.json").string();
    fOrd_  = (root / "Database" / "orders.json").string();
    fLine_ = (root / "Database" / "production.json").string();
}

void JsonRepository::ensureDir() const {
    filesystem::create_directories(dbDir_);
}

// ── 저장 ───────────────────────────────────────────────────

void JsonRepository::saveProducts() const {
    ofstream f(fProd_);
    f << fixed;
    f << "{\"nextId\":" << pm_.getNextId() << ",\"items\":[\n";
    const auto& items = pm_.getAll();
    for (size_t i = 0; i < items.size(); i++) {
        const auto& p = items[i];
        f << "{\"id\":" << p.getId()
          << ",\"name\":\"" << jEsc(p.getName()) << "\""
          << ",\"stock\":" << p.getStock()
          << ",\"productionTime\":" << p.getAvgProductionTime()
          << ",\"yieldRate\":" << p.getYieldRate() << "}";
        if (i + 1 < items.size()) f << ",";
        f << "\n";
    }
    f << "]}";
}

void JsonRepository::saveOrders() const {
    ofstream f(fOrd_);
    f << "{\"nextId\":" << om_.getNextId() << ",\"items\":[\n";
    const auto& items = om_.getAll();
    for (size_t i = 0; i < items.size(); i++) {
        const auto& o = items[i];
        f << "{\"id\":" << o.getId()
          << ",\"customer\":\"" << jEsc(o.getCustomerName()) << "\""
          << ",\"productId\":" << o.getProductId()
          << ",\"qty\":" << o.getQuantity()
          << ",\"status\":" << static_cast<int>(o.getStatus())
          << ",\"createdAt\":" << static_cast<long long>(o.getCreatedAt()) << "}";
        if (i + 1 < items.size()) f << ",";
        f << "\n";
    }
    f << "]}";
}

void JsonRepository::saveProduction() const {
    ofstream f(fLine_);
    auto pid = pl_.getProducingOrderId();
    f << "{\"producing\":" << (pid.has_value() ? *pid : -1) << ",\"queue\":[";
    auto q = pl_.getWaitingQueue();
    bool first = true;
    while (!q.empty()) {
        if (!first) f << ",";
        f << q.front(); q.pop();
        first = false;
    }
    f << "]}";
}

void JsonRepository::save() const {
    ensureDir();
    saveProducts();
    saveOrders();
    saveProduction();
}

// ── 로드 ───────────────────────────────────────────────────

void JsonRepository::loadProducts() {
    const string json = readFile(fProd_);
    if (json.empty()) return;

    int64_t nextId = jInt(json, "nextId");
    if (nextId > 0) pm_.setNextId(static_cast<int>(nextId));

    for (const auto& obj : jObjects(json, "items")) {
        int    id    = static_cast<int>(jInt(obj, "id"));
        string name  = jStr(obj, "name");
        int    stock = static_cast<int>(jInt(obj, "stock"));

        auto parseDouble = [&](const string& key) -> double {
            const string k = "\"" + key + "\":";
            auto pos = obj.find(k);
            if (pos == string::npos) return 0.0;
            try { return stod(obj.substr(pos + k.size())); }
            catch (...) { return 0.0; }
        };

        double avgProdTime = parseDouble("productionTime");
        double yieldRate   = parseDouble("yieldRate");

        if (id > 0) pm_.restoreProduct(id, name, stock, avgProdTime, yieldRate);
    }
}

void JsonRepository::loadOrders() {
    const string json = readFile(fOrd_);
    if (json.empty()) return;

    int64_t nextId = jInt(json, "nextId");
    if (nextId > 0) om_.setNextId(static_cast<int>(nextId));

    for (const auto& obj : jObjects(json, "items")) {
        int       id        = static_cast<int>(jInt(obj, "id"));
        string    customer  = jStr(obj, "customer");
        int       productId = static_cast<int>(jInt(obj, "productId"));
        int       qty       = static_cast<int>(jInt(obj, "qty"));
        int       statusInt = static_cast<int>(jInt(obj, "status"));
        time_t    createdAt = static_cast<time_t>(jInt(obj, "createdAt"));

        if (id > 0) {
            om_.restoreOrder(id, customer, productId, qty,
                             static_cast<OrderStatus>(statusInt), createdAt);
        }
    }
}

void JsonRepository::loadProduction() {
    const string json = readFile(fLine_);
    if (json.empty()) return;

    int64_t pid = jInt(json, "producing");
    if (pid >= 0) pl_.setProducingOrderId(static_cast<int>(pid));

    for (int id : jIntArray(json, "queue"))
        if (id >= 0) pl_.enqueue(id);
}

bool JsonRepository::load() {
    loadProducts();
    loadOrders();
    loadProduction();
    return true;
}

// ── 전체 초기화 ─────────────────────────────────────────────

void JsonRepository::clearAll() {
    pm_.clearAll();
    om_.clearAll();
    pl_.clearAll();
    ensureDir();
    saveProducts();
    saveOrders();
    saveProduction();
}
