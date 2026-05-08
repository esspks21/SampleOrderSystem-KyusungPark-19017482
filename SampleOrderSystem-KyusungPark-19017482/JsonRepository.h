#pragma once
#include "ProductManager.h"
#include "OrderManager.h"
#include "ProductionLine.h"
using namespace std;

// JSON 기반 영속성 — Database/ 폴더에 저장
// regression 테스트는 이 클래스를 사용하지 않으므로 DB 폴더에 영향 없음
class JsonRepository {
public:
    JsonRepository(ProductManager& pm, OrderManager& om, ProductionLine& pl);

    void save() const;
    bool load();
    void clearAll();        // 비밀번호 인증 후 전체 데이터 초기화

private:
    ProductManager& pm_;
    OrderManager&   om_;
    ProductionLine& pl_;

    string dbDir_;
    string fProd_;
    string fOrd_;
    string fLine_;

    void ensureDir() const;
    void saveProducts()   const;
    void saveOrders()     const;
    void saveProduction() const;
    void loadProducts();
    void loadOrders();
    void loadProduction();
};
