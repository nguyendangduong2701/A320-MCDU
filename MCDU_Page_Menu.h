#ifndef MCDU_PAGE_MENU_H
#define MCDU_PAGE_MENU_H

#include "MCDU_BasePage.h"
#include <string>

class MCDU_Page_Menu : public MCDU_BasePage {
public:
    // Khởi tạo với tiêu đề "MCDU MENU"
    MCDU_Page_Menu(MCDU_Controller* ctrl) : MCDU_BasePage("MCDU MENU", ctrl) {}

    // Vẽ giao diện danh mục hệ thống
    void render() override {
        std::cout << "            " << page_title << "            \n\n";
        std::cout << " <FMGC                         \n"; // 1L -> Dẫn tới A/C Status
        std::cout << " <ATSU                         \n"; // 2L -> Dẫn tới ATC/AOC Menu
        std::cout << " <AIDS                         \n"; // 3L
        std::cout << " <CFDS                         \n"; // 4L
        std::cout << "------------------------------------\n";
        std::cout << "                        NAV B/UP> \n"; // 1R
    }

    // Xử lý chuyển trang khi nhấn LSK trái
    void handle_lsk_left(int index, const std::string& scratchpad) override {
        if (index == 1) {
            // Chuyển sang trang A/C Status (thông qua FMGC)
            // controller->set_page(new MCDU_Page_Data_ACStatus(controller));
        } else if (index == 2) {
            // Chuyển sang trang ATSU / ATC COMM
            // controller->set_page(new MCDU_Page_AtcComm(controller));
        }
    }

    void handle_lsk_right(int index, const std::string& scratchpad) override {
        // Xử lý cho NAV B/UP hoặc các tính năng bên phải khác
    }
};

#endif
