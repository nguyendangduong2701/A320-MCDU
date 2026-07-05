#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>

#ifdef _WIN32
#include <windows.h>
#endif

#include "MCDU_Controller.h"
#include "MCDU_Page_Menu.h"
#include "MCDU_Page_Init.h"
#include "MCDU_Page_FPln.h"
#include "MCDU_Page_Perf.h"

void EnableAnsiColors() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif
}

int main() {
    EnableAnsiColors();
    MCDU_Controller mcdu;
    
    mcdu.set_page(new MCDU_Page_Menu(&mcdu));
    mcdu.render_system();

    std::string input;
    bool is_running = true;

    while (is_running) {
        std::cout << "PRESS MCDU KEY: ";
        std::getline(std::cin, input);
        std::transform(input.begin(), input.end(), input.begin(), ::toupper);

        if (input == "EXIT") {
            is_running = false;
            break;
        }
        else if (input == "CLR") {
            mcdu.press_clr();
        }
        // --- HARDWARE CONSOLE FUNCTION KEYS ---
        else if (input == "MENU") mcdu.set_page(new MCDU_Page_Menu(&mcdu));
        else if (input == "INIT") mcdu.set_page(new MCDU_Page_Init(&mcdu));
        else if (input == "FPLN") mcdu.set_page(new MCDU_Page_FPln(&mcdu));
        else if (input == "PERF") mcdu.set_page(new MCDU_Page_Perf(&mcdu));
        
        // --- EDGE LSK KEYS DETECTOR (1L -> 6R) ---
        else if (input.length() == 2 && std::isdigit(input[0]) && (input[1] == 'L' || input[1] == 'R')) {
            int index = input[0] - '0';
            char side = input[1];
            if (index >= 1 && index <= 6) {
                mcdu.trigger_lsk(side, index);
            }
        }
        // --- INSERT DATA INTO SCRATCHPAD ---
        else {
            if (!input.empty()) mcdu.type_char(input);
        }

        mcdu.render_system();
    }
    return 0;
}
