#pragma once
#include "ProductManager.h"
#include "OrderManager.h"
#include "ProductionLine.h"
#include "Monitor.h"
#include "JsonRepository.h"
#include "PasswordGuard.h"
using namespace std;

class App {
public:
    App();
    void run();

private:
    ProductManager  productManager_;
    OrderManager    orderManager_;
    ProductionLine  productionLine_;
    Monitor         monitor_;
    JsonRepository  jsonRepository_;   // Database/*.json 영속성

    void printHeader() const;
    void printSummary() const;
    void printMainMenu() const;

    void runProductMenu();
    void runOrderMenu();
    void runMonitorMenu();
    void runReleaseMenu();
    void runProductionMenu();
    void runResetMenu();               // 비밀번호 인증 후 데이터 초기화

    static string getCurrentTime();
    static int    readInt(const string& prompt);
    static string readLine(const string& prompt);
    static string readPassword(const string& prompt);  // 키 입력 마스킹
    static void   clearScreen();
};
