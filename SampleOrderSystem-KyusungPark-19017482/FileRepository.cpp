#include "FileRepository.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <windows.h>
using namespace std;

// exe 위치에서 위로 탐색해 Database/ 폴더 루트를 반환
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
    return filesystem::current_path();
}

FileRepository::FileRepository(ProductManager& pm, OrderManager& om, ProductionLine& pl)
    : pm_(pm), om_(om), pl_(pl)
{
    auto root = resolveDbRoot();
    dbDir_ = (root / "Database").string();
    fProd_ = (root / "Database" / "products.csv").string();
    fOrd_  = (root / "Database" / "orders.csv").string();
}

void FileRepository::ensureDir() const {
    filesystem::create_directories(dbDir_);
}

// ── 저장 ──────────────────────────────────────────────────

void FileRepository::save() const {
    ensureDir();
    saveProducts();
    saveOrders();
}

void FileRepository::saveProducts() const {
    ofstream f(fProd_);
    f << pm_.getNextId() << "\n";
    f << fixed << setprecision(1);
    for (const auto& p : pm_.getAll())
        f << p.getId() << ","
          << p.getName() << ","
          << p.getStock() << ","
          << p.getAvgProductionTime() << ","
          << p.getYieldRate() << "\n";
}

void FileRepository::saveOrders() const {
    ofstream f(fOrd_);
    f << om_.getNextId() << "\n";
    for (const auto& o : om_.getAll())
        f << o.getId() << ","
          << o.getCustomerName() << ","
          << o.getProductId() << ","
          << o.getQuantity() << ","
          << static_cast<int>(o.getStatus()) << ","
          << static_cast<long long>(o.getCreatedAt()) << ","
          << static_cast<long long>(o.getProducingStartedAt()) << ","
          << o.getProducingActualQty() << "\n";
}

// ── 로드 ──────────────────────────────────────────────────

static void stripCR(string& s) {
    if (!s.empty() && s.back() == '\r') s.pop_back();
}

bool FileRepository::load() {
    loadProducts();
    loadOrders();
    return true;
}

void FileRepository::loadProducts() {
    ifstream f(fProd_);
    if (!f) return;
    int nextId;
    if (!(f >> nextId)) return;
    f.ignore();
    pm_.setNextId(nextId);
    string line;
    while (getline(f, line)) {
        stripCR(line);
        if (line.empty()) continue;
        istringstream ss(line);
        int id, stock; string name; char c; double pt, yr;
        if (!(ss >> id >> c)) continue;
        getline(ss, name, ',');
        if (!(ss >> stock >> c >> pt >> c >> yr)) continue;
        pm_.restoreProduct(id, name, stock, pt, yr);
    }
}

void FileRepository::loadOrders() {
    ifstream f(fOrd_);
    if (!f) return;
    int nextId;
    if (!(f >> nextId)) return;
    f.ignore();
    om_.setNextId(nextId);
    string line;
    while (getline(f, line)) {
        stripCR(line);
        if (line.empty()) continue;
        istringstream ss(line);
        int id, productId, qty, statusInt, producingActualQty;
        string customer; char c;
        long long createdAt, producingStartedAt;
        if (!(ss >> id >> c)) continue;
        getline(ss, customer, ',');
        if (!(ss >> productId >> c >> qty >> c >> statusInt >> c
              >> createdAt >> c >> producingStartedAt >> c
              >> producingActualQty)) continue;
        om_.restoreOrder(id, customer, productId, qty,
                         static_cast<OrderStatus>(statusInt),
                         static_cast<time_t>(createdAt));
        Order* o = om_.findById(id);
        if (o) {
            o->setProducingStartedAt(static_cast<time_t>(producingStartedAt));
            o->setProducingActualQty(producingActualQty);
        }
    }
}

// ── 전체 초기화 ───────────────────────────────────────────

void FileRepository::clearAll() {
    pm_.clearAll();
    om_.clearAll();
    pl_.clearAll();
    ensureDir();
    saveProducts();
    saveOrders();
}
