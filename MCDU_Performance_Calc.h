#pragma once
#include <iostream>
#include <string>

// Bỏ hẳn cái struct trùng lặp ở đây đi vì file Perf đã giữ rồi

class MCDU_Performance_Calc {
public:
    // Giữ nguyên logic tính toán tự động
    static void calculate_v_speeds(double zfw, double block_fuel, int flaps_config, int& out_v1, int& out_vr, int& out_v2) {
        double gross_weight = zfw + block_fuel; 
        
        int base_v1 = 130;
        int base_vr = 132;
        int base_v2 = 135;
        double weight_delta = gross_weight - 60.0;
        
        out_v1 = base_v1 + static_cast<int>(weight_delta * 0.85);
        out_vr = base_vr + static_cast<int>(weight_delta * 0.90);
        out_v2 = base_v2 + static_cast<int>(weight_delta * 0.95);

        if (flaps_config == 2) {
            out_v1 -= 3; out_vr -= 2; out_v2 -= 1;
        } else if (flaps_config == 3) {
            out_v1 -= 5; out_vr -= 4; out_v2 -= 2;
        }
    }
};
