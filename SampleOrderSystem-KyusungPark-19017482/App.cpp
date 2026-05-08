#include "App.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
using namespace std;

// ── ANSI 색상 코드 ─────────────────────────────────────────
namespace {
    const char* CR   = "\033[0m";         // Reset
    const char* BOLD = "\033[1m";         // Bold
    const char* CB   = "\033[94m";        // Bright Blue  (Samsung 브랜드 색상)
    const char* CW   = "\033[97m";        // White
    const char* CY   = "\033[93m";        // Yellow  (시간 강조)
    const char* CC   = "\033[96m";        // Cyan    (메뉴 번호)
    const char* CG   = "\033[92m";        // Green   (성공)
    const char* CRD  = "\033[91m";        // Red     (경고)
    const char* CGR  = "\033[90m";        // Gray    (구분선)
}

// ── 현재 시간 ─────────────────────────────────────────────
string App::getCurrentTime() {
    auto   now = chrono::system_clock::now();
    time_t t   = chrono::system_clock::to_time_t(now);
    tm     local{};
    localtime_s(&local, &t);
    ostringstream oss;
    oss << put_time(&local, "%Y-%m-%d  %H:%M:%S");
    return oss.str();
}

void App::clearScreen() {
    system("cls");
}

// ── 생성자 ────────────────────────────────────────────────
App::App()
    : orderManager_(productManager_),
      productionLine_(orderManager_),
      monitor_(productManager_, orderManager_),
      fileRepository_(productManager_, orderManager_, productionLine_)
{
    fileRepository_.load();
}

// ── 메인 루프 ─────────────────────────────────────────────
void App::run() {
    while (true) {
        clearScreen();
        printHeader();
        printSummary();
        printMainMenu();

        int choice = readInt(string(CC) + "선택" + CR + " > ");
        cout << "\n";

        switch (choice) {
        case 1: runProductMenu();    break;
        case 2: runOrderMenu();      break;
        case 3: runMonitorMenu();    break;
        case 4: runReleaseMenu();    break;
        case 5: runProductionMenu(); break;
        case 0:
            fileRepository_.save();
            cout << CG << "저장 완료. 시스템을 종료합니다." << CR << "\n";
            return;
        default:
            cout << CRD << "올바른 번호를 입력하세요." << CR << "\n";
        }
        readLine("");   // 아무 키나 누르면 다음으로
    }
}

// ── Samsung 타원 로고 + 현재시간 헤더 ────────────────────
void App::printHeader() const {
    string t = getCurrentTime();

    cout << "\n";

    // Samsung 타원형 로고
    cout << CB << BOLD << "       .──────────────────────────────────────────────────." << CR << "\n";
    cout << CB << BOLD << "     ,'                                                    `." << CR << "\n";
    cout << CB << BOLD << "    /                                                        \\" << CR << "\n";
    cout << CB << BOLD << "   /                                                          \\" << CR << "\n";
    cout << CB << BOLD << "  |   " << CR;
    cout << CW << BOLD  << "S  ·  A  ·  M  ·  S  ·  U  ·  N  ·  G" << CR;
    cout << "   ";
    cout << CB << BOLD  << "Electronics" << CR;
    cout << "   ";
    cout << CB << BOLD << "|" << CR << "\n";
    cout << CB << BOLD << "   \\                                                          /" << CR << "\n";
    cout << CB << BOLD << "    \\                                                        /" << CR << "\n";
    cout << CB << BOLD << "     `.                                                    .'" << CR << "\n";
    cout << CB << BOLD << "       `──────────────────────────────────────────────────'" << CR << "\n";

    cout << "\n";
    cout << "        " << CW << BOLD << "반도체 시료 생산주문관리 시스템" << CR << "\n";
    cout << "\n";
    cout << "  " << CY << BOLD << "⏱  현재시각 :  " << t << "  (KST)" << CR << "\n";
    cout << CGR << "─────────────────────────────────────────────────────────────" << CR << "\n";
}

