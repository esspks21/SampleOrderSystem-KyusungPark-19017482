#pragma once
#include <string>
#include "OrderStatus.h"

class Order {
public:
    Order(int id, std::string customerName, int productId, int quantity);

    int getId() const;
    const std::string& getCustomerName() const;
    int getProductId() const;
    int getQuantity() const;
    OrderStatus getStatus() const;
    void setStatus(OrderStatus status);
    std::string toString() const;

private:
    int id_;
    std::string customerName_;
    int productId_;
    int quantity_;
    OrderStatus status_;
};
