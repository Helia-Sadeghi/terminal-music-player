#pragma once
#include <string>
#include <vector>

class UIRenderer {
public:
    void init();
    void clearScreen();          
    void home();                
    void hideCursor();
    void showCursor();

    void drawTopBorder();
    void drawSeparator();
    void drawBottomBorder();

    void drawRow(const std::string& content);         
    void drawCenteredRow(const std::string& content);  
    void drawBox(const std::string& title,
                 const std::vector<std::string>& lines);

    
    int textWidth() const;
};
