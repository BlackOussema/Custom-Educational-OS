#include <stdint.h>

// تعريف عنوان ذاكرة الفيديو لوضع الرسوم (VGA Mode 13h)
#define VGA_ADDRESS 0xA0000
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

// ألوان VGA الأساسية
#define BLACK  0
#define BLUE   1
#define GREEN  2
#define CYAN   3
#define RED    4
#define WHITE  15
#define GRAY   7

/* 1. دالة رسم بكسل واحد (أساس كل شيء) */
void draw_pixel(int x, int y, uint8_t color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    uint8_t* screen = (uint8_t*)VGA_ADDRESS;
    screen[y * SCREEN_WIDTH + x] = color;
}

/* 2. دالة تنظيف الشاشة بلون معين */
void clear_screen(uint8_t color) {
    uint8_t* screen = (uint8_t*)VGA_ADDRESS;
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        screen[i] = color;
    }
}

/* 3. دالة رسم مستطيل (لبناء النوافذ والأزرار) */
void draw_rect(int x, int y, int w, int h, uint8_t color) {
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            draw_pixel(x + i, y + j, color);
        }
    }
}

/* 4. دالة رسم إطار نافذة (Window Frame) */
void draw_window(int x, int y, int w, int h, char* title) {
    // رسم جسم النافذة الرئيسي (رمادي)
    draw_rect(x, y, w, h, GRAY);
    // رسم شريط العنوان (أزرق)
    draw_rect(x, y, w, 15, BLUE);
    // رسم زر الإغلاق الصغير (أحمر)
    draw_rect(x + w - 12, y + 2, 10, 10, RED);
}

/* 5. دالة رسم خط (Line) */
void draw_line(int x1, int y1, int x2, int y2, uint8_t color) {
    // خوارزمية بسيطة لرسم الخطوط
    int dx = x2 - x1;
    int dy = y2 - y1;
    for (int x = x1; x <= x2; x++) {
        int y = y1 + dy * (x - x1) / dx;
        draw_pixel(x, y, color);
    }
}
