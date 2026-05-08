#pragma once
#include "ProductManager.h"
#include "OrderManager.h"
#include "ProductionLine.h"
using namespace std;

// CSV 기반 데이터 영속성 — Database/products.csv, Database/orders.csv
class FileRepository {
public:
    FileRepository(ProductManager& pm, OrderManager& om, ProductionLine& pl);
    void save() const;
    bool load();
    void clearAll();

private:
    ProductManager& pm_;
    OrderManager&   om_;
    ProductionLine& pl_;

    string dbDir_;
    string fProd_;
    string fOrd_;

    void ensureDir()      const;
    void saveProducts()   const;
    void saveOrders()     const;
    void loadProducts();
    void loadOrders();
};
