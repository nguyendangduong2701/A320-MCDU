#pragma once
#include <string>

class MCDU_Controller; 

class MCDU_BasePage {
protected:
    std::string page_title;
    MCDU_Controller* controller;

    // --- AIRBUS CRT DISPLAY COLOR CODES ---
    const std::string RESET = "\033[0m";
    const std::string WHITE = "\033[1;37m";  // Titles and Labels
    const std::string AMBER = "\033[1;33m";  // Mandatory Input Fields (Square brackets)
    const std::string GREEN = "\033[1;32m";  // Active / Auto-computed Data
    const std::string CYAN  = "\033[1;36m";  // New tech / Special indicators

public:
    MCDU_BasePage(const std::string& title, MCDU_Controller* ctrl) 
        : page_title(title), controller(ctrl) {}
    
    virtual ~MCDU_BasePage() = default;

    virtual void render() = 0; 
    virtual void handle_lsk_left(int index, const std::string& scratchpad_data) = 0;
    virtual void handle_lsk_right(int index, const std::string& scratchpad_data) = 0;
};
