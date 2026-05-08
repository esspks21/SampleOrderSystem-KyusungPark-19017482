#pragma once
#include <string>

class Product {
public:
    Product(int id, std::string name, int stock);

    int getId() const;
    const std::string& getName() const;
    int getStock() const;
    void setStock(int stock);
    std::string toString() const;

private:
    int id_;
    std::string name_;
    int stock_;
};
