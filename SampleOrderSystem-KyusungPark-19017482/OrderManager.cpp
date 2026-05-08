#include "OrderManager.h"
#include <iostream>
#include <algorithm>
#include <cmath>
using namespace std;

OrderManager::OrderManager(ProductManager& pm) : productManager_(pm) {}

bool OrderManager::reserveOrder(const string& customerName, int productId, int quantity) {
    if (!productManager_.findById(productId)) return false;
    if (quantity <= 0) return false;
    orders_.emplace_back(nextId_++, customerName, productId, quantity);
    return true;
}

bool OrderManager::acceptOrder(int orderId) {
    Order* o = findById(orderId);
    if (!o || o->getStatus() != OrderStatus::RESERVED) return false;
    o->setStatus(OrderStatus::PENDING);
    return true;
}

bool OrderManager::cancelOrder(int orderId) {
    Order* o = findById(orderId);
    if (!o) return false;
    OrderStatus s = o->getStatus();
    if (s != OrderStatus::RESERVED && s != OrderStatus::PENDING) return false;
    o->setStatus(OrderStatus::CANCELLED);
    return true;
}

void OrderManager::listOrders() const {
    if (orders_.empty()) {
        cout << "  주문이 없습니다.\n";
        return;
    }
    for (const auto& o : orders_)
        cout << "  " << o.toString() << "\n";
}

void OrderManager::listOrdersByStatus(OrderStatus status) const {
    bool found = false;
    for (const auto& o : orders_) {
        if (o.getStatus() == status) {
            cout << "  " << o.toString() << "\n";
            found = true;
        }
    }
    if (!found) cout << "  해당 상태의 주문이 없습니다.\n";
}

Order* OrderManager::findById(int id) {
    for (auto& o : orders_)
        if (o.getId() == id) return &o;
    return nullptr;
}

const Order* OrderManager::findById(int id) const {
    for (const auto& o : orders_)
        if (o.getId() == id) return &o;
    return nullptr;
}

const vector<Order>& OrderManager::getAll() const { return orders_; }

int OrderManager::countByStatus(OrderStatus status) const {
    int count = 0;
    for (const auto& o : orders_)
        if (o.getStatus() == status) ++count;
    return count;
}

bool OrderManager::releaseOrder(int orderId, ProductManager& pm) {
    Order* o = findById(orderId);
    if (!o || o->getStatus() != OrderStatus::CONFIRMED) return false;
    const Product* p = pm.findById(o->getProductId());
    if (!p || p->getStock() < o->getQuantity()) return false;
    pm.updateStock(o->getProductId(), -o->getQuantity());
    o->setStatus(OrderStatus::RELEASE);
    return true;
}

optional<OrderStatus> OrderManager::approveOrder(int orderId) {
    Order* o = findById(orderId);
    if (!o || o->getStatus() != OrderStatus::PENDING) return nullopt;

    const Product* p = productManager_.findById(o->getProductId());
    if (p && p->getStock() >= o->getQuantity()) {
        o->setStatus(OrderStatus::CONFIRMED);
        return OrderStatus::CONFIRMED;
    }

    // 재고 부족 → 생산 필요
    int shortage = o->getQuantity() - (p ? p->getStock() : 0);
    if (shortage <= 0) shortage = o->getQuantity();
    double yr = (p && p->getYieldRate() > 0.0) ? p->getYieldRate() / 100.0 : 0.9;
    int actualQty = static_cast<int>(ceil(shortage / (yr * 0.9)));
    double durationSec = (p ? p->getAvgProductionTime() : 30.0) * actualQty * 60.0;

    // FIFO: 기존 PRODUCING 주문 중 가장 늦게 끝나는 시각 이후에 시작
    time_t scheduledStart = time(nullptr);
    for (const auto& existing : orders_) {
        if (existing.getStatus() != OrderStatus::PRODUCING) continue;
        const Product* ep = productManager_.findById(existing.getProductId());
        double eyr  = (ep && ep->getYieldRate() > 0.0) ? ep->getYieldRate() / 100.0 : 0.9;
        int    eQty = existing.getProducingActualQty();
        double eDur = (ep ? ep->getAvgProductionTime() : 30.0) * eQty * 60.0;
        time_t eEnd = existing.getProducingStartedAt() + static_cast<time_t>(eDur);
        if (eEnd > scheduledStart) scheduledStart = eEnd;
    }

    o->setStatus(OrderStatus::PRODUCING);
    o->setProducingStartedAt(scheduledStart);
    o->setProducingActualQty(actualQty);
    return OrderStatus::PRODUCING;
}

int OrderManager::syncProduction() {
    time_t now = time(nullptr);
    int completed = 0;

    // producingStartedAt_ 오름차순으로 처리 (FIFO 보장)
    vector<Order*> producing;
    for (auto& o : orders_)
        if (o.getStatus() == OrderStatus::PRODUCING && o.getProducingStartedAt() > 0)
            producing.push_back(&o);
    sort(producing.begin(), producing.end(), [](const Order* a, const Order* b) {
        return a->getProducingStartedAt() < b->getProducingStartedAt();
    });

    for (Order* o : producing) {
        const Product* p = productManager_.findById(o->getProductId());
        double durationSec = (p ? p->getAvgProductionTime() : 30.0)
                             * o->getProducingActualQty() * 60.0;
        time_t completionTime = o->getProducingStartedAt() + static_cast<time_t>(durationSec);

        if (now >= completionTime) {
            productManager_.updateStock(o->getProductId(), o->getProducingActualQty());
            o->setStatus(OrderStatus::CONFIRMED);
            ++completed;
        }
    }
    return completed;
}

bool OrderManager::rejectOrder(int orderId) {
    Order* o = findById(orderId);
    if (!o || o->getStatus() != OrderStatus::PENDING) return false;
    o->setStatus(OrderStatus::REJECTED);
    return true;
}

void OrderManager::restoreOrder(int id, const string& customerName,
                                int productId, int quantity,
                                OrderStatus status, time_t createdAt) {
    Order o(id, customerName, productId, quantity);
    o.setStatus(status);
    if (createdAt != 0) o.setCreatedAt(createdAt);
    orders_.push_back(move(o));
}

void OrderManager::setNextId(int id) { nextId_ = id; }
int  OrderManager::getNextId() const { return nextId_; }

void OrderManager::clearAll() {
    orders_.clear();
    nextId_ = 1;
}
