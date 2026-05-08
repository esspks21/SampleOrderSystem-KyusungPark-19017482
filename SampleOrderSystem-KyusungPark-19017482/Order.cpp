#include "Order.h"
#include <sstream>

Order::Order(int id, std::string customerName, int productId, int quantity)
    : id_(id), customerName_(std::move(customerName)),
      productId_(productId), quantity_(quantity),
      status_(OrderStatus::RESERVED) {}

int Order::getId() const { return id_; }
const std::string& Order::getCustomerName() const { return customerName_; }
int Order::getProductId() const { return productId_; }
int Order::getQuantity() const { return quantity_; }
OrderStatus Order::getStatus() const { return status_; }

void Order::setStatus(OrderStatus status) {
    status_ = status;
}

std::string Order::toString() const {
    std::ostringstream oss;
    oss << "[ID:" << id_ << "]"
        << " 고객:" << customerName_
        << " 시료ID:" << productId_
        << " 수량:" << quantity_
        << " 상태:" << statusToString(status_);
    return oss.str();
}
