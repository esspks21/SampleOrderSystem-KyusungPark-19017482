#include "ProductionLine.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <cmath>
using namespace std;

static string fmtTime(time_t t) {
    tm local{}; localtime_s(&local, &t);
    ostringstream oss;
    oss << put_time(&local, "%m/%d %H:%M");
    return oss.str();
}

ProductionLine::ProductionLine(OrderManager& om, ProductManager& pm)
    : orderManager_(om), productManager_(pm) {}

void ProductionLine::showStatus() const {
    const auto& orders = orderManager_.getAll();
    time_t now = time(nullptr);

    vector<const Order*> producing;
    for (const auto& o : orders)
        if (o.getStatus() == OrderStatus::PRODUCING && o.getProducingStartedAt() > 0)
            producing.push_back(&o);

    sort(producing.begin(), producing.end(), [](const Order* a, const Order* b) {
        return a->getProducingStartedAt() < b->getProducingStartedAt();
    });

    if (producing.empty()) {
        cout << "  [생산 중] 없음\n  [대기 큐] 없음\n";
        return;
    }

    bool firstShown = false;
    int queuePos = 1;
    for (const Order* o : producing) {
        const Product* p = productManager_.findById(o->getProductId());
        double durationSec = (p ? p->getAvgProductionTime() : 30.0)
                             * o->getProducingActualQty() * 60.0;
        time_t startTime   = o->getProducingStartedAt();
        time_t endTime     = startTime + static_cast<time_t>(durationSec);

        if (!firstShown && now >= startTime) {
            // 현재 생산 중
            firstShown = true;
            double elapsed  = difftime(now, startTime);
            double progress = min(1.0, elapsed / max(1.0, durationSec)) * 100.0;
            int    remaining = max(0, static_cast<int>((durationSec - elapsed) / 60.0));

            cout << "  [생산 중]\n";
            cout << fixed << setprecision(1);
            cout << "    주문ID:" << o->getId()
                 << "  시료ID:" << o->getProductId()
                 << "  생산량:" << o->getProducingActualQty() << "개"
                 << "  진행:" << progress << "%"
                 << "  잔여:" << remaining << "분"
                 << "  완료예정:" << fmtTime(endTime) << "\n";
        } else {
            // 대기 중
            if (!firstShown) { firstShown = true; cout << "  [생산 중] 없음\n"; }
            if (queuePos == 1) cout << "  [대기 큐]\n";
            cout << "    " << queuePos++ << ". 주문ID:" << o->getId()
                 << "  시료ID:" << o->getProductId()
                 << "  생산량:" << o->getProducingActualQty() << "개"
                 << "  시작예정:" << fmtTime(startTime)
                 << "  완료예정:" << fmtTime(endTime) << "\n";
        }
    }
    if (queuePos == 1) cout << "  [대기 큐] 없음\n";
}

void ProductionLine::clearAll() {}
