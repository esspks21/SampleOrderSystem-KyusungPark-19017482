#pragma once
#include "ProductManager.h"
#include "OrderManager.h"
using namespace std;

class Monitor {
public:
    Monitor(const ProductManager& pm, const OrderManager& om);

    void showDashboard() const;      // 주문 현황 + 재고 현황 통합 대시보드

private:
    const ProductManager& pm_;
    const OrderManager&   om_;

    void showOrderSection()     const;
    void showInventorySection() const;
    int  activeDemand(int productId) const;
};
