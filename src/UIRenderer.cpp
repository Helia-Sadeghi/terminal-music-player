#include "UIRenderer.h"
#include <iostream>
#include <cstdlib>
using namespace std;

static const int INNER = 56;
static const int TEXT  = INNER - 2;

// Unicode box-drawing characters (terminal is configured for UTF-8).
static const string TL = "\u2554"; // ╔
static const string TR = "\u2557"; // ╗
static const string BL = "\u255A"; // ╚
static const string BR = "\u255D"; // ╝
static const string H  = "\u2550"; // ═
static const string V  = "\u2551"; // ║
static const string SL = "\u2560"; // ╠
static const string SR = "\u2563"; // ╣

static int dispWidth(const string& s) {
    int w = 0;
    for (unsigned char c : s) {
        if ((c & 0xC0) != 0x80) ++w;   
    }
    return w;
}

static string takeCodepoints(const string& s, int width) {
    int count = 0;
    size_t i = 0;
    while (i < s.size() && count < width) {
        unsigned char c = s[i];
        size_t len = 1;
        if      ((c & 0x80) == 0x00) len = 1;
        else if ((c & 0xE0) == 0xC0) len = 2;
        else if ((c & 0xF0) == 0xE0) len = 3;
        else if ((c & 0xF8) == 0xF0) len = 4;
        if (i + len > s.size()) len = s.size() - i;
        i += len;
        ++count;
    }
    return s.substr(0, i);
}

static string padField(const string& text, int width) {
    int w = dispWidth(text);
    if (w > width) {
        if (width <= 3) return takeCodepoints(text, width);
        return takeCodepoints(text, width - 3) + "...";
    }
    return text + string(width - w, ' ');
}

static string repeat(const string& s, int n) {
    string r;
    r.reserve(s.size() * n);
    for (int i = 0; i < n; ++i) r += s;
    return r;
}

void UIRenderer::init() {}

int UIRenderer::textWidth() const { return TEXT; }

void UIRenderer::clearScreen() {
#ifdef _WIN32
    int rc = system("cls");
#else
    int rc = system("clear");
#endif
    (void)rc;
}

void UIRenderer::home()       { cout << "\033[H"; }
void UIRenderer::hideCursor() { cout << "\033[?25l"; }
void UIRenderer::showCursor() { cout << "\033[?25h"; }

void UIRenderer::drawTopBorder()    { cout << TL << repeat(H, INNER) << TR << "\n"; }
void UIRenderer::drawBottomBorder() { cout << BL << repeat(H, INNER) << BR << "\n"; }
void UIRenderer::drawSeparator()    { cout << SL << repeat(H, INNER) << SR << "\n"; }

void UIRenderer::drawRow(const string& content) {
    cout << V << " " << padField(content, TEXT) << " " << V << "\n";
}

void UIRenderer::drawCenteredRow(const string& content) {
    int w = dispWidth(content);
    if (w > TEXT) { drawRow(content); return; }
    int left  = (TEXT - w) / 2;
    int right = TEXT - w - left;
    cout << V << " " << string(left, ' ') << content
         << string(right, ' ') << " " << V << "\n";
}

void UIRenderer::drawBox(const string& title, const vector<string>& lines) {
    drawTopBorder();
    drawCenteredRow(title);
    drawSeparator();
    for (const string& line : lines) drawRow(line);
    drawBottomBorder();
}
