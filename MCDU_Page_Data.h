#pragma once
#include "MCDU_BasePage.h"
#include "MCDU_Controller.h"
#include <iostream>
#include <string>

class MCDU_Page_Data : public MCDU_BasePage {
private:
    // Dữ liệu giả lập hệ thống hàng không Airbus thời nay
    std::string airac_cycle = "2607";       // Chu kỳ bản đồ định vị (Ví dụ tháng 7 năm 2026)
    std::string active_db = "NAV DECS 2026"; // Tên cơ sở dữ liệu bay đang chạy
    
    // Tọa độ GPS giả lập của máy bay
    std::string gps_pos = "N 20o59.4 / E 105o59.2"; 

public:
    MCDU_Page_Data(MCDU_Controller* ctrl) : MCDU_BasePage("DATA INDEX", ctrl) {}

    // 1. Vẽ giao diện DATA INDEX gồm các menu con đặc trưng của Airbus
    void render() override {
        std::cout << "            " << page_title << "            \n\n";
        
        // Cột trái: Các trang tra cứu cảm biến và tọa độ (LSK 1L, 2L, 3L)
        std::cout << " <WAYPOINTS                    A/C STATUS>\n"; // LSK 1L & LSK 1R
        std::cout << "                                          \n";
        std::cout << " <POSITION MONITOR         CLOSEST ARPTS>\n"; // LSK 2L & LSK 2R
        std::cout << "                                          \n";
        std::cout << " <IRS MONITOR                            \n"; // LSK 3L
        std::cout << "                                          \n";
        std::cout << " <GPS MONITOR                            \n"; // LSK 4L
        
        std::cout << "------------------------------------\n";
        // Dòng đáy hiển thị nhanh chu kỳ AIRAC đang hoạt động
        std::cout << " ACTIVE NAV DATA BASE: " << airac_cycle << "\n";
    }

    // 2. Logic xử lý các nút bấm bên tay trái (LSK 1L -> 6L)
    void handle_lsk_left(int index, const std::string& scratchpad_data) override {
        // LSK 2L: Phi công muốn xem màn hình tọa độ máy bay
        if (index == 2) {
            std::cout << "\n--- POSITION MONITOR ---\n";
            std::cout << "FM POSITION: " << gps_pos << "\n";
            std::cout << "GPS PRIMARY: HIGH ACCURACY\n";
            std::cout << "------------------------\n";
        }
        // LSK 3L: Xem trạng thái hệ thống dẫn đường quán tính IRS
        else if (index == 3) {
            std::cout << "\n--- IRS MONITOR ---\n";
            std::cout << "IRS 1: ALIGNED (NAV MODE)\n";
            std::cout << "IRS 2: ALIGNED (NAV MODE)\n";
            std::cout << "IRS 3: ALIGNED (NAV MODE)\n";
            std::cout << "-------------------\n";
        }
        // LSK 4L: Xem thông tin vệ tinh GPS
        else if (index == 4) {
            std::cout << "\n--- GPS MONITOR ---\n";
            std::cout << "SAT RECEIVED: 12 SATELLITES\n";
            std::cout << "MODE: NAV HYBRID (GPIRS)\n";
            std::cout << "-------------------\n";
        }
    }

    // 3. Logic xử lý các nút bấm bên tay phải (LSK 1R -> 6R)
    void handle_lsk_right(int index, const std::string& scratchpad_data) override {
        // LSK 1R: Tra cứu thông tin máy bay, loại động cơ và phiên bản phần mềm
        if (index == 1) {
            std::cout << "\n--- AIRCRAFT STATUS ---\n";
            std::cout << "AIRCRAFT TYPE: A320-251N (Neo)\n";
            std::cout << "ENGINES: LEAP-1A26\n";
            std::string status_db = active_db;
            std::cout << "ACTIVE DB: " << status_db << " (" << airac_cycle << ")\n";
            std::cout << "-----------------------\n";
        }
        // LSK 2R: Tìm kiếm các sân bay gần nhất trong khu vực bay (Tính năng khẩn cấp)
        else if (index == 2) {
            std::cout << "\n--- CLOSEST AIRPORTS ---\n";
            std::cout << "1. VVNB (HA NOI)   - 12 NM\n";
            std::cout << "2. VVCI (HAI PHONG)- 45 NM\n";
            std::cout << "3. VVDN (DA NANG)  - 320 NM\n";
            std::cout << "------------------------\n";
        }
    }
};
