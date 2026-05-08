#include "App.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <conio.h>      // _getch(), _kbhit() — 키 입력 감지 (Windows)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>    // Sleep()
#undef CY               // windows.h CY(CURRENCY) 타입과 색상 상수 충돌 방지
using namespace std;

// ── ANSI 색상 코드 + 입력 상수 ───────────────────────────────
namespace {
    const char* CR   = "\033[0m";         // Reset
    const char* BOLD = "\033[1m";         // Bold
    const char* CB   = "\033[38;2;20;40;160m"; // Samsung Blue PMS286C  RGB(20,40,160) #1428A0
    const char* CW   = "\033[97m";        // White
    const char* CY   = "\033[93m";        // Yellow  (시간 강조)
    const char* CC   = "\033[96m";        // Cyan    (메뉴 번호)
    const char* CG   = "\033[92m";        // Green   (성공)
    const char* CRD  = "\033[91m";        // Red     (경고)
    const char* CGR  = "\033[90m";        // Gray    (구분선)

    // 빈 입력(Enter만 누름) 시 readInt가 반환하는 특별값 — 이전 화면으로 복귀
    constexpr int BACK = -9999;
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
      productionLine_(orderManager_, productManager_),
      monitor_(productManager_, orderManager_),
      fileRepository_(productManager_, orderManager_, productionLine_)
{
    fileRepository_.load();
    int completed = orderManager_.syncProduction();
    if (completed > 0) {
        fileRepository_.save();
        cout << CG << "[시스템] 오프라인 중 완료된 생산 " << completed << "건을 반영했습니다." << CR << "\n";
    }
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

        if (choice == BACK) continue;       // 빈 입력 → 메인 메뉴 재표시

        switch (choice) {
        case 1: runProductMenu();    break;
        case 2: runOrderMenu();      break;
        case 3: runMonitorMenu();    break;
        case 4: runReleaseMenu();    break;
        case 5: runProductionMenu(); break;
        case 9: runResetMenu();      break;
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

// ── Samsung 레터마크 + 현재시간 헤더 ─────────────────────
void App::printHeader() const {
    string t = getCurrentTime();

    // SAMSUNG 블록 레터마크 — Samsung Blue RGB(20,40,160) #1428A0
    const char* L[5] = {
        " ███   ██   █   █  ███  █   █ █   █  ████",
        "█     █  █  ██ ██ █     █   █ ██  █ █    ",
        " ███  ████  █ █ █  ███  █   █ █ █ █ █  ██",
        "    █ █  █  █   █     █ █   █ █  ██ █   █",
        " ███  █  █  █   █  ███   ███  █   █  ████"
    };

    cout << "\n";
    for (int i = 0; i < 5; i++)
        cout << "  " << CB << BOLD << L[i] << CR << "\n";

    cout << "\n";
    cout << "       " << CW << BOLD << "반도체 시료 생산주문관리 시스템" << CR << "\n";
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
    cout << "  " << CRD << "9." << CR << "  데이터 초기화  (관리자)\n";
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
        cout << "  " << CC << "4." << CR << " 시료 수정\n";
        cout << "  " << CC << "5." << CR << " 시료 삭제\n";
        cout << "  " << CGR << "0." << CR << " 돌아가기\n\n";
        int c = readInt("선택 > ");
        if (c == 0 || c == BACK) break;
        if (c == 1) {
            string name  = readLine("시료명: ");
            int    stock = readInt("초기 재고: ");

            auto readDouble = [](const string& prompt, double minVal, double maxVal) -> double {
                cout << prompt;
                string line;
                if (!getline(cin, line) || line.empty()) return 0.0;
                double val = 0.0;
                try { val = stod(line); } catch (...) {}
                if (val < minVal) val = minVal;
                if (val > maxVal) val = maxVal;
                // 소수점 1자리로 반올림
                return static_cast<int>(val * 10 + 0.5) / 10.0;
            };

            double ptime = readDouble("평균 생산시간 (분, 예: 12.5): ", 0.0, 99999.9);
            double yield = readDouble("수율 (%, 예: 95.5): ",           0.0,   100.0);

            productManager_.addProduct(name,
                                       stock < 0 ? 0 : stock,
                                       ptime, yield);
            cout << CG << "등록 완료." << CR << "\n";
        } else if (c == 2) {
            productManager_.listProducts(true);
        } else if (c == 3) {
            string kw = readLine("검색어 (ID 또는 이름, 대소문자 무시): ");
            auto res = productManager_.search(kw, true);
            if (res.empty()) {
                cout << CRD << "  검색 결과 없음." << CR << "\n";
            } else {
                cout << "  " << left << setw(6) << "ID"
                     << "\t" << setw(28) << "시료명"
                     << "\t" << setw(16) << "평균생산(min)"
                     << "\t" << setw(10) << "수율(%)"
                     << "\t" << "재고(ea)" << "\n";
                cout << "  " << string(75, '-') << "\n";
                for (const auto& p : res) cout << "  " << p.toString() << "\n";
                cout << "  " << string(75, '-') << "\n";
                cout << "  총 " << res.size() << "개\n";
            }
        } else if (c == 4) {
            productManager_.listProducts(true);
            int id = readInt("수정할 시료 ID: ");
            if (id == BACK) { readLine(""); continue; }
            const Product* target = productManager_.findById(id);
            if (!target) {
                cout << CRD << "  해당 ID의 시료가 없습니다." << CR << "\n";
            } else {
                cout << "  현재 정보: " << target->toString() << "\n";
                cout << CGR << "  (Enter만 누르면 현재 값 유지)\n" << CR;

                // 시료명
                string name = readLine("  시료명 [" + target->getName() + "]: ");
                if (name.empty()) name = target->getName();

                // 재고
                cout << "  재고 [" << target->getStock() << "]: ";
                string sline; getline(cin, sline);
                int stock = sline.empty() ? target->getStock()
                                          : (stoi(sline) < 0 ? 0 : stoi(sline));

                // 평균 생산시간
                auto readDbl = [](const string& prompt, double cur) -> double {
                    cout << prompt;
                    string line; getline(cin, line);
                    if (line.empty()) return cur;
                    double v = 0.0;
                    try { v = stod(line); } catch (...) { return cur; }
                    if (v < 0.0) v = 0.0;
                    return static_cast<int>(v * 10 + 0.5) / 10.0;
                };

                double ptime = readDbl(
                    "  평균 생산시간(분) [" + to_string(target->getAvgProductionTime()) + "]: ",
                    target->getAvgProductionTime());
                double yield = readDbl(
                    "  수율(%) [" + to_string(target->getYieldRate()) + "]: ",
                    target->getYieldRate());

                if (productManager_.updateProduct(id, name, stock, ptime, yield))
                    cout << CG << "  수정 완료." << CR << "\n";
                else
                    cout << CRD << "  수정 실패." << CR << "\n";
            }
        } else if (c == 5) {
            productManager_.listProducts(true);
            int id = readInt("삭제할 시료 ID: ");
            if (id == BACK) { readLine(""); continue; }
            const Product* target = productManager_.findById(id);
            if (!target) {
                cout << CRD << "  해당 ID의 시료가 없습니다." << CR << "\n";
            } else {
                cout << "  " << target->toString() << "\n\n";

                // ── 삭제 가능 여부 검사 ───────────────────────────
                vector<string> reasons;

                // 1. 재고 잔여
                if (target->getStock() > 0)
                    reasons.push_back("재고가 " + to_string(target->getStock()) +
                                      " ea 남아있습니다.");

                // 2. 활성 주문 존재 (RESERVED/PENDING/CONFIRMED/PRODUCING)
                const vector<OrderStatus> activeStates = {
                    OrderStatus::RESERVED, OrderStatus::PENDING,
                    OrderStatus::CONFIRMED, OrderStatus::PRODUCING
                };
                int activeCount = 0;
                for (const auto& o : orderManager_.getAll()) {
                    if (o.getProductId() != id) continue;
                    for (auto s : activeStates) {
                        if (o.getStatus() == s) { activeCount++; break; }
                    }
                }
                if (activeCount > 0)
                    reasons.push_back("진행 중인 주문이 " + to_string(activeCount) +
                                      "건 있습니다 (RESERVED/PENDING/CONFIRMED/PRODUCING).");

                // 3. 현재 생산 라인에서 생산 중
                auto prodId = productionLine_.getProducingOrderId();
                if (prodId.has_value()) {
                    const Order* po = orderManager_.findById(*prodId);
                    if (po && po->getProductId() == id)
                        reasons.push_back("현재 생산 라인에서 생산 중입니다.");
                }

                if (!reasons.empty()) {
                    cout << CRD << BOLD << "  ✗ 삭제 불가 — 아래 사유를 확인하세요.\n" << CR;
                    for (const auto& r : reasons)
                        cout << CRD << "    • " << r << CR << "\n";
                } else {
                    string confirm = readLine(
                        string(CRD) + "  정말 삭제하시겠습니까? (y/N): " + CR);
                    if (confirm == "y" || confirm == "Y") {
                        productManager_.deleteProduct(id);
                        cout << CG << "  삭제 완료." << CR << "\n";
                    } else {
                        cout << CGR << "  취소되었습니다." << CR << "\n";
                    }
                }
            }
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
        cout << "  " << CC << "3." << CR << " 주문 승인       (PENDING → CONFIRMED 또는 PRODUCING)\n";
        cout << "  " << CC << "4." << CR << " 주문 거절       (PENDING → REJECTED)\n";
        cout << "  " << CC << "5." << CR << " 주문 취소\n";
        cout << "  " << CC << "6." << CR << " 전체 주문 목록\n";
        cout << "  " << CGR << "0." << CR << " 돌아가기\n\n";
        int c = readInt("선택 > ");
        if (c == 0 || c == BACK) break;

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
            auto result = orderManager_.approveOrder(id);
            if (!result) {
                cout << CRD << "승인 실패 (PENDING 상태 주문만 승인 가능)." << CR << "\n";
            } else if (*result == OrderStatus::CONFIRMED) {
                cout << CG << "승인 완료. 재고 충분 → CONFIRMED (출고 대기)." << CR << "\n";
            } else {
                productionLine_.enqueue(id);
                cout << CY << "승인 완료. 재고 부족 → 생산 라인 등록 (PRODUCING)." << CR << "\n";
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

// ── 모니터링 (실시간 자동 갱신) ──────────────────────────────
void App::runMonitorMenu() {
    constexpr int REFRESH_INTERVAL_MS = 3000;
    constexpr int POLL_STEP_MS        = 100;

    while (true) {
        clearScreen();

        // 생산 완료 체크 후 자동 반영
        int synced = orderManager_.syncProduction();
        if (synced > 0) fileRepository_.save();

        // 헤더 + 대시보드
        cout << "\033[38;2;20;40;160m" << "\033[1m"
             << "  ═══════════════════════════════════════════════════════════\n"
             << "    모니터링 대시보드  " << getCurrentTime() << "\n"
             << "  ═══════════════════════════════════════════════════════════\n"
             << "\033[0m\n";

        monitor_.showDashboard();

        cout << "\n\033[90m  [ " << REFRESH_INTERVAL_MS / 1000
             << "초마다 자동 갱신 | 아무 키: 돌아가기 ]\033[0m\n";

        // REFRESH_INTERVAL_MS 동안 키 입력 감시
        for (int elapsed = 0; elapsed < REFRESH_INTERVAL_MS; elapsed += POLL_STEP_MS) {
            if (_kbhit()) { _getch(); return; }
            Sleep(POLL_STEP_MS);
        }
    }
}

// ── 출고 처리 ─────────────────────────────────────────────
void App::runReleaseMenu() {
    while (true) {
        clearScreen();
        cout << CB << BOLD << "[ 출고 처리 ]" << CR << "\n\n";
        cout << "  " << CC << "1." << CR << " 출고 대기 목록 조회  (CONFIRMED)\n";
        cout << "  " << CC << "2." << CR << " 특정 주문 출고\n";
        cout << "  " << CC << "3." << CR << " 전체 자동 출고       (CONFIRMED → RELEASE 일괄 처리)\n";
        cout << "  " << CGR << "0." << CR << " 돌아가기\n\n";
        int c = readInt("선택 > ");
        if (c == 0 || c == BACK) break;

        if (c == 1) {
            orderManager_.listOrdersByStatus(OrderStatus::CONFIRMED);
        } else if (c == 2) {
            orderManager_.listOrdersByStatus(OrderStatus::CONFIRMED);
            int id = readInt("출고할 주문 ID: ");
            if (id == BACK) { readLine(""); continue; }
            if (orderManager_.releaseOrder(id, productManager_)) {
                fileRepository_.save();
                cout << CG << "출고 완료. 재고 차감됨." << CR << "\n";
            } else {
                cout << CRD << "출고 실패 (CONFIRMED 상태 주문 또는 재고 확인)." << CR << "\n";
            }
        } else if (c == 3) {
            int released = orderManager_.processAllRelease(productManager_);
            if (released > 0) {
                fileRepository_.save();
                cout << CG << released << "건 출고 완료." << CR << "\n";
            } else {
                cout << CY << "출고 처리할 CONFIRMED 주문이 없습니다." << CR << "\n";
            }
        }
        readLine("");
    }
}

// ── 생산 라인 ─────────────────────────────────────────────
void App::runProductionMenu() {
    while (true) {
        clearScreen();
        cout << CB << BOLD << "[ 생산 라인 ]" << CR << "\n\n";
        cout << "  " << CC << "1." << CR << " 생산 현황 조회\n";
        cout << "  " << CGR << "0." << CR << " 돌아가기\n\n";
        int c = readInt("선택 > ");
        if (c == 0 || c == BACK) break;
        if (c == 1) {
            int synced = orderManager_.syncProduction();
            if (synced > 0) {
                fileRepository_.save();
                cout << CG << "생산 완료 " << synced << "건 반영됨.\n" << CR;
            }
            productionLine_.showStatus();
        }
        readLine("");
    }
}

// ── 입력 유틸리티 ─────────────────────────────────────────
int App::readInt(const string& prompt) {
    cout << prompt;
    string line;
    if (!getline(cin, line)) return BACK;
    if (line.empty()) return BACK;          // 빈 입력 → 이전 화면
    try { return stoi(line); }
    catch (...) { return -1; }
}

string App::readLine(const string& prompt) {
    if (!prompt.empty()) cout << prompt;
    string line;
    getline(cin, line);
    return line;
}

// ── 비밀번호 입력 — 키 입력을 * 로 마스킹 ──────────────────
string App::readPassword(const string& prompt) {
    cout << prompt;
    string pw;
    int ch;
    while ((ch = _getch()) != '\r' && ch != '\n') {
        if (ch == '\b') {           // 백스페이스
            if (!pw.empty()) {
                cout << "\b \b";
                pw.pop_back();
            }
        } else if (ch >= 32 && ch < 127) {
            cout << '*';
            pw += static_cast<char>(ch);
        }
    }
    cout << "\n";
    return pw;
}

// ── 데이터 초기화 (관리자 전용) ───────────────────────────
void App::runResetMenu() {
    clearScreen();
    cout << CRD << BOLD << "⚠  누적 데이터 초기화  ⚠" << CR << "\n\n";
    cout << "  시료 · 주문 · 생산 이력이 모두 영구 삭제됩니다.\n";
    cout << "  이 작업은 되돌릴 수 없습니다.\n\n";
    cout << CY << "  🔑 비밀번호 힌트 :  " << CR
         << PasswordGuard::hint() << "\n\n";

    string pw = readPassword("  비밀번호 입력 : ");

    if (!PasswordGuard::verify(pw)) {
        cout << "\n" << CRD << "  ✗ 비밀번호가 틀렸습니다." << CR << "\n";
        readLine("");
        return;
    }

    cout << "\n" << CY << "  정말로 초기화하시겠습니까? (y/N) : " << CR;
    string confirm = readLine("");
    if (confirm != "y" && confirm != "Y") {
        cout << CGR << "  취소되었습니다." << CR << "\n";
        readLine("");
        return;
    }

    fileRepository_.clearAll();
    cout << "\n" << CG << BOLD << "  ✓ 모든 데이터가 초기화되었습니다." << CR << "\n";
    readLine("");
}
