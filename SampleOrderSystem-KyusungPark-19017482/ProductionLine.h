#pragma once
#include <queue>
#include <optional>
#include "OrderManager.h"

class ProductionLine {
public:
    explicit ProductionLine(OrderManager& om);

    void enqueue(int orderId);
    bool startNext();
    bool completeProducing();
    void showStatus() const;
    bool isProducing() const;
    std::optional<int> getProducingOrderId() const;
    const std::queue<int>& getWaitingQueue() const;

    void setProducingOrderId(std::optional<int> id);

private:
    std::queue<int>    waitingQueue_;
    std::optional<int> producingOrderId_;
    OrderManager&      orderManager_;
};
