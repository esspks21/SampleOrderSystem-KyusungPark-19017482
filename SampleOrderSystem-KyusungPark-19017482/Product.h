#pragma once
#include <string>
using namespace std;

class Product {
public:
    Product(int id, string name, int stock,
            double avgProductionTime = 0.0, double yieldRate = 0.0);

    int           getId()               const;
    const string& getName()             const;
    int           getStock()            const;
    double        getAvgProductionTime() const;   // 평균 생산시간 (분, 소수점 1자리)
    double        getYieldRate()        const;    // 수율 (%)

    void setStock(int stock);
    void setAvgProductionTime(double minutes);
    void setYieldRate(double rate);

    string toString() const;

private:
    int    id_;
    string name_;
    int    stock_;
    double avgProductionTime_;  // 평균 생산 소요시간 (분)
    double yieldRate_;          // 수율 0.0 ~ 100.0 (%)
};