// ── 요약 정보 ─────────────────────────────────────────────
void App::printSummary() const {
    auto cnt = [&](OrderStatus s) { return orderManager_.countByStatus(s); };

    cout << CGR << "─────────────────────────────────────────────────────────────\n" << CR;
    cout << CW << BOLD << " [시스템 현황]" << CR << "\n";
    cout << "  " << CC << "시료" << CR << " : " << CW << productManager_.getAll().size() << CR;
    cout << "   " << CC << "예약" << CR << " : " << CW << cnt(OrderStatus::RESERVED)  << CR;
    cout << "   " << CC << "대기" << CR << " : " << CW << cnt(OrderStatus::PENDING)   << CR;
    cout << "   " << CC << "확정" << CR << " : " << CY << cnt(OrderStatus::CONFIRMED) << CR;
    cout << "   " << CC << "생산중" << CR << " : " << CY << cnt(OrderStatus::PRODUCING) << CR;
    cout << "   " << CC << "출고" << CR << " : " << CG << cnt(OrderStatus::RELEASE)   << CR;
    cout << "\n";
    cout << CGR << "─────────────────────────────────────────────────────────────\n" << CR;
}

// ── 메인 메뉴 ─────────────────────────────────────────────
void App::printMainMenu() const {
    cout << "\n";
    cout << "  " << CC << BOLD << "1." << CR << "  시료 관리\n";
    cout << "  " << CC << BOLD << "2." << CR << "  주문 관리\n";
    cout << "  " << CC << BOLD << "3." << CR << "  모니터링\n";
    cout << "  " << CC << BOLD << "4." << CR << "  출고 처리\n";
    cout << "  " << CC << BOLD << "5." << CR << "  생산 라인\n";
    cout << "  " << CGR << "0." << CR << "  종료\n";
    cout << "\n";
}

// ── 시료 관리 ─────────────────────────────────────────────
void App::runProductMenu() {
    while (true) {
        clearScreen();
        cout << CB << BOLD << "[ 시료 관리 ]" << CR << "\n\n";
        cout << "  " << CC << "1." << CR << " 시료 등록\n";
        cout << "  " << CC << "2." << CR << " 목록 조회\n";
        cout << "  " << CC << "3." << CR << " 이름 검색\n";
        cout << "  " << CGR << "0." << CR << " 돌아가기\n\n";
        int c = readInt("선택 > ");
        if (c == 0) break;
        if (c == 1) {
            string name = readLine("시료명: ");
            int stock   = readInt("초기 재고: ");
            productManager_.addProduct(name, stock < 0 ? 0 : stock);
            cout << CG << "등록 완료." << CR << "\n";
        } else if (c == 2) {
            productManager_.listProducts();
        } else if (c == 3) {
            string kw = readLine("검색어: ");
            auto res  = productManager_.searchByName(kw);
            if (res.empty()) cout << CRD << "  검색 결과 없음." << CR << "\n";
            else for (const auto& p : res) cout << "  " << p.toString() << "\n";
        }
        readLine("");
    }
}

// ── 주문 관리 ─────────────────────────────────────────────
void App::runOrderMenu() {
    while (true) {
        clearScreen();
        cout << CB << BOLD << "[ 주문 관리 ]" << CR << "\n\n";
        cout << "  " << CC << "1." << CR << " 시료 예약       (→ RESERVED)\n";
        cout << "  " << CC << "2." << CR << " 주문 접수       (RESERVED → PENDING)\n";
        cout << "  " << CC << "3." << CR << " 주문 승인       (PENDING → CONFIRMED)\n";
        cout << "  " << CC << "4." << CR << " 주문 거절       (PENDING → REJECTED)\n";
        cout << "  " << CC << "5." << CR << " 주문 취소\n";
        cout << "  " << CC << "6." << CR << " 전체 주문 목록\n";
        cout << "  " << CGR << "0." << CR << " 돌아가기\n\n";
        int c = readInt("선택 > ");
        if (c == 0) break;

        if (c == 1) {
            productManager_.listProducts();
            int pid      = readInt("시료 ID: ");
            string cname = readLine("고객명: ");
            int qty      = readInt("주문 수량: ");
            cout << (orderManager_.reserveOrder(cname, pid, qty)
                     ? string(CG) + "예약 완료." + CR
                     : string(CRD) + "예약 실패 (시료 ID 확인 또는 수량 > 0)." + CR) << "\n";
        } else if (c == 2) {
            orderManager_.listOrdersByStatus(OrderStatus::RESERVED);
            int id = readInt("접수할 주문 ID: ");
            cout << (orderManager_.acceptOrder(id)
                     ? string(CG) + "접수 완료." + CR
                     : string(CRD) + "접수 실패." + CR) << "\n";
        } else if (c == 3) {
            orderManager_.listOrdersByStatus(OrderStatus::PENDING);
            int id = readInt("승인할 주문 ID: ");
            if (orderManager_.approveOrder(id)) {
                productionLine_.enqueue(id);
                cout << CG << "승인 완료. 생산 큐에 추가됨." << CR << "\n";
            } else {
                cout << CRD << "승인 실패." << CR << "\n";
            }
        } else if (c == 4) {
            orderManager_.listOrdersByStatus(OrderStatus::PENDING);
            int id = readInt("거절할 주문 ID: ");
            cout << (orderManager_.rejectOrder(id)
                     ? string(CG) + "거절 완료." + CR
                     : string(CRD) + "거절 실패." + CR) << "\n";
        } else if (c == 5) {
            orderManager_.listOrders();
            int id = readInt("취소할 주문 ID: ");
            cout << (orderManager_.cancelOrder(id)
                     ? string(CG) + "취소 완료." + CR
                     : string(CRD) + "취소 실패 (RESERVED/PENDING 단계에서만 가능)." + CR) << "\n";
        } else if (c == 6) {
            orderManager_.listOrders();
        }
        readLine("");
    }
}

