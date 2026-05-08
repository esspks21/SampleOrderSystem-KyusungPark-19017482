#pragma once
#include <string>
using namespace std;

class Product {
public:
    Product(int id, string name, int stock);

    int getId() const;
    const string& getName() const;
    int getStock() const;
    void setStock(int stock);
    string toString() const;

private:
    int id_;
    string name_;
    int stock_;
};
