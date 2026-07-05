#pragma once
#include "MCDU_BasePage.h"
#include "MCDU_Controller.h"
#include <iostream>
#include <string>

class MCDU_Page_RadNav : public MCDU_BasePage {
private:
    // Các thông số đài vô tuyến mặc định do máy tính tự động dò (Auto-tuned)
    std::string vor1_freq = "115.30";
    std::string vor1_id = "HAN";
    bool is_vor1_auto = true; // Cờ đánh dấu đài VOR1 đang tự động hay chỉnh tay

    std::string ils_freq = "110.10";
    std::string ils_id = "I-HAN";
    std::string ils_crs = "113"; // Hướng đường băng (Course)
    bool is_ils_auto = true;  // Cờ đánh dấu đài ILS đang tự động hay chỉnh tay

    std::string adf1_freq = "375";
    bool is_adf1_auto = true;

public:
    MCDU_Page_RadNav(MCDU_Controller* ctrl) : MCDU_BasePage("RADIO NAV", ctrl) {}

    // 1. Vẽ giao diện RAD NAV chuẩn Airbus (Chữ Auto sẽ viết nhỏ hoặc ghi kèm định dạng)
    void render() override {
        std::cout << "            " << page_title << "            \n\n";
        
        // Dòng VOR 1 (LSK 1L)
        std::cout << " VOR1 / FREQ\n";
        std::cout << " " << vor1_id << " / " << vor1_freq 
                  << (is_vor1_auto ? " (A)" : " (M)") << "                \n"; 
        
        std::cout << "------------------------------------\n";
        
        // Dòng ILS (LSK 3L)
        std::cout << " ILS / FREQ      CRS\n";
        std::cout << " " << ils_id << " / " << ils_freq << "   " << ils_crs 
                  << (is_ils_auto ? " (A)" : " (M)") << "                \n";
                  
        std::cout << "------------------------------------\n";
        
        // Dòng ADF 1 (LSK 5L)
        std::cout << " ADF1 / FREQ\n";
        std::cout << " " << adf1_freq << (is_adf1_auto ? " (A)" : " (M)") << "                        \n";
    }

    // 2. Logic xử lý các nút bấm bên tay trái (LSK 1L -> 6L) - Nhập liệu đè tần số
    void handle_lsk_left(int index, const std::string& scratchpad_data) override {
        // LSK 1L: Nhập đè tần số VOR 1 (Ví dụ nhập: 114.50 hoặc tên đài "TSN")
        if (index == 1) {
            if (scratchpad_data.empty()) return;
            
            // Nếu phi công gõ lệnh "CLR" -> Reset về chế độ Tự động dò đài (Auto-tune)
            if (scratchpad_data == "CLR") {
                vor1_freq = "115.30";
                vor1_id = "HAN";
                is_vor1_auto = true;
                std::cout << "\n[RAD NAV]: VOR1 tra ve che do Tu dong do dai (Auto-tuned).\n";
            } else {
                // Ép sang chế độ chỉnh tay (Manual)
                vor1_freq = scratchpad_data;
                vor1_id = "MAN"; // Đổi tên hiển thị thành chế độ chỉnh tay
                is_vor1_auto = false;
                std::cout << "\n[RAD NAV]: Da ep tan so VOR1 bang tay: " << scratchpad_data << "\n";
            }
            controller->clear_scratchpad();
        }
        
        // LSK 3L: Nhập đè tần số ILS để hạ cánh khóa tự động vào đường băng
        else if (index == 3) {
            if (scratchpad_data.empty()) return;

            if (scratchpad_data == "CLR") {
                ils_freq = "110.10";
                ils_id = "I-HAN";
                is_ils_auto = true;
                std::cout << "\n[RAD NAV]: ILS tra ve che do Tu dong.\n";
            } else {
                ils_freq = scratchpad_data;
                ils_id = "I-MAN";
                is_ils_auto = false;
                std::cout << "\n[RAD NAV]: Da ep tan so ILS bang tay: " << scratchpad_data << "\n";
            }
            controller->clear_scratchpad();
        }

        // LSK 5L: Nhập tần số đài ADF
        else if (index == 5) {
            if (scratchpad_data.empty()) return;

            if (scratchpad_data == "CLR") {
                adf1_freq = "375";
                is_adf1_auto = true;
            } else {
                adf1_freq = scratchpad_data;
                is_adf1_auto = false;
            }
            controller->clear_scratchpad();
        }
    }

    // Trang RAD NAV thực tế không sử dụng các nút LSK bên phải cho việc nạp tần số chính
    void handle_lsk_right(int index, const std::string& scratchpad_data) override {}
};
