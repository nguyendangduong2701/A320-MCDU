#pragma once
#include "MCDU_BasePage.h"
#include "MCDU_Controller.h"
#include <iostream>
#include <string>
#include <vector>

class MCDU_Page_AtcComm : public MCDU_BasePage {
private:
    std::string atc_center = "____";     // Mã đài kiểm soát không lưu (Ví dụ: VVTS, VVNB)
    std::string connection_status = "NOT ACTIVE"; 
    bool is_connected = false;

    // Hộp thư đến chứa tin nhắn từ Không lưu (ATC) gửi xuống
    std::vector<std::string> incoming_messages;

public:
    MCDU_Page_AtcComm(MCDU_Controller* ctrl) : MCDU_BasePage("ATC COMM", ctrl) {
        // Giả lập một vài tin nhắn mẫu từ ATC trong bộ nhớ
        incoming_messages.push_back("CLIMB TO FL350");
        incoming_messages.push_back("SQUAWK 2026");
    }

    // 1. Vẽ giao diện ATC COMM đúng chuẩn CPDLC kỹ thuật số của Airbus
    void render() override {
        std::cout << "            " << page_title << "            \n\n";
        
        std::cout << " ATC CTR                    STATUS\n";
        std::cout << " [" << atc_center << "]               " << connection_status << "\n"; // LSK 1L để nhập đài, hiển thị trạng thái ở 1R
        
        std::cout << "------------------------------------\n";
        
        // Hiển thị danh sách tin nhắn từ không lưu ở giữa màn hình
        std::cout << " ── INCOMING ATC MESSAGES ──\n";
        if (is_connected) {
            for (size_t i = 0; i < incoming_messages.size(); ++i) {
                std::cout << "  MSG " << i + 1 << ": " << incoming_messages[i] << "\n";
            }
            std::cout << "------------------------------------\n";
            std::cout << " <WILCO (ACCEPT)           STANDBY>\n"; // LSK 5L (Đồng ý lệnh) & LSK 5R (Chờ chút)
            std::cout << " <UNABLE (REJECT)                 \n"; // LSK 6L (Từ chối lệnh)
        } else {
            std::cout << "   NO ACTIVE CONNECTION TO ATC\n";
            std::cout << "   ENTER CENTER CODE AND PRESS 1L\n";
            std::cout << "------------------------------------\n";
        }
    }

    // 2. Logic xử lý các nút bấm bên tay trái (LSK 1L -> 6L)
    void handle_lsk_left(int index, const std::string& scratchpad_data) override {
        // LSK 1L: Nhập mã đài kiểm soát không lưu để kết nối mạng Data Link
        if (index == 1) {
            if (scratchpad_data.empty()) return;
            
            if (scratchpad_data == "CLR") {
                atc_center = "____";
                connection_status = "NOT ACTIVE";
                is_connected = false;
                std::cout << "\n[ATC COMM]: Da ngat ket noi Data Link.\n";
            } else {
                atc_center = scratchpad_data;
                connection_status = "CONNECTED";
                is_connected = true;
                std::cout << "\n[ATC COMM]: Da thiet lap ket noi CPDLC voi dai: " << atc_center << "\n";
            }
            controller->clear_scratchpad();
        }
        
        // Khi đã kết nối, xử lý các nút phản hồi tin nhắn ATC
        if (is_connected) {
            // LSK 5L: Bấm WILCO (Will Comply) -> Báo với ATC là tôi hiểu và sẽ làm theo lệnh
            if (index == 5) {
                std::cout << "\n[CPDLC TX]: Gửi tin nhan 'WILCO' den dai " << atc_center << "\n";
                if (!incoming_messages.empty()) {
                    incoming_messages.erase(incoming_messages.begin()); // Xóa tin nhắn cũ nhất vì đã xử lý xong
                }
            }
            // LSK 6L: Bấm UNABLE -> Báo với ATC là không thể thực hiện lệnh vì lý do an toàn/thời tiết
            else if (index == 6) {
                std::cout << "\n[CPDLC TX]: Gửi tin nhan 'UNABLE' den dai " << atc_center << "\n";
            }
        }
    }

    // 3. Logic xử lý các nút bấm bên tay phải (LSK 1R -> 6R)
    void handle_lsk_right(int index, const std::string& scratchpad_data) override {
        // LSK 5R: Bấm STANDBY -> Báo với ATC là phi công đang kiểm tra lại, sẽ trả lời sau
        if (is_connected && index == 5) {
            std::cout << "\n[CPDLC TX]: Gửi tin nhan 'STANDBY' den dai " << atc_center << "\n";
        }
    }
};
