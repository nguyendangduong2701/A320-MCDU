#pragma once
#include "MCDU_BasePage.h"
#include "MCDU_Controller.h"
#include <iostream>
#include <string>
#include <vector>

class MCDU_Page_SecFPln : public MCDU_BasePage {
private:
    std::string sec_from_to = "____/____";
    std::vector<std::string> sec_route; // Danh sách các điểm của đường bay phụ
    bool has_copied = false;            // Cờ kiểm tra xem đã copy lộ trình chính sang chưa

public:
    MCDU_Page_SecFPln(MCDU_Controller* ctrl) : MCDU_BasePage("SEC F-PLN", ctrl) {}

    // 1. Vẽ giao diện SEC F-PLN đúng chuẩn hiển thị màu trắng/hổ phách của Airbus
    void render() override {
        std::cout << "            " << page_title << "            \n\n";
        
        if (!has_copied && sec_route.empty()) {
            // Giao diện khi trang phụ đang trống rỗng
            std::cout << " <COPY ACTIVE                  [INIT]>\n"; // LSK 1L để copy nhanh từ đường bay chính
            std::cout << "                                      \n";
            std::cout << " <SEC INDEX                           \n"; // LSK 2L
            std::cout << "                                      \n";
            std::cout << " SEC FROM/TO                          \n";
            std::cout << " [" << sec_from_to << "]                       \n"; // LSK 3L để nhập chặng bay phụ bằng tay
        } else {
            // Giao diện khi đã có lộ trình phụ bên trong
            std::cout << " <ACTIVATE SEC                        \n"; // LSK 1L: KÍCH HOẠT BIẾN THÀNH ĐƯỜNG BAY CHÍNH
            std::cout << "                                      \n";
            std::cout << " --- SEC ROUTE LIST ---               \n";
            std::cout << " FROM: " << sec_from_to << "                   \n";
            for (size_t i = 0; i < sec_route.size(); ++i) {
                std::cout << "  [" << i + 1 << "] " << sec_route[i] << "\n";
            }
            std::cout << "------------------------------------\n";
            std::cout << " <DELETE SEC                          \n"; // LSK 6L để xóa sạch làm lại từ đầu
        }
    }

    // 2. Logic xử lý các nút bấm bên tay trái (LSK 1L -> 6L)
    void handle_lsk_left(int index, const std::string& scratchpad_data) override {
        // TRƯỜNG HỢP 1: Trang đang trống
        if (!has_copied && sec_route.empty()) {
            // LSK 1L: Tính năng COPY ACTIVE (Sao chép toàn bộ đường bay chính sang đây để sửa đổi)
            if (index == 1) {
                sec_from_to = "VVTS/VVNB"; // Giả lập lấy từ bộ nhớ F-PLN chính sang
                sec_route.push_back("DONER");
                sec_route.push_back("MESAN");
                has_copied = true;
                std::cout << "\n[SEC F-PLN]: Da sao chep lo trinh ACTIVE sang lo trinh SECONDARY!\n";
            }
            // LSK 3L: Nhập chặng bay mới bằng tay (Ví dụ: VVNB/VVDN)
            else if (index == 3) {
                if (scratchpad_data.find('/') != std::string::npos && scratchpad_data.length() == 9) {
                    sec_from_to = scratchpad_data;
                    sec_route.push_back("WAYPOINT-A"); // Tạo 1 điểm giả lập cho chặng bay mới
                    has_copied = true;
                    controller->clear_scratchpad();
                } else {
                    std::cout << "\n[MCDU]: FORMAT ERROR (VD dung: VVNB/VVDN)\n";
                }
            }
        }
        // TRƯỜNG HỢP 2: Đã có dữ liệu lộ trình phụ
        else {
            // LSK 1L: ACTIVATE SEC (Kích hoạt lộ trình phụ đè lên lộ trình chính)
            if (index == 1) {
                std::cout << "\n[FMS LOGIC]: !!! ACTIVATE SECONDARY FLIGHT PLAN !!!\n";
                std::cout << "[FMS LOGIC]: Lo trinh phu da bien thanh lo trinh chinh dang bay.\n";
                
                // Sau khi kích hoạt thành công, Airbus sẽ tự động đưa phi công quay lại trang F-PLN chính
                // logic: controller->set_page(new MCDU_Page_FPln(controller));
            }
            // LSK 6L: DELETE SEC (Xóa bản nháp đường bay phụ này đi)
            else if (index == 6) {
                sec_from_to = "____/____";
                sec_route.clear();
                has_copied = false;
                std::cout << "\n[SEC F-PLN]: Da xoa sach ban nhap duong bay phu.\n";
            }
        }
    }

    // Trang SEC F-PLN cơ bản chủ yếu tương tác hàng cột trái để quản lý bản nháp
    void handle_lsk_right(int index, const std::string& scratchpad_data) override {}
};
