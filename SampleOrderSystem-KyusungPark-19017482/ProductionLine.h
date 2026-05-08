#pragma once
#include <queue>
#include <optional>
#include "OrderManager.h"
using namespace std;

class ProductionLine {
public:
    explicit ProductionLine(OrderManager& om, ProductManager& pm);

    void showStatus() const;  // 생산 현황 표시 (시간 기반 진행률 포함)
    void clearAll();

    // JsonRepository 호환용 (내부 상태 없음 — orders에서 직접 읽음)
    void          enqueue(int) {}
    void          setProducingOrderId(optional<int>) {}
    optional<int> getProducingOrderId() const { return nullopt; }
    queue<int>    getWaitingQueue()     const { return {}; }

private:
    OrderManager&  orderManager_;
    ProductManager& productManager_;
};
