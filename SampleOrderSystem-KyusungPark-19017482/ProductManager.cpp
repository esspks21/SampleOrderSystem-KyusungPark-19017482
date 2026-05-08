#include "ProductManager.h"
#include <iostream>
using namespace std;

void ProductManager::addProduct(const string& name, int stock) {
    products_.emplace_back(nextId_++, name, stock);
}

void ProductManager::listProducts() const {
    if (products_.empty()) {
        cout << "  등록된 시료가 없습니다.\n";
        return;
    }
    for (const auto& p : products_)
        cout << "  " << p.toString() << "\n";
}

vector<Product> ProductManager::searchByName(const string& keyword) const {
    vector<Product> result;
    for (const auto& p : products_)
        if (p.getName().find(keyword) != string::npos)
            result.push_back(p);
    return result;
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

void ProductManager::restoreProduct(int id, const string& name, int stock) {
    products_.emplace_back(id, name, stock);
}

void ProductManager::setNextId(int id) { nextId_ = id; }
int  ProductManager::getNextId() const { return nextId_; }
