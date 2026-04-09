#include "start_screen.h"
#include "display_utils.h"

static int frame = 0;

int start_screen_draw(void) {
    const char *line1 = "SPACE";
    const char *line2 = "INVADERS";
    const int len1 = 5;
    const int len2 = 8;
    const int total_chars = len1 + len2;
    const int frames_per_char = 12; //~200ms per, tune as needed
    
    int reveal = frame / frames_per_char;
    if (reveal > total_chars) {
        reveal = total_chars;
    }
    // draw SPACE
    int line1chars = reveal < len1 ? reveal : len1;
    int x = 22;
    int y1 = 10;
    for (int i = 0; i < line1chars; i++) {
        x += draw_char(x, y1, line1[i], FONT_5X5_FLEX, 0, 255, 100) + 1;
    }
    // blink cursor on line 1
    if (reveal < len1 && (frame / 3) % 2 == 0) {
        draw_rect(x, y1, 2, 5, -1, 0, 255, 100);
    }
    // draw INVADERS
    int line2chars = reveal > len1 ? reveal - len1 : 0;
    if (line2chars > len2) line2chars = len2;
    int x2 = 12;
    int y2 = 20;
    for (int i = 0; i < line2chars; i++) {
        x2 += draw_char(x2, y2, line2[i], FONT_5X5_FLEX, 0, 255, 100) + 1;
    }
    // blink cursor on line 2
    if (reveal >= len1 && reveal < total_chars && (frame / 3) % 2 == 0) {
        draw_rect(x2, y2, 2, 5, -1, 0, 255, 100);
    }
    // hold 30 frames after full reveal, then loop
    if (frame > total_chars * frames_per_char + 40) {
        frame = 0;
    } else {
        frame++;
    }

    return 0;
}
