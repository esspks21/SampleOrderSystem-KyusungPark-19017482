#pragma once
#include "ProductManager.h"
#include "OrderManager.h"
#include "ProductionLine.h"

class FileRepository {
public:
    FileRepository(ProductManager& pm, OrderManager& om, ProductionLine& pl);
    void save() const;
    bool load();

private:
    ProductManager&  productManager_;
    OrderManager&    orderManager_;
    ProductionLine&  productionLine_;

    static constexpr const char* kProductsFile   = "data/products.csv";
    static constexpr const char* kOrdersFile     = "data/orders.csv";
    static constexpr const char* kProductionFile = "data/production.txt";

    void ensureDataDir() const;
    void saveProducts() const;
    void saveOrders() const;
    void saveProduction() const;
    void loadProducts();
    void loadOrders();
    void loadProduction();
};
