#pragma once
#include "MCDU_BasePage.h"
#include "MCDU_Controller.h"
#include <iostream>
#include <string>
#include <vector>

class MCDU_Page_FPln : public MCDU_BasePage {
public:
    MCDU_Page_FPln(MCDU_Controller* ctrl) : MCDU_BasePage("ACT F-PLN", ctrl) {}

    // 1. Vẽ giao diện lộ trình bay lấy dữ liệu trực tiếp từ Controller sang
    void render() override {
        // Tiêu đề trang màu TRẮNG căn giữa
        std::cout << WHITE << "            " << page_title << "            \n\n" << RESET;
        std::cout << WHITE << " FROM FROM                     UTC \n" << RESET;
        std::cout << GREEN << " -PPOS-                       1200 \n" << RESET;
        std::cout << WHITE << "------------------------------------\n" << RESET;

        // Nếu phi công chưa nhập chặng bay ở INIT A, thông báo đường bay trống
        if (!controller->is_route_loaded || controller->global_route.empty()) {
            std::cout << AMBER << "       [ NO FLIGHT PLAN LOADED ]    \n" << RESET;
            std::cout << WHITE << "       ENTER FROM/TO IN INIT PAGE   \n\n\n\n" << RESET;
        } else {
            // Vòng lặp hiển thị danh sách điểm bay tự động điền từ dữ liệu chung
            for (size_t i = 0; i < controller->global_route.size(); ++i) {
                int lsk_index = i + 1; // Ánh xạ số nút LSK (1L, 2L, 3L...)
                
                if (controller->global_route[i].is_discontinuity) {
                    // Nếu là đoạn đứt gãy: Hiện chữ màu HỔ PHÁCH để cảnh báo phi công xóa đi
                    std::cout << AMBER << " ─── DISCONTINUITY ───\n" << RESET;
                    std::cout << AMBER << " " << controller->global_route[i].name << "       (" << lsk_index << "L)\n" << RESET;
                } else {
                    // Nếu là điểm bay bình thường: Hiện màu XANH LÁ, ép giới hạn tốc độ/độ cao dạt hẳn sang phải
                    std::cout << GREEN << " " << controller->global_route[i].name << "\n" << RESET;
                    std::cout << WHITE << " [ ]                         " << GREEN << controller->global_route[i].speed_alt << WHITE << " (" << lsk_index << "L)\n" << RESET;
                }
            }
        }

        std::cout << WHITE << "------------------------------------\n" << RESET;
        // Dòng cuối hiển thị Sân bay đích (Destination)
        if (controller->is_route_loaded) {
            std::string dest = controller->global_from_to.substr(5, 4);
            std::cout << WHITE << " <DEST [" << GREEN << dest << WHITE << "]                       [ ]\n" << RESET;
        } else {
            std::cout << WHITE << " <DEST [    ]                       [ ]\n" << RESET;
        }
    }

    // 2. Logic tương tác phím xóa CLR để nối liền mạch đường bay khi phi công bấm xóa đoạn đứt gãy
    void handle_lsk_left(int index, const std::string& scratchpad_data) override {
        size_t vector_index = index - 1;
        if (!controller->is_route_loaded || vector_index >= controller->global_route.size()) return;

        // Nếu phi công gõ lệnh "CLR" rồi bấm nút cạnh dòng DISCONTINUITY để nối liền mạch đường bay
        if (scratchpad_data == "CLR" && controller->global_route[vector_index].is_discontinuity) {
            // Xóa đoạn đứt gãy ra khỏi bộ nhớ tổng
            controller->global_route.erase(controller->global_route.begin() + vector_index);
            controller->clear_scratchpad(); // Dọn sạch dòng đáy
            std::cout << "\033[1;32m\n[F-PLN LOGIC]: DISCONTINUITY CLEARED! ROUTE CONNECTED.\n\033[0m";
        }
    }

    void handle_lsk_right(int index, const std::string& scratchpad_data) override {}
};
