#include "Product.h"
#include <sstream>
#include <iomanip>
using namespace std;

Product::Product(int id, string name, int stock,
                 double avgProductionTime, double yieldRate)
    : id_(id), name_(move(name)), stock_(stock),
      avgProductionTime_(avgProductionTime), yieldRate_(yieldRate) {}

int           Product::getId()               const { return id_; }
const string& Product::getName()             const { return name_; }
int           Product::getStock()            const { return stock_; }
double        Product::getAvgProductionTime() const { return avgProductionTime_; }
double        Product::getYieldRate()        const { return yieldRate_; }

void Product::setStock(int stock)                  { stock_ = stock; }
void Product::setAvgProductionTime(double minutes) { avgProductionTime_ = minutes; }
void Product::setYieldRate(double rate)            { yieldRate_ = rate; }

string Product::toString() const {
    ostringstream oss;
    oss << fixed << setprecision(1);
    oss << left  << setw(6)  << id_
        << "\t"  << setw(28) << name_
        << "\t"  << right << setw(10) << avgProductionTime_
        << "\t"  << setw(8)  << yieldRate_
        << "\t"  << setw(6)  << stock_;
    return oss.str();
}
