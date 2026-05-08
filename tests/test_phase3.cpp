// Phase 3 테스트 하네스 — OrderManager (reserveOrder / acceptOrder / cancelOrder)
// 컴파일: cl /std:c++20 /I..\SampleOrderSystem-KyusungPark-19017482 test_phase3.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\Product.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\Order.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\ProductManager.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\OrderManager.cpp

#include <iostream>
#include "../SampleOrderSystem-KyusungPark-19017482/OrderManager.h"

static int passed = 0, failed = 0;

static void check(const std::string& label, bool cond) {
    if (cond) { std::cout << "[PASS] " << label << "\n"; ++passed; }
    else       { std::cout << "[FAIL] " << label << "\n"; ++failed; }
}

static void testReserveOrder() {
    std::cout << "\n--- reserveOrder ---\n";
    ProductManager pm;
    pm.addProduct("SiO2", 50);
    OrderManager om(pm);

    check("valid reserve",          om.reserveOrder("Alice", 1, 5));
    check("initial status RESERVED", om.findById(1)->getStatus() == OrderStatus::RESERVED);
    check("invalid productId fails", !om.reserveOrder("Bob", 99, 5));
    check("zero quantity fails",     !om.reserveOrder("Carol", 1, 0));
    check("negative quantity fails", !om.reserveOrder("Dave", 1, -1));
}

static void testAcceptOrder() {
    std::cout << "\n--- acceptOrder ---\n";
    ProductManager pm; pm.addProduct("SiO2", 50);
    OrderManager om(pm);
    om.reserveOrder("Alice", 1, 5);

    check("accept RESERVED → PENDING", om.acceptOrder(1));
    check("status == PENDING",          om.findById(1)->getStatus() == OrderStatus::PENDING);
    check("accept PENDING fails",       !om.acceptOrder(1));
    check("invalid id fails",           !om.acceptOrder(99));
}

static void testCancelOrder() {
    std::cout << "\n--- cancelOrder ---\n";
    ProductManager pm; pm.addProduct("SiO2", 50);
    OrderManager om(pm);
    om.reserveOrder("Alice", 1, 5); // id=1 RESERVED
    om.reserveOrder("Bob",   1, 3); // id=2 RESERVED
    om.acceptOrder(2);              // id=2 → PENDING

    check("cancel RESERVED",            om.cancelOrder(1));
    check("status == CANCELLED",         om.findById(1)->getStatus() == OrderStatus::CANCELLED);
    check("cancel PENDING",              om.cancelOrder(2));
    check("status == CANCELLED",         om.findById(2)->getStatus() == OrderStatus::CANCELLED);

    om.reserveOrder("Carol", 1, 2); // id=3
    om.acceptOrder(3);
    om.findById(3)->setStatus(OrderStatus::CONFIRMED);
    check("cancel CONFIRMED fails",      !om.cancelOrder(3));
}

static void testCountByStatus() {
    std::cout << "\n--- countByStatus ---\n";
    ProductManager pm; pm.addProduct("SiO2", 50);
    OrderManager om(pm);
    om.reserveOrder("A", 1, 1);
    om.reserveOrder("B", 1, 1);
    om.acceptOrder(1);

    check("RESERVED == 1",   om.countByStatus(OrderStatus::RESERVED)  == 1);
    check("PENDING == 1",    om.countByStatus(OrderStatus::PENDING)   == 1);
    check("CONFIRMED == 0",  om.countByStatus(OrderStatus::CONFIRMED) == 0);
}

int main() {
    std::cout << "=== Phase 3 Test: OrderManager (reserve/accept/cancel) ===\n";
    testReserveOrder();
    testAcceptOrder();
    testCancelOrder();
    testCountByStatus();
    std::cout << "\n=========================================================\n";
    std::cout << "PASS: " << passed << " / FAIL: " << failed << "\n";
    return failed == 0 ? 0 : 1;
}
