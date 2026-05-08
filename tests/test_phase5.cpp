// Phase 5 테스트 하네스 — ProductionLine
// 컴파일: cl /std:c++20 /I..\SampleOrderSystem-KyusungPark-19017482 test_phase5.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\Product.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\Order.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\ProductManager.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\OrderManager.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\ProductionLine.cpp

#include <iostream>
#include "../SampleOrderSystem-KyusungPark-19017482/ProductionLine.h"

static int passed = 0, failed = 0;

static void check(const std::string& label, bool cond) {
    if (cond) { std::cout << "[PASS] " << label << "\n"; ++passed; }
    else       { std::cout << "[FAIL] " << label << "\n"; ++failed; }
}

static void makeConfirmedOrder(OrderManager& om, int& outId) {
    static int customerId = 0;
    std::string name = "Customer" + std::to_string(++customerId);
    om.reserveOrder(name, 1, 2);
    outId = om.getAll().back().getId();
    om.acceptOrder(outId);
    om.approveOrder(outId);  // → CONFIRMED
}

static void testStartNext() {
    std::cout << "\n--- startNext ---\n";
    ProductManager pm; pm.addProduct("Si-A", 100);
    OrderManager om(pm);
    ProductionLine pl(om);

    int id1, id2;
    makeConfirmedOrder(om, id1);
    makeConfirmedOrder(om, id2);
    pl.enqueue(id1);
    pl.enqueue(id2);

    check("startNext success",          pl.startNext());
    check("isProducing == true",         pl.isProducing());
    check("producingId == id1",          pl.getProducingOrderId() == id1);
    check("order status == PRODUCING",   om.findById(id1)->getStatus() == OrderStatus::PRODUCING);
    check("startNext while producing fails", !pl.startNext());
}

static void testCompleteProducing() {
    std::cout << "\n--- completeProducing ---\n";
    ProductManager pm; pm.addProduct("Si-A", 100);
    OrderManager om(pm);
    ProductionLine pl(om);

    int id;
    makeConfirmedOrder(om, id);
    pl.enqueue(id);
    pl.startNext();

    check("completeProducing success",   pl.completeProducing());
    check("isProducing == false",         !pl.isProducing());
    check("order status == RELEASE",      om.findById(id)->getStatus() == OrderStatus::RELEASE);
    check("completeProducing again fails",!pl.completeProducing());
}

static void testQueueOrder() {
    std::cout << "\n--- queue FIFO order ---\n";
    ProductManager pm; pm.addProduct("Si-A", 100);
    OrderManager om(pm);
    ProductionLine pl(om);

    int id1, id2, id3;
    makeConfirmedOrder(om, id1);
    makeConfirmedOrder(om, id2);
    makeConfirmedOrder(om, id3);
    pl.enqueue(id1); pl.enqueue(id2); pl.enqueue(id3);

    pl.startNext();
    check("first producing == id1", pl.getProducingOrderId() == id1);
    pl.completeProducing();

    pl.startNext();
    check("second producing == id2", pl.getProducingOrderId() == id2);
    pl.completeProducing();

    pl.startNext();
    check("third producing == id3", pl.getProducingOrderId() == id3);
}

int main() {
    std::cout << "=== Phase 5 Test: ProductionLine ===\n";
    testStartNext();
    testCompleteProducing();
    testQueueOrder();
    std::cout << "\n====================================\n";
    std::cout << "PASS: " << passed << " / FAIL: " << failed << "\n";
    return failed == 0 ? 0 : 1;
}
