// Phase 7 테스트 하네스 — Monitor (주문 현황 집계, 재고 상태)
// 컴파일: cl /std:c++20 /I..\SampleOrderSystem-KyusungPark-19017482 test_phase7.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\Product.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\Order.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\ProductManager.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\OrderManager.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\Monitor.cpp

#include <iostream>
#include "../SampleOrderSystem-KyusungPark-19017482/Monitor.h"

static int passed = 0, failed = 0;

static void check(const std::string& label, bool cond) {
    if (cond) { std::cout << "[PASS] " << label << "\n"; ++passed; }
    else       { std::cout << "[FAIL] " << label << "\n"; ++failed; }
}

static void testOrderSummary() {
    std::cout << "\n--- showOrderSummary ---\n";
    ProductManager pm; pm.addProduct("Si-A", 100);
    OrderManager om(pm);

    om.reserveOrder("A", 1, 5);  // RESERVED
    om.reserveOrder("B", 1, 3);
    om.acceptOrder(2);           // PENDING
    om.reserveOrder("C", 1, 2);
    om.acceptOrder(3);
    om.approveOrder(3);          // CONFIRMED

    check("RESERVED == 1",   om.countByStatus(OrderStatus::RESERVED)  == 1);
    check("PENDING == 1",    om.countByStatus(OrderStatus::PENDING)   == 1);
    check("CONFIRMED == 1",  om.countByStatus(OrderStatus::CONFIRMED) == 1);
    check("REJECTED == 0",   om.countByStatus(OrderStatus::REJECTED)  == 0);

    std::cout << "  [출력 확인]\n";
    Monitor mon(pm, om);
    mon.showOrderSummary();
}

static void testInventoryStatus() {
    std::cout << "\n--- showInventoryStatus ---\n";
    ProductManager pm;
    pm.addProduct("Si-A", 100);  // 충분
    pm.addProduct("Si-B",   5);  // 부족 예정
    pm.addProduct("Si-C",   0);  // 고갈

    OrderManager om(pm);
    om.reserveOrder("A", 2, 20); // demand 20 > stock 5 → 부족
    // Si-A demand 0 < stock 100 → 여유
    // Si-C stock 0 → 고갈

    check("Si-A stock == 100", pm.findById(1)->getStock() == 100);
    check("Si-B stock == 5",   pm.findById(2)->getStock() == 5);
    check("Si-C stock == 0",   pm.findById(3)->getStock() == 0);

    std::cout << "  [출력 확인]\n";
    Monitor mon(pm, om);
    mon.showInventoryStatus();
}

int main() {
    std::cout << "=== Phase 7 Test: Monitor ===\n";
    testOrderSummary();
    testInventoryStatus();
    std::cout << "\n============================\n";
    std::cout << "PASS: " << passed << " / FAIL: " << failed << "\n";
    return failed == 0 ? 0 : 1;
}
