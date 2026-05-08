#pragma once
#include <vector>
#include <string>
#include "Product.h"
using namespace std;

class ProductManager {
public:
    void addProduct(const string& name, int stock);
    void listProducts(bool ascending = true) const;
    vector<Product> search(const string& keyword, bool ascending = true) const;
    bool updateStock(int id, int delta);
    Product* findById(int id);
    const Product* findById(int id) const;
    const vector<Product>& getAll() const;

    void restoreProduct(int id, const string& name, int stock);
    void setNextId(int id);
    int  getNextId() const;

private:
    vector<Product> products_;
    int nextId_ = 1;
};
