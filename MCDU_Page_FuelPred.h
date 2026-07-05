#pragma once
#include "MCDU_BasePage.h"
#include "MCDU_Controller.h"
#include <iostream>
#include <string>

class MCDU_Page_FuelPred : public MCDU_BasePage {
private:
    // Các thông số trọng tải lấy từ bộ nhớ INIT B sang (Giả lập đơn vị Tấn - Tonnes)
    double zfw = 62.5;       // Zero Fuel Weight (Cân nặng máy bay chưa tính xăng)
    double fob = 12.4;       // Fuel On Board (Lượng xăng hiện tại trong bình)
    
    // Các thông số dự báo xăng tiêu thụ do máy tính tự tính toán (Auto-calculated)
    double rte_rsv_fuel = 0.4;  // Xăng dự phòng dọc đường (Route Reserve - 5%)
    double altn_fuel = 1.5;     // Xăng để bay đến sân bay dự phòng (Alternate)
    double hold_fuel = 1.2;     // Xăng để bay vòng chờ hạ cánh (Holding - 30 phút)
    
    // Kết quả tính toán quan trọng nhất: Xăng dự trữ tối thiểu bắt buộc (EXTRA FUEL)
    // Công thức Airbus: EXTRA = FOB - (RTE RSV + ALTN + HOLD + MIN DEST RY)
    double extra_fuel = 5.8; 
    std::string extra_time = "01+25"; // Thời gian bay tương ứng với số xăng Extra (1 tiếng 25 phút)

public:
    MCDU_Page_FuelPred(MCDU_Controller* ctrl) : MCDU_BasePage("FUEL PRED", ctrl) {}

    // 1. Vẽ giao diện FUEL PRED đúng chuẩn màn hình xanh/hổ phách của Airbus
    void render() override {
        std::cout << "            " << page_title << "            \n\n";
        
        // Dòng 1: Hiển thị tổng cân nặng và xăng hiện tại trong bình (LSK 1L & 1R)
        std::cout << " AT ZFW       FOB        GW / CG\n";
        std::cout << "  " << zfw << "        " << fob << "T      74.9 / 28.2\n";
        
        std::cout << "------------------------------------\n";
        
        // Dòng 2 & 3: Xăng dự phòng chặng bay và Xăng bay đến sân bay dự phòng (LSK 2R & 3R)
        std::cout << " RTE RSV / %             ALTN / TIME\n";
        std::cout << "  " << rte_rsv_fuel << "  / 5.0%             " << altn_fuel << " / 00+22\n"; // LSK 2R & 3R
        
        // Dòng 4: Xăng bay chờ (LSK 4R)
        std::cout << "                         FINAL / TIME\n";
        std::cout << "                         " << hold_fuel << " / 00+30\n"; // LSK 4R
        
        std::cout << "------------------------------------\n";
        
        // Dòng đáy quan trọng nhất: Lượng xăng thừa an toàn (EXTRA)
        std::cout << " EXTRA / TIME\n";
        std::cout << "  " << extra_fuel << " / " << extra_time << "                   \n"; // LSK 5L
    }

    // 2. Logic xử lý các nút bấm bên tay trái (LSK 1L -> 6L)
    void handle_lsk_left(int index, const std::string& scratchpad_data) override {
        // Trang FUEL PRED chủ yếu hiển thị dữ liệu tự động, phi công ít khi can thiệp cột trái
        if (index == 5) {
            std::cout << "\n[FUEL LOGIC]: Day la luong xang Extra an toan sau khi tru het chi phi bay!\n";
        }
    }

    // 3. Logic xử lý các nút bấm bên tay phải (LSK 1R -> 6R) - Cho phép phi công sửa lượng xăng dự phòng
    void handle_lsk_right(int index, const std::string& scratchpad_data) override {
        if (scratchpad_data.empty()) return;

        try {
            double new_val = std::stod(scratchpad_data);

            // LSK 3R: Phi công thay đổi lượng xăng dự phòng để bay đến sân bay thay thế (Alternate Fuel)
            if (index == 3) {
                altn_fuel = new_val;
                recalculate_extra(); // Gọi hàm tính toán lại xăng thừa ngay lập tức
                controller->clear_scratchpad();
            }
            // LSK 4R: Thay đổi lượng xăng bay chờ Holding
            else if (index == 4) {
                hold_fuel = new_val;
                recalculate_extra();
                controller->clear_scratchpad();
            }
        } catch (...) {
            std::cout << "\n[MCDU]: FORMAT ERROR\n";
        }
    }

private:
    // Hàm logic cốt lõi: Tự động cập nhật lượng xăng EXTRA dựa trên các thông số mới nhập vào
    void recalculate_extra() {
        // Giả lập công thức tính toán xăng thừa đơn giản của FMS Airbus
        double min_dest_fuel = 3.5; // Xăng tối thiểu phải còn lại khi chạm bánh xuống đường băng đích
        extra_fuel = fob - (rte_rsv_fuel + altn_fuel + hold_fuel + min_dest_fuel);
        
        if (extra_fuel < 0) {
            extra_fuel = 0.0;
            extra_time = "00+00";
            std::cout << "\n[MCDU WARNING]: LOW EXTRA FUEL (Xang du tru o muc bao dong!)\n";
        } else {
            // Giả lập tính thời gian bay: 1 tấn xăng chạy được khoảng 15 phút bay chờ
            int total_minutes = static_cast<int>(extra_fuel * 15);
            int hours = total_minutes / 60;
            int minutes = total_minutes % 60;
            
            // Định dạng lại chuỗi hiển thị dạng HH+MM (Ví dụ: 01+15)
            char buf[10];
            snprintf(buf, sizeof(buf), "%02d+%02d", hours, minutes);
            extra_time = std::string(buf);
        }
    }
};
