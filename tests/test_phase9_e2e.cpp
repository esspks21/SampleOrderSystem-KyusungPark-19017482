// Phase 9 E2E 시나리오 테스트 — 전체 흐름 통합 검증
// 예약 → 접수 → 승인 → 생산시작 → 생산완료(RELEASE) 경로
// 예약 → 접수 → 승인 → 출고처리(재고차감 RELEASE) 경로
// 컴파일: cl /std:c++20 /I..\SampleOrderSystem-KyusungPark-19017482 test_phase9_e2e.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\Product.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\Order.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\ProductManager.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\OrderManager.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\ProductionLine.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\Monitor.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\FileRepository.cpp

#include <iostream>
#include "../SampleOrderSystem-KyusungPark-19017482/ProductionLine.h"
#include "../SampleOrderSystem-KyusungPark-19017482/Monitor.h"
#include "../SampleOrderSystem-KyusungPark-19017482/FileRepository.h"

static int passed = 0, failed = 0;

static void check(const std::string& label, bool cond) {
    if (cond) { std::cout << "[PASS] " << label << "\n"; ++passed; }
    else       { std::cout << "[FAIL] " << label << "\n"; ++failed; }
}

static void testProductionPath() {
    std::cout << "\n--- E2E: 예약→접수→승인→생산→출고 (생산 경로) ---\n";
    ProductManager pm; pm.addProduct("Si-A", 50);
    OrderManager om(pm);
    ProductionLine pl(om);

    // 예약
    check("reserveOrder",    om.reserveOrder("Alice", 1, 10));
    check("status RESERVED", om.findById(1)->getStatus() == OrderStatus::RESERVED);

    // 접수
    check("acceptOrder",     om.acceptOrder(1));
    check("status PENDING",  om.findById(1)->getStatus() == OrderStatus::PENDING);

    // 승인 + 큐
    check("approveOrder",    om.approveOrder(1));
    check("status CONFIRMED",om.findById(1)->getStatus() == OrderStatus::CONFIRMED);
    pl.enqueue(1);

    // 생산 시작
    check("startNext",       pl.startNext());
    check("status PRODUCING",om.findById(1)->getStatus() == OrderStatus::PRODUCING);
    check("isProducing",     pl.isProducing());

    // 생산 완료
    check("completeProducing",  pl.completeProducing());
    check("status RELEASE",     om.findById(1)->getStatus() == OrderStatus::RELEASE);
    check("not producing",      !pl.isProducing());
}

static void testDirectReleasePath() {
    std::cout << "\n--- E2E: 예약→접수→승인→직접출고 (재고 차감 경로) ---\n";
    ProductManager pm; pm.addProduct("GaN-B", 100);
    OrderManager om(pm);

    om.reserveOrder("Bob", 1, 30);
    om.acceptOrder(1);
    om.approveOrder(1);

    check("releaseOrder success",  om.releaseOrder(1, pm));
    check("status RELEASE",        om.findById(1)->getStatus() == OrderStatus::RELEASE);
    check("stock deducted to 70",  pm.findById(1)->getStock() == 70);
}

static void testMonitorAfterFlow() {
    std::cout << "\n--- E2E: 모니터링 집계 정확성 ---\n";
    ProductManager pm; pm.addProduct("Si-X", 200);
    OrderManager om(pm);
    ProductionLine pl(om);

    om.reserveOrder("A", 1, 5);                          // RESERVED
    om.reserveOrder("B", 1, 5); om.acceptOrder(2);       // PENDING
    om.reserveOrder("C", 1, 5); om.acceptOrder(3); om.approveOrder(3); pl.enqueue(3); // CONFIRMED
    om.reserveOrder("D", 1, 5); om.acceptOrder(4); om.approveOrder(4); pl.enqueue(4); pl.startNext(); // PRODUCING
    om.reserveOrder("E", 1, 5); om.acceptOrder(5); om.approveOrder(5); om.releaseOrder(5, pm); // RELEASE

    check("RESERVED == 1",   om.countByStatus(OrderStatus::RESERVED)  == 1);
    check("PENDING == 1",    om.countByStatus(OrderStatus::PENDING)   == 1);
    check("CONFIRMED == 1",  om.countByStatus(OrderStatus::CONFIRMED) == 1);
    check("PRODUCING == 1",  om.countByStatus(OrderStatus::PRODUCING) == 1);
    check("RELEASE == 1",    om.countByStatus(OrderStatus::RELEASE)   == 1);

    Monitor mon(pm, om);
    std::cout << "\n  [모니터링 출력]\n";
    mon.showOrderSummary();
    mon.showInventoryStatus();
}

int main() {
    std::cout << "=== Phase 9 E2E Test: 전체 흐름 통합 ===\n";
    testProductionPath();
    testDirectReleasePath();
    testMonitorAfterFlow();
    std::cout << "\n=========================================\n";
    std::cout << "PASS: " << passed << " / FAIL: " << failed << "\n";
    return failed == 0 ? 0 : 1;
}
