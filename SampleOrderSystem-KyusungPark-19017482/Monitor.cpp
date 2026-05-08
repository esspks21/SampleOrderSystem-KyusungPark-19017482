#include "Monitor.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
using namespace std;

namespace {
    const char* CR   = "\033[0m";
    const char* BOLD = "\033[1m";
    const char* CG   = "\033[92m";   // green  — 여유 / RELEASE
    const char* CY   = "\033[93m";   // yellow — 부족 / PRODUCING
    const char* CRD  = "\033[91m";   // red    — 고갈
    const char* CC   = "\033[96m";   // cyan   — 항목 라벨
    const char* CW   = "\033[97m";   // white
    const char* CGR  = "\033[90m";   // gray   — 구분선
    const char* CB   = "\033[38;2;20;40;160m"; // Samsung Blue

    const char* LINE = "  ─────────────────────────────────────────────────────────\n";
}

Monitor::Monitor(const ProductManager& pm, const OrderManager& om)
    : pm_(pm), om_(om) {}

int Monitor::activeDemand(int productId) const {
    int total = 0;
    for (const auto& o : om_.getAll()) {
        OrderStatus s = o.getStatus();
        if (o.getProductId() == productId &&
            (s == OrderStatus::RESERVED  || s == OrderStatus::PENDING ||
             s == OrderStatus::CONFIRMED || s == OrderStatus::PRODUCING))
            total += o.getQuantity();
    }
    return total;
}

// ── 주문 현황 ─────────────────────────────────────────────────
void Monitor::showOrderSection() const {
    const vector<pair<OrderStatus, const char*>> targets = {
        {OrderStatus::RESERVED,  "RESERVED "},
        {OrderStatus::PENDING,   "PENDING  "},
        {OrderStatus::CONFIRMED, "CONFIRMED"},
        {OrderStatus::PRODUCING, "PRODUCING"},
        {OrderStatus::RELEASE,   "RELEASE  "},
    };

    cout << CB << BOLD << "  ▶ 주문 현황" << CR
         << CGR << "  (REJECTED·CANCELLED 제외)" << CR << "\n";
    cout << LINE;
    cout << "  " << left
         << BOLD << setw(14) << "상태" << CR
         << CC   << setw(6)  << "건수" << CR
         << CW   << "주요 주문 (최대 4건)" << CR << "\n";
    cout << LINE;

    for (const auto& [status, label] : targets) {
        int count = om_.countByStatus(status);

        // 상태별 색상
        const char* col = (status == OrderStatus::RELEASE)   ? CG  :
                          (status == OrderStatus::CONFIRMED)  ? CG  :
                          (status == OrderStatus::PRODUCING)  ? CY  : CW;

        cout << "  " << col << BOLD << setw(13) << label << CR
             << "  " << CW  << setw(4)  << count << "건" << CR;

        // 최대 4개 주문 요약
        int shown = 0;
        for (const auto& o : om_.getAll()) {
            if (o.getStatus() != status) continue;
            if (shown == 0) cout << "   ";
            else            cout << " | ";
            cout << CC << "[" << o.getId() << "]" << CR
                 << CW << o.getCustomerName() << CR;
            if (++shown >= 4) { cout << CGR << "..." << CR; break; }
        }
        cout << "\n";
    }
    cout << LINE;
}

// ── 재고 현황 ─────────────────────────────────────────────────
void Monitor::showInventorySection() const {
    const auto& products = pm_.getAll();
    if (products.empty()) {
        cout << "  등록된 시료가 없습니다.\n";
        return;
    }

    // 상태 분류
    vector<const Product*> depleted, shortage, sufficient;
    for (const auto& p : products) {
        int stock  = p.getStock();
        int demand = activeDemand(p.getId());
        if (stock == 0)          depleted.push_back(&p);
        else if (stock < demand) shortage.push_back(&p);
        else                     sufficient.push_back(&p);
    }

    cout << CB << BOLD << "  ▶ 시료 재고 현황" << CR << "\n";
    cout << LINE;
    // 요약
    cout << "  " << CRD << "● 고갈 " << depleted.size()  << "개" << CR << "   "
         << CY  << "● 부족 " << shortage.size()  << "개" << CR << "   "
         << CG  << "● 여유 " << sufficient.size() << "개" << CR
         << CGR << "  (전체 " << products.size() << "개)" << CR << "\n";
    cout << LINE;

    // 헤더
    cout << "  " << BOLD
         << left  << setw(6)  << "ID"
         << "\t"  << setw(26) << "시료명"
         << "\t"  << right << setw(6) << "재고"
         << "\t"  << setw(6) << "수요"
         << "\t"  << left << "상태" << CR << "\n";
    cout << LINE;

    // 고갈 (빨강) 먼저
    for (const Product* p : depleted) {
        int demand = activeDemand(p->getId());
        cout << "  " << CRD
             << left << setw(6) << p->getId()
             << "\t" << setw(26) << p->getName()
             << "\t" << right << setw(6) << p->getStock()
             << "\t" << setw(6) << demand
             << "\t" << BOLD << "[고갈]" << CR << "\n";
    }

    // 부족 (노랑)
    for (const Product* p : shortage) {
        int demand = activeDemand(p->getId());
        cout << "  " << CY
             << left << setw(6) << p->getId()
             << "\t" << setw(26) << p->getName()
             << "\t" << right << setw(6) << p->getStock()
             << "\t" << setw(6) << demand
             << "\t" << BOLD << "[부족]" << CR << "\n";
    }

    // 여유 (초록) — 재고 적은 순으로 상위 10개만 표시
    sort(sufficient.begin(), sufficient.end(), [this](const Product* a, const Product* b) {
        return a->getStock() < b->getStock();
    });
    int shown = 0;
    for (const Product* p : sufficient) {
        if (shown >= 10) break;
        int demand = activeDemand(p->getId());
        cout << "  " << CG
             << left << setw(6) << p->getId()
             << "\t" << setw(26) << p->getName()
             << "\t" << right << setw(6) << p->getStock()
             << "\t" << setw(6) << demand
             << "\t" << "[여유]" << CR << "\n";
        ++shown;
    }
    if ((int)sufficient.size() > shown)
        cout << CGR << "  ... 여유 " << sufficient.size() - shown << "개 생략 (재고 충분)\n" << CR;

    cout << LINE;
}

// ── 통합 대시보드 ──────────────────────────────────────────────
void Monitor::showDashboard() const {
    showOrderSection();
    cout << "\n";
    showInventorySection();
}
