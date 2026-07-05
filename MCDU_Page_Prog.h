#pragma once
#include "MCDU_BasePage.h"
#include "MCDU_Controller.h"
#include <iostream>
#include <string>

class MCDU_Page_Prog : public MCDU_BasePage {
private:
    // Dữ liệu hiển thị mặc định của trang PROG Airbus
    int cruise_alt = 350;       // CRZ ALT: Độ cao hành trình (FL350)
    int opt_alt = 352;          // OPT ALT: Độ cao tối ưu do máy tính tính dựa trên cân nặng
    int max_alt = 391;          // MAX REC ALT: Độ cao trần tối đa khuyến cáo
    
    std::string brg_dist_wpt = "VVNB"; // Điểm dùng để check hướng/khoảng cách nhanh (Bearing/Distance)
    double brg = 345.0;                // Hướng bay đến điểm check (Độ)
    double dist = 120.5;               // Khoảng cách đến điểm check (Hải lý - NM)

public:
    MCDU_Page_Prog(MCDU_Controller* ctrl) : MCDU_BasePage("PROGRESS", ctrl) {}

    // 1. Vẽ giao diện trang PROG chuẩn đồ họa Airbus
    void render() override {
        std::cout << "            " << page_title << "            \n\n";
        
        // Dòng 1: Độ cao hành trình hiện tại và Độ cao tối ưu/Tối đa
        std::cout << " CRZ           OPT      MAX REC\n";
        std::cout << " FL" << cruise_alt << "         FL" << opt_alt << "   FL" << max_alt << "\n"; // LSK 1L để đổi độ cao trần
        
        // Dòng 2: Chỉ số tối ưu hóa nhiên liệu (Cost Index hoặc các chế độ bay)
        std::cout << " RECI FL\n";
        std::cout << " [  ]                               \n"; // LSK 2L
        
        std::cout << "------------------------------------\n";
        
        // Dòng 3 & 4: Tính năng BRG / DIST cực kỳ hay dùng để phi công kiểm tra nhanh một điểm bất kỳ
        std::cout << " BRG / DIST    TO\n";
        std::cout << " " << brg << " o/ " << dist << "NM  " << brg_dist_wpt << "\n"; // LSK 4R để nhập điểm check nhanh
        
        std::cout << " G/S           XTK \n";
        std::cout << " 420KT         R 0.0NM              \n"; // Tốc độ mặt đất và độ lệch tâm đường bay (Crosstrack Error)
    }

    // 2. Xử lý các nút bấm bên tay trái (LSK 1L -> 6L)
    void handle_lsk_left(int index, const std::string& scratchpad_data) override {
        // LSK 1L: Thay đổi độ cao hành trình (CRZ ALT) nếu phi công được ATC cho phép leo cao/xuống thấp
        if (index == 1) {
            if (scratchpad_data.empty()) return;
            
            try {
                // Thường phi công nhập kiểu số như "370" hoặc chữ "FL370"
                std::string raw_data = scratchpad_data;
                if (raw_data.substr(0, 2) == "FL") {
                    raw_data = raw_data.substr(2);
                }
                
                int new_alt = std::stoi(raw_data);
                
                // Logic chặn: Không cho phép nhập độ cao vượt quá giới hạn trần tối đa (MAX REC)
                if (new_alt > max_alt) {
                    std::cout << "\n[MCDU]: MAX ALT EXCEEDED (Vuot qua tran bay khuyen cao!)\n";
                } else {
                    cruise_alt = new_alt;
                    controller->clear_scratchpad();
                }
            } catch (...) {
                std::cout << "\n[MCDU]: FORMAT ERROR\n";
            }
        }
    }

    // 3. Xử lý các nút bấm bên tay phải (LSK 1R -> 6R)
    void handle_lsk_right(int index, const std::string& scratchpad_data) override {
        // LSK 4R: Nhập một điểm điều hướng bất kỳ để máy tính "bắn" góc (BRG) và khoảng cách (DIST) ngay lập tức
        if (index == 4) {
            if (scratchpad_data.empty()) return;
            
            // Nhận điểm mới vào bộ nhớ
            brg_dist_wpt = scratchpad_data;
            
            // Giả lập logic tính toán: Khi đổi điểm, góc và khoảng cách tự nhảy số ngẫu nhiên 
            // (Sau này đoạn này sẽ gọi hàm Haversine toán học để tính khoảng cách thật dựa trên tọa độ GPS)
            brg = (rand() % 360);
            dist = (rand() % 500) + 10.5;
            
            controller->clear_scratchpad();
        }
    }
};
