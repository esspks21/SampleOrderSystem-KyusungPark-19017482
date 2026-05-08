// Phase 6 테스트 하네스 — 출고 처리 (releaseOrder)
// 컴파일: cl /std:c++20 /I..\SampleOrderSystem-KyusungPark-19017482 test_phase6.cpp
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

static int makeConfirmedOrder(OrderManager& om, int productId, int qty) {
    static int cid = 0;
    om.reserveOrder("C" + std::to_string(++cid), productId, qty);
    int id = static_cast<int>(om.getAll().size());
    om.acceptOrder(id);
    om.approveOrder(id);
    return id;
}

static void testReleaseSuccess() {
    std::cout << "\n--- releaseOrder success ---\n";
    ProductManager pm; pm.addProduct("Si-A", 100);
    OrderManager om(pm);
    int id = makeConfirmedOrder(om, 1, 30);

    check("release CONFIRMED",          om.releaseOrder(id, pm));
    check("status == RELEASE",           om.findById(id)->getStatus() == OrderStatus::RELEASE);
    check("stock deducted to 70",        pm.findById(1)->getStock() == 70);
}

static void testReleaseInsufficientStock() {
    std::cout << "\n--- releaseOrder insufficient stock ---\n";
    ProductManager pm; pm.addProduct("Si-A", 10);
    OrderManager om(pm);
    int id = makeConfirmedOrder(om, 1, 50);  // qty > stock

    check("release fails (stock < qty)", !om.releaseOrder(id, pm));
    check("status unchanged (CONFIRMED)", om.findById(id)->getStatus() == OrderStatus::CONFIRMED);
    check("stock unchanged at 10",        pm.findById(1)->getStock() == 10);
}

static void testReleaseWrongStatus() {
    std::cout << "\n--- releaseOrder wrong status ---\n";
    ProductManager pm; pm.addProduct("Si-A", 100);
    OrderManager om(pm);

    om.reserveOrder("Alice", 1, 5); // id=1 RESERVED
    check("release RESERVED fails",  !om.releaseOrder(1, pm));

    om.acceptOrder(1); // → PENDING
    check("release PENDING fails",   !om.releaseOrder(1, pm));
}

int main() {
    std::cout << "=== Phase 6 Test: 출고 처리 (releaseOrder) ===\n";
    testReleaseSuccess();
    testReleaseInsufficientStock();
    testReleaseWrongStatus();
    std::cout << "\n===============================================\n";
    std::cout << "PASS: " << passed << " / FAIL: " << failed << "\n";
    return failed == 0 ? 0 : 1;
}
