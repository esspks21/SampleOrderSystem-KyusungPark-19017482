// Phase 1 테스트 하네스 — Product, Order, OrderStatus 기본 동작 검증
// 컴파일: cl /std:c++20 /I..\SampleOrderSystem-KyusungPark-19017482 test_phase1.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\Product.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\Order.cpp

#include <iostream>
#include <string>
#include <cassert>
#include "../SampleOrderSystem-KyusungPark-19017482/Product.h"
#include "../SampleOrderSystem-KyusungPark-19017482/Order.h"
#include "../SampleOrderSystem-KyusungPark-19017482/OrderStatus.h"

static int passed = 0;
static int failed = 0;

static void check(const std::string& label, bool condition) {
    if (condition) {
        std::cout << "[PASS] " << label << "\n";
        ++passed;
    } else {
        std::cout << "[FAIL] " << label << "\n";
        ++failed;
    }
}

static void testProduct() {
    std::cout << "\n--- Product ---\n";
    Product p(1, "Si-Wafer-A", 100);

    check("getId() == 1",          p.getId() == 1);
    check("getName() == Si-Wafer-A", p.getName() == "Si-Wafer-A");
    check("getStock() == 100",     p.getStock() == 100);

    p.setStock(80);
    check("setStock(80)",          p.getStock() == 80);

    std::string s = p.toString();
    check("toString() contains ID", s.find("1") != std::string::npos);
    check("toString() contains name", s.find("Si-Wafer-A") != std::string::npos);
    check("toString() contains stock", s.find("80") != std::string::npos);
}

static void testOrder() {
    std::cout << "\n--- Order ---\n";
    Order o(10, "KyusungPark", 1, 5);

    check("getId() == 10",             o.getId() == 10);
    check("getCustomerName()",         o.getCustomerName() == "KyusungPark");
    check("getProductId() == 1",       o.getProductId() == 1);
    check("getQuantity() == 5",        o.getQuantity() == 5);
    check("initial status RESERVED",   o.getStatus() == OrderStatus::RESERVED);

    o.setStatus(OrderStatus::PENDING);
    check("setStatus(PENDING)",        o.getStatus() == OrderStatus::PENDING);

    o.setStatus(OrderStatus::CONFIRMED);
    check("setStatus(CONFIRMED)",      o.getStatus() == OrderStatus::CONFIRMED);

    std::string s = o.toString();
    check("toString() contains ID",    s.find("10") != std::string::npos);
    check("toString() contains customer", s.find("KyusungPark") != std::string::npos);
    check("toString() contains status",   s.find("CONFIRMED") != std::string::npos);
}

static void testStatusToString() {
    std::cout << "\n--- statusToString ---\n";
    check("RESERVED",  statusToString(OrderStatus::RESERVED)  == "RESERVED");
    check("PENDING",   statusToString(OrderStatus::PENDING)   == "PENDING");
    check("CONFIRMED", statusToString(OrderStatus::CONFIRMED) == "CONFIRMED");
    check("PRODUCING", statusToString(OrderStatus::PRODUCING) == "PRODUCING");
    check("RELEASE",   statusToString(OrderStatus::RELEASE)   == "RELEASE");
    check("REJECTED",  statusToString(OrderStatus::REJECTED)  == "REJECTED");
    check("CANCELLED", statusToString(OrderStatus::CANCELLED) == "CANCELLED");
}

int main() {
    std::cout << "=== Phase 1 Test Harness ===\n";

    testProduct();
    testOrder();
    testStatusToString();

    std::cout << "\n============================\n";
    std::cout << "PASS: " << passed << " / FAIL: " << failed << "\n";
    return failed == 0 ? 0 : 1;
}
