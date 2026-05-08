#include "ProductManager.h"
#include <iostream>
#include <algorithm>
#include <cctype>
using namespace std;

namespace {
    string toLower(const string& s) {
        string result = s;
        transform(result.begin(), result.end(), result.begin(),
                  [](unsigned char c) { return tolower(c); });
        return result;
    }
}

void ProductManager::addProduct(const string& name, int stock,
                                double avgProductionTime, double yieldRate) {
    products_.emplace_back(nextId_++, name, stock, avgProductionTime, yieldRate);
}

void ProductManager::listProducts(bool ascending) const {
    if (products_.empty()) {
        cout << "  등록된 시료가 없습니다.\n";
        return;
    }
    vector<Product> sorted = products_;
    sort(sorted.begin(), sorted.end(), [ascending](const Product& a, const Product& b) {
        return ascending ? a.getId() < b.getId() : a.getId() > b.getId();
    });
    for (const auto& p : sorted)
        cout << "  " << p.toString() << "\n";
}

vector<Product> ProductManager::search(const string& keyword, bool ascending) const {
    const string kw = toLower(keyword);
    vector<Product> result;
    for (const auto& p : products_) {
        bool matchId   = toLower(to_string(p.getId())).find(kw) != string::npos;
        bool matchName = toLower(p.getName()).find(kw) != string::npos;
        if (matchId || matchName)
            result.push_back(p);
    }
    sort(result.begin(), result.end(), [ascending](const Product& a, const Product& b) {
        return ascending ? a.getId() < b.getId() : a.getId() > b.getId();
    });
    return result;
}

bool ProductManager::updateProduct(int id, const string& name, int stock,
                                   double avgProductionTime, double yieldRate) {
    Product* p = findById(id);
    if (!p) return false;
    // name이 비어 있으면 기존값 유지
    if (!name.empty()) {
        // Product는 name을 직접 수정하는 setter가 없으므로 필드별 setter 사용
        // name은 불변이 아니므로 새 Product로 교체하는 대신 직접 값 반영
        // (Product에 setName 없음 → 재구성)
        *p = Product(id, name, stock, avgProductionTime, yieldRate);
    } else {
        p->setStock(stock);
        p->setAvgProductionTime(avgProductionTime);
        p->setYieldRate(yieldRate);
    }
    return true;
}

bool ProductManager::deleteProduct(int id) {
    auto it = find_if(products_.begin(), products_.end(),
                      [id](const Product& p) { return p.getId() == id; });
    if (it == products_.end()) return false;
    products_.erase(it);
    return true;
}

bool ProductManager::updateStock(int id, int delta) {
    Product* p = findById(id);
    if (!p) return false;
    int newStock = p->getStock() + delta;
    if (newStock < 0) return false;
    p->setStock(newStock);
    return true;
}

Product* ProductManager::findById(int id) {
    for (auto& p : products_)
        if (p.getId() == id) return &p;
    return nullptr;
}

const Product* ProductManager::findById(int id) const {
    for (const auto& p : products_)
        if (p.getId() == id) return &p;
    return nullptr;
}

const vector<Product>& ProductManager::getAll() const { return products_; }

void ProductManager::restoreProduct(int id, const string& name, int stock,
                                    double avgProductionTime, double yieldRate) {
    products_.emplace_back(id, name, stock, avgProductionTime, yieldRate);
}

void ProductManager::setNextId(int id) { nextId_ = id; }
int  ProductManager::getNextId() const { return nextId_; }

void ProductManager::clearAll() {
    products_.clear();
    nextId_ = 1;
}
