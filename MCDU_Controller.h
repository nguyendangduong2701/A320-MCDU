#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "MCDU_BasePage.h"

struct GlobalWaypoint {
    std::string name;
    std::string speed_alt;
    bool is_discontinuity;
};

class MCDU_Controller {
private:
    std::string scratchpad = "";
    MCDU_BasePage* current_page = nullptr;

public:
    // --- FMS SHARED RAM DATABASE ---
    bool is_route_loaded = false;
    std::string global_from_to = "____/____";
    std::string global_flt_num = "________";
    std::string global_cost_index = "___";
    std::string global_crz_fl = "_____";
    
    int global_v1 = 0, global_vr = 0, global_v2 = 0;
    
    std::string global_vor_freq = "115.30", global_vor_id = "HAN";
    std::string global_ils_freq = "110.10", global_ils_id = "I-HAN";
    
    double global_zfw = 62.5, global_fob = 12.4, global_extra = 0.0;
    std::string global_extra_time = "00+00";
    std::vector<GlobalWaypoint> global_route;

    // UI Frames Theme Colors
    const std::string RESET = "\033[0m";
    const std::string CYAN  = "\033[1;36m";
    const std::string WHITE = "\033[1;37m";
    const std::string RED   = "\033[1;31m";

public:
    MCDU_Controller() = default;
    ~MCDU_Controller() { if (current_page) delete current_page; }

    std::string get_scratchpad() const { return scratchpad; }
    MCDU_BasePage* get_current_page_ptr() { return current_page; }

    void set_page(MCDU_BasePage* new_page) {
        if (current_page) delete current_page;
        current_page = new_page;
    }

    // Refresh CRT Display instantly (Zero line-scrolling bug)
    void render_system() {
#ifdef _WIN32
        std::system("cls"); 
#else
        std::system("clear");
#endif
        std::cout << CYAN << "====================================\n" << RESET;
        if (current_page) current_page->render();
        std::cout << CYAN << "------------------------------------\n" << RESET;
        
        // Render Scratchpad at the bottom row
        if (scratchpad.find("ERROR") != std::string::npos || scratchpad.find("RANGE") != std::string::npos) {
            std::cout << RED << scratchpad << "\n" << RESET; // Blink error message in RED
        } else {
            std::cout << WHITE << (scratchpad.empty() ? "[ EMPTY ]" : scratchpad) << "\n" << RESET;
        }
        std::cout << CYAN << "====================================\n" << RESET;
    }

    // Auto-fill core engine based on aircraft Gross Weight
    void trigger_auto_fill_system(const std::string& from_to_input) {
        global_from_to = from_to_input;
        is_route_loaded = true;
        global_flt_num = "HVN" + from_to_input.substr(5, 3);
        global_cost_index = "35";
        global_crz_fl = "FL350";

        double gw = global_zfw + global_fob;
        global_v1 = 130 + static_cast<int>((gw - 60) * 0.85);
        global_vr = 132 + static_cast<int>((gw - 60) * 0.90);
        global_v2 = 135 + static_cast<int>((gw - 60) * 0.95);

        global_extra = global_fob - (0.4 + 1.5 + 1.2 + 3.5);
        int total_mins = static_cast<int>(global_extra * 15);
        char buf[16];
        std::snprintf(buf, sizeof(buf), "%02d+%02d", total_mins / 60, total_mins % 60);
        global_extra_time = std::string(buf);

        global_route.clear();
        global_route.push_back({from_to_input.substr(0, 4), "---/---", false});
        global_route.push_back({"DONER", "250/FL100", false});
        global_route.push_back({"[ FLT PLN DISCONTINUITY ]", "-------", true});
        global_route.push_back({from_to_input.substr(5, 4), "---/---", false});
    }

    void type_char(const std::string& text) {
        if (scratchpad.find("ERROR") != std::string::npos || scratchpad.find("RANGE") != std::string::npos) {
            scratchpad = ""; // Typing anything clears the error row
        }
        scratchpad += text;
    }
    
    void press_clr() {
        if (scratchpad.find("ERROR") != std::string::npos || scratchpad.find("RANGE") != std::string::npos) {
            scratchpad = ""; return;
        }
        if (!scratchpad.empty() && scratchpad != "CLR") scratchpad.pop_back();
        else if (scratchpad.empty()) scratchpad = "CLR";
    }
    
    void clear_scratchpad() { scratchpad = ""; }

    void trigger_lsk(char side, int index) {
        if (!current_page) return;
        if (side == 'L') current_page->handle_lsk_left(index, scratchpad);
        else if (side == 'R') current_page->handle_lsk_right(index, scratchpad);
    }
};
