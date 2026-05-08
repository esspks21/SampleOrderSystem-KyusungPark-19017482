#pragma once
#include <vector>
#include "Order.h"
#include "ProductManager.h"
using namespace std;

class OrderManager {
public:
    explicit OrderManager(ProductManager& pm);

    bool reserveOrder(const string& customerName, int productId, int quantity);
    bool acceptOrder(int orderId);
    bool cancelOrder(int orderId);
    bool approveOrder(int orderId);
    bool rejectOrder(int orderId);
    bool releaseOrder(int orderId, ProductManager& pm);

    void listOrders() const;
    void listOrdersByStatus(OrderStatus status) const;
    Order* findById(int id);
    const Order* findById(int id) const;
    const vector<Order>& getAll() const;
    int  countByStatus(OrderStatus status) const;

    void restoreOrder(int id, const string& customerName,
                      int productId, int quantity, OrderStatus status,
                      time_t createdAt = 0);
    void setNextId(int id);
    int  getNextId() const;
    void clearAll();

private:
    vector<Order> orders_;
    ProductManager& productManager_;
    int nextId_ = 1;
};
