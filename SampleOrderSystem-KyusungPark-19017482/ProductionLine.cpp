#include "ProductionLine.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
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

    // 제품 ID → Product 맵 캐싱 (루프마다 O(N) 검색 → O(1) 조회)
    unordered_map<int, const Product*> productCache;
    for (const auto& prod : productManager_.getAll())
        productCache[prod.getId()] = &prod;

    bool firstShown = false;
    int queuePos = 1;
    for (const Order* o : producing) {
        auto it = productCache.find(o->getProductId());
        const Product* p = (it != productCache.end()) ? it->second : nullptr;
        double durationSec = (p ? p->getAvgProductionTime() : 30.0)
                             * o->getProducingActualQty() * 60.0;
        time_t startTime   = o->getProducingStartedAt();
        time_t endTime     = startTime + static_cast<time_t>(durationSec);

        if (!firstShown && now >= startTime) {
            // 현재 생산 중
            firstShown = true;
            double elapsed     = difftime(now, startTime);
            double ratio       = min(1.0, elapsed / max(1.0, durationSec));
            double progress    = ratio * 100.0;
            int    remaining   = max(0, static_cast<int>((durationSec - elapsed) / 60.0));
            int    totalQty    = o->getProducingActualQty();
            int    producedSoFar = static_cast<int>(ratio * totalQty); // 현재까지 생산된 추정량

            // 수율·공식 근거 표기
            const Product* pr = p;
            double yr  = (pr && pr->getYieldRate() > 0.0) ? pr->getYieldRate() / 100.0 : 0.9;
            int shortage = max(0, o->getQuantity() - (pr ? (pr->getStock() - totalQty) : 0));
            // 공식: 실생산량 = ceil(부족분 / (수율 × 0.9))
            double unitMin = pr ? pr->getAvgProductionTime() : 30.0;

            cout << "\033[1m  ┌─ 생산 중 ────────────────────────────────────────────────\033[0m\n";
            cout << fixed << setprecision(1);
            cout << "  │ 주문ID: " << o->getId()
                 << "  고객: " << o->getCustomerName()
                 << "  시료ID: " << o->getProductId();
            if (pr) cout << " (" << pr->getName() << ")";
            cout << "\n";
            cout << "  │ 주문수량: " << o->getQuantity() << "개"
                 << "   실생산량: " << totalQty << "개"
                 << "  [ceil(부족분 / (수율×0.9)) = ceil(" << shortage
                 << " / (" << (yr * 100) << "% × 0.9))]\n";
            cout << "  │ 단위생산시간: " << unitMin << " min/ea"
                 << "   총생산시간: " << setprecision(0) << (durationSec / 60.0) << " min\n";
            cout << "  │ 진행률: " << setprecision(1) << progress << "%"
                 << "   현재까지 생산량(추정): " << producedSoFar << " / " << totalQty << "개"
                 << "   잔여: " << remaining << "분\n";
            cout << "  │ 시작: " << fmtTime(startTime)
                 << "   완료예정: " << fmtTime(endTime) << "\n";
            cout << "\033[1m  └──────────────────────────────────────────────────────────\033[0m\n";
        } else {
            // 대기 중 (FIFO 큐)
            if (!firstShown) { firstShown = true; cout << "  [생산 중] 없음\n"; }
            if (queuePos == 1)
                cout << "\033[1m  ┌─ 대기 큐 (FIFO) ─────────────────────────────────────────\033[0m\n";
            cout << "  │ " << queuePos++ << ". 주문ID:" << o->getId()
                 << "  고객:" << o->getCustomerName()
                 << "  시료ID:" << o->getProductId()
                 << "  실생산량:" << o->getProducingActualQty() << "개"
                 << "  시작:" << fmtTime(startTime)
                 << "  완료:" << fmtTime(endTime) << "\n";
        }
    }
    if (queuePos == 1) cout << "  [대기 큐] 없음\n";
    else cout << "\033[1m  └──────────────────────────────────────────────────────────\033[0m\n";
}

void ProductionLine::clearAll() {}
