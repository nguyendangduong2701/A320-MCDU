#pragma once
#include "MCDU_BasePage.h"
#include "MCDU_Controller.h"
#include <iostream>
#include <string>

class MCDU_Page_Perf : public MCDU_BasePage {
public:
    MCDU_Page_Perf(MCDU_Controller* ctrl) : MCDU_BasePage("TAKE OFF PERF", ctrl) {}

    void render() override {
        std::cout << WHITE << "          " << page_title << "             \n\n" << RESET;
        std::cout << WHITE << " V1                            TRANS ALT\n" << RESET;
        
        // Nếu đã kích hoạt tự điền chặng bay, tốc độ sẽ hiện sẵn màu XANH LÁ chuẩn Airbus
        if (controller->is_route_loaded) {
            std::cout << GREEN << "  " << controller->global_v1 << "KT" << RESET << WHITE << "                         10000\n" << RESET;
            std::cout << WHITE << " VR                            FLAPS/THS\n" << RESET;
            std::cout << GREEN << "  " << controller->global_vr << "KT" << RESET << WHITE << "                         1/DN0.5\n" << RESET;
            std::cout << WHITE << " V2                            FLEX TEMP\n" << RESET;
            std::cout << GREEN << "  " << controller->global_v2 << "KT" << RESET << WHITE << "                         55*\n" << RESET;
        } else {
            // Nếu chưa nhập chặng bay, hiện ô trống hổ phách bắt buộc nhập
            std::cout << AMBER << " [   ]" << RESET << WHITE << "                         10000\n" << RESET;
            std::cout << WHITE << " VR                            FLAPS/THS\n" << RESET;
            std::cout << AMBER << " [   ]" << RESET << WHITE << "                         1/DN0.5\n" << RESET;
            std::cout << WHITE << " V2                            FLEX TEMP\n" << RESET;
            std::cout << AMBER << " [   ]" << RESET << WHITE << "                         55*\n" << RESET;
        }
        
        std::cout << "\n\n";
        std::cout << WHITE << "------------------------------------\n" << RESET;
        std::cout << WHITE << " <NEXT PHASE                                \n" << RESET;
    }

    void handle_lsk_left(int index, const std::string& data) override {}
    void handle_lsk_right(int index, const std::string& data) override {}
};
