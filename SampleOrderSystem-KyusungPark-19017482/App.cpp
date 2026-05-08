#include "App.h"
#include <iostream>

App::App()
    : orderManager_(productManager_),
      productionLine_(orderManager_),
      monitor_(productManager_, orderManager_),
      fileRepository_(productManager_, orderManager_, productionLine_)
{
    fileRepository_.load();
}

void App::run() {
    while (true) {
        printHeader();
        printSummary();
        printMainMenu();

        int choice = readInt("선택 > ");
        std::cout << "\n";

        switch (choice) {
        case 1: runProductMenu();    break;
        case 2: runOrderMenu();      break;
        case 3: runMonitorMenu();    break;
        case 4: runReleaseMenu();    break;
        case 5: runProductionMenu(); break;
        case 0:
            fileRepository_.save();
            std::cout << "저장 완료. 시스템을 종료합니다.\n";
            return;
        default:
            std::cout << "올바른 번호를 입력하세요.\n";
        }
    }
}

void App::printHeader() const {
    std::cout << "\n============================================\n";
    std::cout << "  반도체 시료 생산주문관리 시스템\n";
    std::cout << "============================================\n";
}

void App::printSummary() const {
    std::cout << "[요약]"
              << " 시료:"    << productManager_.getAll().size()
              << "  예약:"   << orderManager_.countByStatus(OrderStatus::RESERVED)
              << "  대기:"   << orderManager_.countByStatus(OrderStatus::PENDING)
              << "  확정:"   << orderManager_.countByStatus(OrderStatus::CONFIRMED)
              << "  생산중:" << orderManager_.countByStatus(OrderStatus::PRODUCING)
              << "  출고:"   << orderManager_.countByStatus(OrderStatus::RELEASE)
              << "\n";
}

void App::printMainMenu() const {
    std::cout << "--------------------------------------------\n";
    std::cout << "1. 시료 관리\n";
    std::cout << "2. 주문 관리\n";
    std::cout << "3. 모니터링\n";
    std::cout << "4. 출고 처리\n";
    std::cout << "5. 생산 라인\n";
    std::cout << "0. 종료\n";
    std::cout << "--------------------------------------------\n";
}

void App::runProductMenu() {
    while (true) {
        std::cout << "\n--- 시료 관리 ---\n";
        std::cout << "1. 시료 등록\n2. 목록 조회\n3. 이름 검색\n0. 돌아가기\n";
        int c = readInt("선택 > ");
        if (c == 0) break;
        if (c == 1) {
            std::string name = readLine("시료명: ");
            int stock = readInt("초기 재고: ");
            productManager_.addProduct(name, stock < 0 ? 0 : stock);
            std::cout << "등록 완료.\n";
        } else if (c == 2) {
            productManager_.listProducts();
        } else if (c == 3) {
            std::string kw = readLine("검색어: ");
            auto res = productManager_.searchByName(kw);
            if (res.empty()) std::cout << "  검색 결과 없음.\n";
            else for (const auto& p : res) std::cout << "  " << p.toString() << "\n";
        }
    }
}

void App::runOrderMenu() {
    while (true) {
        std::cout << "\n--- 주문 관리 ---\n";
        std::cout << "1. 시료 예약 (RESERVED)\n";
        std::cout << "2. 주문 접수 (RESERVED→PENDING)\n";
        std::cout << "3. 주문 승인 (PENDING→CONFIRMED)\n";
        std::cout << "4. 주문 거절 (PENDING→REJECTED)\n";
        std::cout << "5. 주문 취소\n";
        std::cout << "6. 전체 주문 목록\n";
        std::cout << "0. 돌아가기\n";
        int c = readInt("선택 > ");
        if (c == 0) break;

        if (c == 1) {
            productManager_.listProducts();
            int pid = readInt("시료 ID: ");
            std::string cname = readLine("고객명: ");
            int qty = readInt("주문 수량: ");
            std::cout << (orderManager_.reserveOrder(cname, pid, qty)
                          ? "예약 완료.\n"
                          : "예약 실패 (시료 ID 확인 또는 수량 > 0).\n");
        } else if (c == 2) {
            orderManager_.listOrdersByStatus(OrderStatus::RESERVED);
            int id = readInt("접수할 주문 ID: ");
            std::cout << (orderManager_.acceptOrder(id) ? "접수 완료.\n" : "접수 실패.\n");
        } else if (c == 3) {
            orderManager_.listOrdersByStatus(OrderStatus::PENDING);
            int id = readInt("승인할 주문 ID: ");
            if (orderManager_.approveOrder(id)) {
                productionLine_.enqueue(id);
                std::cout << "승인 완료. 생산 큐에 추가됨.\n";
            } else {
                std::cout << "승인 실패.\n";
            }
        } else if (c == 4) {
            orderManager_.listOrdersByStatus(OrderStatus::PENDING);
            int id = readInt("거절할 주문 ID: ");
            std::cout << (orderManager_.rejectOrder(id) ? "거절 완료.\n" : "거절 실패.\n");
        } else if (c == 5) {
            orderManager_.listOrders();
            int id = readInt("취소할 주문 ID: ");
            std::cout << (orderManager_.cancelOrder(id)
                          ? "취소 완료.\n"
                          : "취소 실패 (RESERVED/PENDING 단계에서만 가능).\n");
        } else if (c == 6) {
            orderManager_.listOrders();
        }
    }
}

void App::runMonitorMenu() {
    while (true) {
        std::cout << "\n--- 모니터링 ---\n";
        std::cout << "1. 상태별 주문 현황\n2. 시료별 재고 현황\n0. 돌아가기\n";
        int c = readInt("선택 > ");
        if (c == 0) break;
        if (c == 1) monitor_.showOrderSummary();
        else if (c == 2) monitor_.showInventoryStatus();
    }
}

void App::runReleaseMenu() {
    while (true) {
        std::cout << "\n--- 출고 처리 ---\n";
        std::cout << "1. CONFIRMED 주문 출고 (재고 차감)\n0. 돌아가기\n";
        int c = readInt("선택 > ");
        if (c == 0) break;
        if (c == 1) {
            orderManager_.listOrdersByStatus(OrderStatus::CONFIRMED);
            int id = readInt("출고할 주문 ID: ");
            std::cout << (orderManager_.releaseOrder(id, productManager_)
                          ? "출고 완료. 재고 차감됨.\n"
                          : "출고 실패 (재고 부족 또는 상태 오류).\n");
        }
    }
}

void App::runProductionMenu() {
    while (true) {
        std::cout << "\n--- 생산 라인 ---\n";
        std::cout << "1. 생산 현황 보기\n";
        std::cout << "2. 생산 시작 (CONFIRMED→PRODUCING)\n";
        std::cout << "3. 생산 완료 (PRODUCING→RELEASE)\n";
        std::cout << "0. 돌아가기\n";
        int c = readInt("선택 > ");
        if (c == 0) break;
        if (c == 1) productionLine_.showStatus();
        else if (c == 2)
            std::cout << (productionLine_.startNext()
                          ? "생산 시작.\n"
                          : "실패 (대기 큐 없음 또는 이미 생산 중).\n");
        else if (c == 3)
            std::cout << (productionLine_.completeProducing()
                          ? "생산 완료. 출고 처리됨.\n"
                          : "실패 (생산 중인 주문 없음).\n");
    }
}

int App::readInt(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) return -1;
    try { return std::stoi(line); }
    catch (...) { return -1; }
}

std::string App::readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    return line;
}
