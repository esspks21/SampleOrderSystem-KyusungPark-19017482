#pragma once
#include "ProductManager.h"
#include "OrderManager.h"
#include "ProductionLine.h"
#include "Monitor.h"
#include "FileRepository.h"
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
    FileRepository  fileRepository_;

    void printHeader() const;
    void printSummary() const;
    void printMainMenu() const;

    void runProductMenu();
    void runOrderMenu();
    void runMonitorMenu();
    void runReleaseMenu();
    void runProductionMenu();

    static int         readInt(const string& prompt);
    static string readLine(const string& prompt);
};
