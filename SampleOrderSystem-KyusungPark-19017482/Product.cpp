#include "Product.h"
#include <sstream>

Product::Product(int id, std::string name, int stock)
    : id_(id), name_(std::move(name)), stock_(stock) {}

int Product::getId() const { return id_; }
const std::string& Product::getName() const { return name_; }
int Product::getStock() const { return stock_; }

void Product::setStock(int stock) {
    stock_ = stock;
}

std::string Product::toString() const {
    std::ostringstream oss;
    oss << "[ID:" << id_ << "] " << name_ << " (재고: " << stock_ << ")";
    return oss.str();
}
