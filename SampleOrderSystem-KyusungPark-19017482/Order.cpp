#include "Order.h"
#include <sstream>
#include <iomanip>
using namespace std;

Order::Order(int id, string customerName, int productId, int quantity)
    : id_(id), customerName_(move(customerName)),
      productId_(productId), quantity_(quantity),
      status_(OrderStatus::RESERVED),
      createdAt_(time(nullptr)) {}

int getId_impl(int id) { return id; }

int         Order::getId()          const { return id_; }
const string& Order::getCustomerName() const { return customerName_; }
int         Order::getProductId()   const { return productId_; }
int         Order::getQuantity()    const { return quantity_; }
OrderStatus Order::getStatus()      const { return status_; }
time_t      Order::getCreatedAt()   const { return createdAt_; }

void Order::setStatus(OrderStatus status) { status_ = status; }
void Order::setCreatedAt(time_t t)        { createdAt_ = t; }

string Order::getCreatedAtStr() const {
    tm local{};
    localtime_s(&local, &createdAt_);
    ostringstream oss;
    oss << put_time(&local, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

string Order::toString() const {
    ostringstream oss;
    oss << "[ID:" << id_ << "]"
        << " 고객:" << customerName_
        << " 시료ID:" << productId_
        << " 수량:" << quantity_
        << " 상태:" << statusToString(status_)
        << " 접수:" << getCreatedAtStr();
    return oss.str();
}