// ── 모니터링 ──────────────────────────────────────────────
void App::runMonitorMenu() {
    while (true) {
        clearScreen();
        cout << CB << BOLD << "[ 모니터링 ]" << CR << "\n\n";
        cout << "  " << CC << "1." << CR << " 상태별 주문 현황\n";
        cout << "  " << CC << "2." << CR << " 시료별 재고 현황\n";
        cout << "  " << CGR << "0." << CR << " 돌아가기\n\n";
        int c = readInt("선택 > ");
        if (c == 0) break;
        if (c == 1) monitor_.showOrderSummary();
        else if (c == 2) monitor_.showInventoryStatus();
        readLine("");
    }
}

// ── 출고 처리 ─────────────────────────────────────────────
void App::runReleaseMenu() {
    while (true) {
        clearScreen();
        cout << CB << BOLD << "[ 출고 처리 ]" << CR << "\n\n";
        cout << "  " << CC << "1." << CR << " CONFIRMED 주문 출고 (재고 차감)\n";
        cout << "  " << CGR << "0." << CR << " 돌아가기\n\n";
        int c = readInt("선택 > ");
        if (c == 0) break;
        if (c == 1) {
            orderManager_.listOrdersByStatus(OrderStatus::CONFIRMED);
            int id = readInt("출고할 주문 ID: ");
            cout << (orderManager_.releaseOrder(id, productManager_)
                     ? string(CG) + "출고 완료. 재고 차감됨." + CR
                     : string(CRD) + "출고 실패 (재고 부족 또는 상태 오류)." + CR) << "\n";
        }
        readLine("");
    }
}

// ── 생산 라인 ─────────────────────────────────────────────
void App::runProductionMenu() {
    while (true) {
        clearScreen();
        cout << CB << BOLD << "[ 생산 라인 ]" << CR << "\n\n";
        cout << "  " << CC << "1." << CR << " 생산 현황 보기\n";
        cout << "  " << CC << "2." << CR << " 생산 시작  (CONFIRMED → PRODUCING)\n";
        cout << "  " << CC << "3." << CR << " 생산 완료  (PRODUCING → RELEASE)\n";
        cout << "  " << CGR << "0." << CR << " 돌아가기\n\n";
        int c = readInt("선택 > ");
        if (c == 0) break;
        if (c == 1) productionLine_.showStatus();
        else if (c == 2)
            cout << (productionLine_.startNext()
                     ? string(CG) + "생산 시작." + CR
                     : string(CRD) + "실패 (대기 큐 없음 또는 이미 생산 중)." + CR) << "\n";
        else if (c == 3)
            cout << (productionLine_.completeProducing()
                     ? string(CG) + "생산 완료. 출고 처리됨." + CR
                     : string(CRD) + "실패 (생산 중인 주문 없음)." + CR) << "\n";
        readLine("");
    }
}

// ── 입력 유틸리티 ─────────────────────────────────────────
int App::readInt(const string& prompt) {
    cout << prompt;
    string line;
    if (!getline(cin, line)) return -1;
    try { return stoi(line); }
    catch (...) { return -1; }
}

string App::readLine(const string& prompt) {
    if (!prompt.empty()) cout << prompt;
    string line;
    getline(cin, line);
    return line;
}
