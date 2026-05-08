// Phase 8 테스트 하네스 — FileRepository (저장·로드 영속성 검증)
// 컴파일: cl /std:c++20 /I..\SampleOrderSystem-KyusungPark-19017482 test_phase8.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\Product.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\Order.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\ProductManager.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\OrderManager.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\ProductionLine.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\FileRepository.cpp

#include <iostream>
#include "../SampleOrderSystem-KyusungPark-19017482/FileRepository.h"

static int passed = 0, failed = 0;

static void check(const std::string& label, bool cond) {
    if (cond) { std::cout << "[PASS] " << label << "\n"; ++passed; }
    else       { std::cout << "[FAIL] " << label << "\n"; ++failed; }
}

static void testSaveAndLoad() {
    std::cout << "\n--- save & load ---\n";

    // --- 저장 단계 ---
    ProductManager pm1; pm1.addProduct("Si-A", 100); pm1.addProduct("GaN-B", 50);
    OrderManager om1(pm1);
    ProductionLine pl1(om1);

    om1.reserveOrder("Alice", 1, 10); // id=1 RESERVED
    om1.acceptOrder(1);               // PENDING
    om1.approveOrder(1);             // CONFIRMED
    pl1.enqueue(1);
    pl1.startNext();                  // PRODUCING

    om1.reserveOrder("Bob", 2, 5);   // id=2 RESERVED

    FileRepository repo1(pm1, om1, pl1);
    repo1.save();

    // --- 로드 단계 (새 인스턴스) ---
    ProductManager pm2;
    OrderManager om2(pm2);
    ProductionLine pl2(om2);
    FileRepository repo2(pm2, om2, pl2);
    repo2.load();

    check("product count == 2",          pm2.getAll().size() == 2);
    check("Si-A stock == 100",           pm2.findById(1) && pm2.findById(1)->getStock() == 100);
    check("GaN-B stock == 50",           pm2.findById(2) && pm2.findById(2)->getStock() == 50);

    check("order count == 2",            om2.getAll().size() == 2);
    check("order1 status == PRODUCING",  om2.findById(1) && om2.findById(1)->getStatus() == OrderStatus::PRODUCING);
    check("order2 status == RESERVED",   om2.findById(2) && om2.findById(2)->getStatus() == OrderStatus::RESERVED);

    check("productionLine producing",    pl2.isProducing());
    check("producingOrderId == 1",       pl2.getProducingOrderId() == 1);
}

int main() {
    std::cout << "=== Phase 8 Test: FileRepository ===\n";
    testSaveAndLoad();
    std::cout << "\n====================================\n";
    std::cout << "PASS: " << passed << " / FAIL: " << failed << "\n";
    return failed == 0 ? 0 : 1;
}
