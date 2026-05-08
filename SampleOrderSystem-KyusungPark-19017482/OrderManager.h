#pragma once
#include <vector>
#include "Order.h"
#include "ProductManager.h"

class OrderManager {
public:
    explicit OrderManager(ProductManager& pm);

    // Phase 3
    bool reserveOrder(const std::string& customerName, int productId, int quantity);
    bool acceptOrder(int orderId);
    bool cancelOrder(int orderId);

    // Phase 4
    bool approveOrder(int orderId);
    bool rejectOrder(int orderId);

    void listOrders() const;
    void listOrdersByStatus(OrderStatus status) const;
    Order* findById(int id);
    const Order* findById(int id) const;
    const std::vector<Order>& getAll() const;
    int  countByStatus(OrderStatus status) const;

    void restoreOrder(int id, const std::string& customerName,
                      int productId, int quantity, OrderStatus status);
    void setNextId(int id);
    int  getNextId() const;

private:
    std::vector<Order> orders_;
    ProductManager& productManager_;
    int nextId_ = 1;
};
