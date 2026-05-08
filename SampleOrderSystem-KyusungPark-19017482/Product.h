#pragma once
#include <string>
using namespace std;

class Product {
public:
    Product(int id, string name, int stock,
            int productionTime = 0, double yieldRate = 0.0);

    int           getId()            const;
    const string& getName()          const;
    int           getStock()         const;
    int           getProductionTime() const;   // 생산시간 (분)
    double        getYieldRate()     const;    // 수율 (%)

    void setStock(int stock);
    void setProductionTime(int minutes);
    void setYieldRate(double rate);

    string toString() const;

private:
    int    id_;
    string name_;
    int    stock_;
    int    productionTime_;   // 분 단위 생산 소요시간
    double yieldRate_;        // 수율 0.0 ~ 100.0 (%)
};
