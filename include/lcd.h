#ifndef LCD_H
#define LCD_H
extern int lcd_init(int color);
extern void lcd_design();
extern void lcd_draw_word(unsigned char *ch, int color, int len, int x, int y);
extern void lcd_draw_word1(unsigned char *ch, int color, int len,  int w, int x, int y);
extern void lcd_draw_rect(int x0, int y0, int w, int h, int color);

#endif