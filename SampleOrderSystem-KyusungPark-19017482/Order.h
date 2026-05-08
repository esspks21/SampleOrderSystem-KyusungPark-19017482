#pragma once
#include <string>
#include "OrderStatus.h"
using namespace std;

class Order {
public:
    Order(int id, string customerName, int productId, int quantity);

    int getId() const;
    const string& getCustomerName() const;
    int getProductId() const;
    int getQuantity() const;
    OrderStatus getStatus() const;
    void setStatus(OrderStatus status);
    string toString() const;

private:
    int id_;
    string customerName_;
    int productId_;
    int quantity_;
    OrderStatus status_;
};
