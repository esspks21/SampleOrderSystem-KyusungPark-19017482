#include "Product.h"
#include <sstream>
#include <iomanip>
using namespace std;

Product::Product(int id, string name, int stock,
                 int productionTime, double yieldRate)
    : id_(id), name_(move(name)), stock_(stock),
      productionTime_(productionTime), yieldRate_(yieldRate) {}

int           Product::getId()            const { return id_; }
const string& Product::getName()          const { return name_; }
int           Product::getStock()         const { return stock_; }
int           Product::getProductionTime() const { return productionTime_; }
double        Product::getYieldRate()     const { return yieldRate_; }

void Product::setStock(int stock)              { stock_ = stock; }
void Product::setProductionTime(int minutes)   { productionTime_ = minutes; }
void Product::setYieldRate(double rate)        { yieldRate_ = rate; }

string Product::toString() const {
    ostringstream oss;
    oss << "[ID:" << id_ << "] " << name_
        << "  재고:" << stock_
        << "  생산:" << productionTime_ << "분"
        << "  수율:" << fixed << setprecision(1) << yieldRate_ << "%";
    return oss.str();
}
