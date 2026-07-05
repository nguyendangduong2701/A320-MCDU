#pragma once
#include "MCDU_BasePage.h"
#include <iostream>
#include <string>

class MCDU_Controller; // Forward declaration fixes circular dependency bugs

class MCDU_Page_Init : public MCDU_BasePage {
public:
    MCDU_Page_Init(MCDU_Controller* ctrl);

    void render() override;
    void handle_lsk_left(int index, const std::string& data) override;
    void handle_lsk_right(int index, const std::string& data) override;
};

#include "MCDU_Controller.h"

inline MCDU_Page_Init::MCDU_Page_Init(MCDU_Controller* ctrl) : MCDU_BasePage("INIT A", ctrl) {}

inline void MCDU_Page_Init::render() {
    std::cout << WHITE << "            " << page_title << "            \n" << RESET;
    std::cout << WHITE << " CO RTE                     FROM/TO\n" << RESET;
    
    if (!controller->is_route_loaded) {
        std::cout << WHITE << " [      ]              " << AMBER << "[" << controller->global_from_to << "]\n" << RESET;
    } else {
        std::cout << WHITE << " [      ]              " << GREEN << " " << controller->global_from_to << "\n" << RESET;
    }
    
    std::cout << WHITE << " ALTNRTE\n" << RESET;
    std::cout << WHITE << " [      ]\n" << RESET;
    std::cout << WHITE << " FLT NUM\n" << RESET;
    std::cout << (controller->is_route_loaded ? GREEN : AMBER) << " [" << controller->global_flt_num << "]\n" << RESET;
    std::cout << "\n";
    std::cout << WHITE << " COST INDEX                 CRZ FL / TEMP\n" << RESET;
    std::cout << (controller->is_route_loaded ? GREEN : WHITE) << " [" << controller->global_cost_index << "]                       [" << controller->global_crz_fl << "]\n" << RESET;
}

inline void MCDU_Page_Init::handle_lsk_left(int index, const std::string& data) {
    if (index == 3 && !data.empty() && data != "CLR") {
        controller->global_flt_num = data;
        controller->clear_scratchpad();
    }
    else if (index == 5 && !data.empty() && data != "CLR") {
        try {
            int ci = std::stoi(data);
            if (ci >= 0 && ci <= 999) {
                controller->global_cost_index = data;
                controller->clear_scratchpad();
            } else { controller->type_char("ENTRY OUT OF RANGE"); }
        } catch (...) { controller->type_char("FORMAT ERROR"); }
    }
}

inline void MCDU_Page_Init::handle_lsk_right(int index, const std::string& data) {
    if (index == 1) {
        // Clear Route System (Resets data fields to Amber)
        if (data == "CLR") {
            controller->is_route_loaded = false;
            controller->global_from_to = "____/____";
            controller->global_flt_num = "________";
            controller->global_cost_index = "___";
            controller->global_crz_fl = "_____";
            controller->global_route.clear();
            controller->clear_scratchpad();
            return;
        }
        // Input validation & Auto-fill activation
        if (!data.empty() && data.find('/') != std::string::npos && data.length() == 9) {
            controller->trigger_auto_fill_system(data);
            controller->clear_scratchpad();
        } else if (!data.empty()) {
            controller->type_char("FORMAT ERROR");
        }
    }
}
