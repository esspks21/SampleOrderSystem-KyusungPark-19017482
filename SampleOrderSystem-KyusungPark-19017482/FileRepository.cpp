#include "FileRepository.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
using namespace std;

FileRepository::FileRepository(ProductManager& pm, OrderManager& om, ProductionLine& pl)
    : productManager_(pm), orderManager_(om), productionLine_(pl) {}

void FileRepository::ensureDataDir() const {
    filesystem::create_directories(kDatabaseDir);
}

void FileRepository::save() const {
    ensureDataDir();
    saveProducts();
    saveOrders();
    saveProduction();
}

bool FileRepository::load() {
    loadProducts();
    loadOrders();
    loadProduction();
    return true;
}

void FileRepository::saveProducts() const {
    ofstream f(kProductsFile);
    f << productManager_.getNextId() << "\n";
    for (const auto& p : productManager_.getAll())
        f << p.getId() << "," << p.getName() << "," << p.getStock()
          << "," << p.getAvgProductionTime() << "," << p.getYieldRate() << "\n";
}

void FileRepository::saveOrders() const {
    ofstream f(kOrdersFile);
    f << orderManager_.getNextId() << "\n";
    for (const auto& o : orderManager_.getAll())
        f << o.getId() << "," << o.getCustomerName() << ","
          << o.getProductId() << "," << o.getQuantity() << ","
          << static_cast<int>(o.getStatus()) << "\n";
}

void FileRepository::saveProduction() const {
    ofstream f(kProductionFile);
    auto pid = productionLine_.getProducingOrderId();
    f << (pid.has_value() ? *pid : -1) << "\n";
    auto q = productionLine_.getWaitingQueue();
    while (!q.empty()) { f << q.front() << "\n"; q.pop(); }
}

void FileRepository::loadProducts() {
    ifstream f(kProductsFile);
    if (!f) return;
    int nextId;
    if (!(f >> nextId)) return;
    f.ignore();
    productManager_.setNextId(nextId);
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        int id, stock;
        string name;
        char comma;
        if (!(ss >> id >> comma)) continue;
        getline(ss, name, ',');
        double avgProdTime = 0.0, yieldRate = 0.0;
        if (!(ss >> stock)) continue;
        ss >> comma >> avgProdTime >> comma >> yieldRate;
        productManager_.restoreProduct(id, name, stock, avgProdTime, yieldRate);
    }
}

void FileRepository::loadOrders() {
    ifstream f(kOrdersFile);
    if (!f) return;
    int nextId;
    if (!(f >> nextId)) return;
    f.ignore();
    orderManager_.setNextId(nextId);
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        int id, productId, quantity, statusInt;
        string customerName;
        char comma;
        if (!(ss >> id >> comma)) continue;
        getline(ss, customerName, ',');
        if (!(ss >> productId >> comma >> quantity >> comma >> statusInt)) continue;
        orderManager_.restoreOrder(id, customerName, productId, quantity,
                                   static_cast<OrderStatus>(statusInt));
    }
}

void FileRepository::loadProduction() {
    ifstream f(kProductionFile);
    if (!f) return;
    int pid;
    if (!(f >> pid)) return;
    if (pid != -1) productionLine_.setProducingOrderId(pid);
    while (f >> pid)
        if (pid >= 0) productionLine_.enqueue(pid);
}
