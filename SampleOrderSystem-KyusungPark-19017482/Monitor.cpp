#include "Monitor.h"
#include <iostream>
#include <iomanip>

Monitor::Monitor(const ProductManager& pm, const OrderManager& om)
    : productManager_(pm), orderManager_(om) {}

void Monitor::showOrderSummary() const {
    std::cout << "  [상태별 주문 현황] (REJECTED 제외)\n";
    std::cout << "  RESERVED  : " << orderManager_.countByStatus(OrderStatus::RESERVED)  << "건\n";
    std::cout << "  PENDING   : " << orderManager_.countByStatus(OrderStatus::PENDING)   << "건\n";
    std::cout << "  CONFIRMED : " << orderManager_.countByStatus(OrderStatus::CONFIRMED) << "건\n";
    std::cout << "  PRODUCING : " << orderManager_.countByStatus(OrderStatus::PRODUCING) << "건\n";
    std::cout << "  RELEASE   : " << orderManager_.countByStatus(OrderStatus::RELEASE)   << "건\n";
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
    std::cout << "  [시료별 재고 현황]\n";
    if (productManager_.getAll().empty()) {
        std::cout << "  등록된 시료가 없습니다.\n";
        return;
    }
    for (const auto& p : productManager_.getAll()) {
        int stock  = p.getStock();
        int demand = activeDemandForProduct(p.getId());

        std::string inventoryStatus;
        if (stock == 0)          inventoryStatus = "고갈";
        else if (stock < demand) inventoryStatus = "부족";
        else                     inventoryStatus = "여유";

        std::cout << "  " << p.toString()
                  << " | 수요: " << demand
                  << " | 상태: " << inventoryStatus << "\n";
    }
}
