#include "App.h"
#ifdef _WIN32
#include <windows.h>
using namespace std;
#endif

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    App app;
    app.run();
    return 0;
}
