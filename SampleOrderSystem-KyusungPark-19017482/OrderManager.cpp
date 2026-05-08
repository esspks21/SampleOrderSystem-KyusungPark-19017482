#include "OrderManager.h"
#include <iostream>
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

bool OrderManager::approveOrder(int orderId) {
    Order* o = findById(orderId);
    if (!o || o->getStatus() != OrderStatus::PENDING) return false;
    o->setStatus(OrderStatus::CONFIRMED);
    return true;
}

bool OrderManager::rejectOrder(int orderId) {
    Order* o = findById(orderId);
    if (!o || o->getStatus() != OrderStatus::PENDING) return false;
    o->setStatus(OrderStatus::REJECTED);
    return true;
}

void OrderManager::restoreOrder(int id, const string& customerName,
                                int productId, int quantity, OrderStatus status) {
    Order o(id, customerName, productId, quantity);
    o.setStatus(status);
    orders_.push_back(move(o));
}

void OrderManager::setNextId(int id) { nextId_ = id; }
int  OrderManager::getNextId() const { return nextId_; }
