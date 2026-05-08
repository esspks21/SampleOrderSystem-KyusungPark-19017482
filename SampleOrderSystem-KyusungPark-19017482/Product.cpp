#include "Product.h"
#include <sstream>
using namespace std;

Product::Product(int id, string name, int stock)
    : id_(id), name_(move(name)), stock_(stock) {}

int Product::getId() const { return id_; }
const string& Product::getName() const { return name_; }
int Product::getStock() const { return stock_; }

void Product::setStock(int stock) {
    stock_ = stock;
}

string Product::toString() const {
    ostringstream oss;
    oss << "[ID:" << id_ << "] " << name_ << " (재고: " << stock_ << ")";
    return oss.str();
}
