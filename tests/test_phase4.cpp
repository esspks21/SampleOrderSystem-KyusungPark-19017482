// Phase 4 테스트 하네스 — OrderManager (approveOrder / rejectOrder)
// 컴파일: cl /std:c++20 /I..\SampleOrderSystem-KyusungPark-19017482 test_phase4.cpp
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

static void makeReservedAndPending(OrderManager& om) {
    om.reserveOrder("Alice", 1, 5);  // id=1
    om.acceptOrder(1);               // → PENDING
}

static void testApproveOrder() {
    std::cout << "\n--- approveOrder ---\n";
    ProductManager pm; pm.addProduct("Si-A", 100);
    OrderManager om(pm);
    makeReservedAndPending(om);

    check("approve PENDING → CONFIRMED", om.approveOrder(1));
    check("status == CONFIRMED",          om.findById(1)->getStatus() == OrderStatus::CONFIRMED);
    check("approve CONFIRMED fails",      !om.approveOrder(1));

    om.reserveOrder("Bob", 1, 3);  // id=2 RESERVED (not yet PENDING)
    check("approve RESERVED fails",       !om.approveOrder(2));
    check("approve invalid id fails",     !om.approveOrder(99));
}

static void testRejectOrder() {
    std::cout << "\n--- rejectOrder ---\n";
    ProductManager pm; pm.addProduct("Si-A", 100);
    OrderManager om(pm);
    makeReservedAndPending(om);

    check("reject PENDING → REJECTED",  om.rejectOrder(1));
    check("status == REJECTED",          om.findById(1)->getStatus() == OrderStatus::REJECTED);
    check("reject REJECTED fails",       !om.rejectOrder(1));

    om.reserveOrder("Bob", 1, 3);  // id=2 RESERVED
    check("reject RESERVED fails",       !om.rejectOrder(2));
    check("reject invalid id fails",     !om.rejectOrder(99));
}

static void testApproveAndRejectMutualExclusion() {
    std::cout << "\n--- approve/reject mutual exclusion ---\n";
    ProductManager pm; pm.addProduct("Si-A", 100);
    OrderManager om(pm);

    om.reserveOrder("A", 1, 1); om.acceptOrder(1); // id=1 PENDING
    om.reserveOrder("B", 1, 1); om.acceptOrder(2); // id=2 PENDING
    om.approveOrder(1);  // → CONFIRMED
    om.rejectOrder(2);   // → REJECTED

    check("CONFIRMED count == 1", om.countByStatus(OrderStatus::CONFIRMED) == 1);
    check("REJECTED count == 1",  om.countByStatus(OrderStatus::REJECTED)  == 1);
    check("PENDING count == 0",   om.countByStatus(OrderStatus::PENDING)   == 0);
}

int main() {
    std::cout << "=== Phase 4 Test: OrderManager (approve/reject) ===\n";
    testApproveOrder();
    testRejectOrder();
    testApproveAndRejectMutualExclusion();
    std::cout << "\n===================================================\n";
    std::cout << "PASS: " << passed << " / FAIL: " << failed << "\n";
    return failed == 0 ? 0 : 1;
}
