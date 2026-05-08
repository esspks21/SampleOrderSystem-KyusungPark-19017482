#pragma once
#include "ProductManager.h"
#include "OrderManager.h"
#include "ProductionLine.h"
using namespace std;

class FileRepository {
public:
    FileRepository(ProductManager& pm, OrderManager& om, ProductionLine& pl);
    void save() const;
    bool load();

private:
    ProductManager&  productManager_;
    OrderManager&    orderManager_;
    ProductionLine&  productionLine_;

    static constexpr const char* kDatabaseDir    = "../Database";
    static constexpr const char* kProductsFile   = "../Database/products.csv";
    static constexpr const char* kOrdersFile     = "../Database/orders.csv";
    static constexpr const char* kProductionFile = "../Database/production.txt";

    void ensureDataDir() const;
    void saveProducts() const;
    void saveOrders() const;
    void saveProduction() const;
    void loadProducts();
    void loadOrders();
    void loadProduction();
};
