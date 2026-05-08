#include "ProductionLine.h"
#include <iostream>
using namespace std;

ProductionLine::ProductionLine(OrderManager& om) : orderManager_(om) {}

void ProductionLine::enqueue(int orderId) {
    waitingQueue_.push(orderId);
}

bool ProductionLine::startNext() {
    if (producingOrderId_.has_value() || waitingQueue_.empty()) return false;
    int id = waitingQueue_.front();
    waitingQueue_.pop();
    Order* o = orderManager_.findById(id);
    if (!o || o->getStatus() != OrderStatus::CONFIRMED) return false;
    o->setStatus(OrderStatus::PRODUCING);
    producingOrderId_ = id;
    return true;
}

bool ProductionLine::completeProducing() {
    if (!producingOrderId_.has_value()) return false;
    Order* o = orderManager_.findById(*producingOrderId_);
    if (!o || o->getStatus() != OrderStatus::PRODUCING) return false;
    o->setStatus(OrderStatus::RELEASE);
    producingOrderId_.reset();
    return true;
}

bool ProductionLine::isProducing() const {
    return producingOrderId_.has_value();
}

optional<int> ProductionLine::getProducingOrderId() const {
    return producingOrderId_;
}

const queue<int>& ProductionLine::getWaitingQueue() const {
    return waitingQueue_;
}

void ProductionLine::setProducingOrderId(optional<int> id) {
    producingOrderId_ = id;
}

void ProductionLine::showStatus() const {
    if (producingOrderId_.has_value()) {
        const Order* o = orderManager_.findById(*producingOrderId_);
        cout << "  [생산 중] "
                  << (o ? o->toString() : "ID:" + to_string(*producingOrderId_)) << "\n";
    } else {
        cout << "  [생산 중] 없음\n";
    }

    if (waitingQueue_.empty()) {
        cout << "  [대기 큐] 없음\n";
        return;
    }

    auto q = waitingQueue_;
    int  pos = 1;
    cout << "  [대기 큐]\n";
    while (!q.empty()) {
        int id = q.front(); q.pop();
        const Order* o = orderManager_.findById(id);
        cout << "    " << pos++ << ". "
                  << (o ? o->toString() : "ID:" + to_string(id)) << "\n";
    }
}

void ProductionLine::clearAll() {
    waitingQueue_ = queue<int>{};
    producingOrderId_.reset();
}
