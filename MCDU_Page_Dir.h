#pragma once
#include "MCDU_BasePage.h"
#include "MCDU_Controller.h"
#include <iostream>
#include <string>

class MCDU_Page_Dir : public MCDU_BasePage {
private:
    std::string direct_target = "[     ]"; // Điểm đang được chọn để bay thẳng
    bool is_dirty = false;                 // Cờ đánh dấu nếu có điểm mới đang chờ nạp

public:
    MCDU_Page_Dir(MCDU_Controller* ctrl) : MCDU_BasePage("DIR TO", ctrl) {}

    // 1. Vẽ giao diện trang DIR TO đúng chuẩn thiết kế Airbus
    void render() override {
        std::cout << "            " << page_title << "            \n\n";
        std::cout << " WPT\n";
        std::cout << " <" << direct_target << "                        \n"; // LSK 1L để nạp điểm
        std::cout << "------------------------------------\n";
        std::cout << " [ F-PLN ]                      [ ] \n"; // LSK 6L để quay lại Flight Plan
        
        if (is_dirty) {
            std::cout << "                                    \n";
            std::cout << " *INSERT*                         [ ]\n"; // LSK 6R hiện lên khi cần xác nhận
        }
    }

    // 2. Xử lý các nút bấm bên tay trái (LSK 1L -> 6L)
    void handle_lsk_left(int index, const std::string& scratchpad_data) override {
        // LSK 1L: Nhận tên điểm điều hướng từ dòng đáy Scratchpad
        if (index == 1) {
            if (scratchpad_data.empty()) {
                std::cout << "\n[MCDU]: FORMAT ERROR (Dong day dang trong)\n";
                return;
            }
            
            // Tạm thời nhận điểm vào màn hình chờ
            direct_target = scratchpad_data;
            is_dirty = true; // Kích hoạt cờ chờ xác nhận (Insert)
            controller->clear_scratchpad(); // Nhập xong xóa dòng đáy
        }
        
        // LSK 6L: Cho phép quay trở lại trang danh sách đường bay chính
        else if (index == 6) {
            std::cout << "\n[MCDU]: Dang quay lai trang F-PLN...\n";
            // Đoạn này sau này bạn nạp file F-PLN vào: controller->set_page(new MCDU_Page_FPln(controller));
        }
    }

    // 3. Xử lý các nút bấm bên tay phải (LSK 1R -> 6R)
    void handle_lsk_right(int index, const std::string& scratchpad_data) override {
        // LSK 6R: Nút xác nhận bấm lệnh "BAY THẲNG" (INSERT)
        if (index == 6 && is_dirty) {
            std::cout << "\n[FMS LOGIC]: EXECUTE DIRECT TO -> " << direct_target << "\n";
            std::cout << "[FMS LOGIC]: Da xoa toan bo cac diem trung gian truoc " << direct_target << "!\n";
            
            is_dirty = false; // Tắt cờ chờ
            // Sau khi kết nối logic với bộ xử lý đường bay, hệ thống sẽ tự động chuyển về trang F-PLN tại đây
        }
    }
};
