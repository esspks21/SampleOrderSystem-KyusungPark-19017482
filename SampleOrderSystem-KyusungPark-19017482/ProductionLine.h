#pragma once
#include <queue>
#include <optional>
#include "OrderManager.h"
using namespace std;

class ProductionLine {
public:
    explicit ProductionLine(OrderManager& om);

    void enqueue(int orderId);
    bool startNext();
    bool completeProducing();
    void showStatus() const;
    bool isProducing() const;
    optional<int> getProducingOrderId() const;
    const queue<int>& getWaitingQueue() const;

    void setProducingOrderId(optional<int> id);

private:
    queue<int>    waitingQueue_;
    optional<int> producingOrderId_;
    OrderManager&      orderManager_;
};
