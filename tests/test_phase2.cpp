// Phase 2 테스트 하네스 — ProductManager
// 컴파일: cl /std:c++20 /I..\SampleOrderSystem-KyusungPark-19017482 test_phase2.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\Product.cpp
//         ..\SampleOrderSystem-KyusungPark-19017482\ProductManager.cpp

#include <iostream>
#include <string>
#include "../SampleOrderSystem-KyusungPark-19017482/ProductManager.h"

static int passed = 0, failed = 0;

static void check(const std::string& label, bool cond) {
    if (cond) { std::cout << "[PASS] " << label << "\n"; ++passed; }
    else       { std::cout << "[FAIL] " << label << "\n"; ++failed; }
}

static void testAddAndList() {
    std::cout << "\n--- addProduct / listProducts ---\n";
    ProductManager pm;
    pm.addProduct("SiO2-A", 50);
    pm.addProduct("GaN-B",  30);
    const auto& all = pm.getAll();
    check("count == 2",           all.size() == 2);
    check("first id == 1",        all[0].getId() == 1);
    check("second id == 2",       all[1].getId() == 2);
    check("first name == SiO2-A", all[0].getName() == "SiO2-A");
    check("first stock == 50",    all[0].getStock() == 50);
}

static void testFindById() {
    std::cout << "\n--- findById ---\n";
    ProductManager pm;
    pm.addProduct("Si-A", 10);
    pm.addProduct("Si-B", 20);
    check("findById(1) not null",       pm.findById(1) != nullptr);
    check("findById(2) name == Si-B",   pm.findById(2)->getName() == "Si-B");
    check("findById(99) == null",        pm.findById(99) == nullptr);
}

static void testSearchByName() {
    std::cout << "\n--- searchByName ---\n";
    ProductManager pm;
    pm.addProduct("Si-Wafer-A",  100);
    pm.addProduct("GaN-Wafer-B",  50);
    pm.addProduct("Si-Chip-C",    20);
    auto res = pm.searchByName("Si");
    check("search 'Si' → 2 results", res.size() == 2);
    auto none = pm.searchByName("XYZ");
    check("search 'XYZ' → empty",    none.empty());
}

static void testUpdateStock() {
    std::cout << "\n--- updateStock ---\n";
    ProductManager pm;
    pm.addProduct("Test", 100);
    check("+20 → 120",              pm.updateStock(1,  20) && pm.findById(1)->getStock() == 120);
    check("-50 → 70",               pm.updateStock(1, -50) && pm.findById(1)->getStock() ==  70);
    check("over-deduct fails",      !pm.updateStock(1, -200));
    check("stock unchanged at 70",  pm.findById(1)->getStock() == 70);
    check("invalid id fails",       !pm.updateStock(99, 10));
}

int main() {
    std::cout << "=== Phase 2 Test: ProductManager ===\n";
    testAddAndList();
    testFindById();
    testSearchByName();
    testUpdateStock();
    std::cout << "\n====================================\n";
    std::cout << "PASS: " << passed << " / FAIL: " << failed << "\n";
    return failed == 0 ? 0 : 1;
}
