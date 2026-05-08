#include "Monitor.h"
#include <iostream>
#include <iomanip>
using namespace std;

Monitor::Monitor(const ProductManager& pm, const OrderManager& om)
    : productManager_(pm), orderManager_(om) {}

void Monitor::showOrderSummary() const {
    cout << "  [상태별 주문 현황] (REJECTED 제외)\n";
    cout << "  RESERVED  : " << orderManager_.countByStatus(OrderStatus::RESERVED)  << "건\n";
    cout << "  PENDING   : " << orderManager_.countByStatus(OrderStatus::PENDING)   << "건\n";
    cout << "  CONFIRMED : " << orderManager_.countByStatus(OrderStatus::CONFIRMED) << "건\n";
    cout << "  PRODUCING : " << orderManager_.countByStatus(OrderStatus::PRODUCING) << "건\n";
    cout << "  RELEASE   : " << orderManager_.countByStatus(OrderStatus::RELEASE)   << "건\n";
}

int Monitor::activeDemandForProduct(int productId) const {
    int total = 0;
    for (const auto& o : orderManager_.getAll()) {
        OrderStatus s = o.getStatus();
        if (o.getProductId() == productId &&
            (s == OrderStatus::RESERVED || s == OrderStatus::PENDING || s == OrderStatus::CONFIRMED))
            total += o.getQuantity();
    }
    return total;
}

void Monitor::showInventoryStatus() const {
    cout << "  [시료별 재고 현황]\n";
    if (productManager_.getAll().empty()) {
        cout << "  등록된 시료가 없습니다.\n";
        return;
    }
    for (const auto& p : productManager_.getAll()) {
        int stock  = p.getStock();
        int demand = activeDemandForProduct(p.getId());

        string inventoryStatus;
        if (stock == 0)          inventoryStatus = "고갈";
        else if (stock < demand) inventoryStatus = "부족";
        else                     inventoryStatus = "여유";

        cout << "  " << p.toString()
                  << " | 수요: " << demand
                  << " | 상태: " << inventoryStatus << "\n";
    }
}
