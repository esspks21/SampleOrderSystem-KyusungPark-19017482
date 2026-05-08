#pragma once
#include "ProductManager.h"
#include "OrderManager.h"
#include "ProductionLine.h"
#include "Monitor.h"
#include "FileRepository.h"
#include "PasswordGuard.h"
using namespace std;

class App {
public:
    App();
    void run();
    void forceSave(); // 강제 종료 핸들러에서 호출 (public 필요)

private:
    ProductManager  productManager_;
    OrderManager    orderManager_;
    ProductionLine  productionLine_;
    Monitor         monitor_;
    FileRepository  fileRepository_;   // Database/*.csv 영속성

    void printHeader() const;
    void printSummary() const;
    void printMainMenu() const;

    void runProductMenu();
    void runOrderMenu();
    void runMonitorMenu();
    void runReleaseMenu();
    void runProductionMenu();
    void runResetMenu();               // 비밀번호 인증 후 데이터 초기화

    int  doSync();    // syncProduction() + processAllRelease() + save

    static string getCurrentTime();
    static int    readInt(const string& prompt);
    static string readLine(const string& prompt);
    static string readPassword(const string& prompt);  // 키 입력 마스킹
    static void   clearScreen();
};
