#pragma once
#include <vector>
#include <string>
#include "Product.h"

class ProductManager {
public:
    void addProduct(const std::string& name, int stock);
    void listProducts() const;
    std::vector<Product> searchByName(const std::string& keyword) const;
    bool updateStock(int id, int delta);
    Product* findById(int id);
    const Product* findById(int id) const;
    const std::vector<Product>& getAll() const;

    void restoreProduct(int id, const std::string& name, int stock);
    void setNextId(int id);
    int  getNextId() const;

private:
    std::vector<Product> products_;
    int nextId_ = 1;
};
