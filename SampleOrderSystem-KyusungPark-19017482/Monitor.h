#pragma once
#include "ProductManager.h"
#include "OrderManager.h"

class Monitor {
public:
    Monitor(const ProductManager& pm, const OrderManager& om);
    void showOrderSummary() const;
    void showInventoryStatus() const;

private:
    const ProductManager& productManager_;
    const OrderManager&   orderManager_;

    int activeDemandForProduct(int productId) const;
};
