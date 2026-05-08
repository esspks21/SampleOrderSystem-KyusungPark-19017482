#pragma once
#include <string>
#include <ctime>
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
    time_t getCreatedAt() const;

    void setStatus(OrderStatus status);
    void setCreatedAt(time_t t);
    void setProducingStartedAt(time_t t);
    void setProducingActualQty(int qty);

    time_t getProducingStartedAt() const;
    int    getProducingActualQty()  const;

    string toString() const;
    string getCreatedAtStr() const;

private:
    int id_;
    string customerName_;
    int productId_;
    int quantity_;
    OrderStatus status_;
    time_t createdAt_;
    time_t producingStartedAt_ = 0; // 실제 생산 시작 예정 시각
    int    producingActualQty_ = 0; // 실제 생산 수량 (수율·오차 반영)
};
