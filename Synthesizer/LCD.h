
#include "stdio.h"
#include "stdint.h"

#define WHITE         0xFFFF
#define BLACK         0x0000    
#define BLUE          0x001F  
#define BRED          0XF81F
#define GRED          0XFFE0
#define GBLUE         0X07FF
#define RED           0xF800
#define MAGENTA       0xF81F
#define GREEN         0x07E0
#define CYAN          0x7FFF
#define YELLOW        0xFFE0
#define BROWN         0XBC40 
#define BRRED         0XFC07 
#define GRAY          0X8430 
#define DARKBLUE      0X01CF  
#define LIGHTBLUE     0X7D7C   
#define GRAYBLUE      0X5458 
#define LIGHTGREEN    0X841F 
#define LGRAY         0XC618 
#define LGRAYBLUE     0XA651
#define LBBLUE        0X2B12 


#define IMAGE_BACKGROUND    WHITE
#define FONT_FOREGROUND     BLACK
#define FONT_BACKGROUND     WHITE

#define FONT_24_WIDTH 17
#define FONT_24_HEIGHT 24

#define LCD_WIDTH 320
#define LCD_HEIGHT 240 

void LCD_init(void);
void LCD_WriteData_word(uint16_t data);
void LCD_WriteData_Byte(uint8_t data);
void LCD_Write_Command(uint8_t data);
void LCD_SetCursor(uint16_t x, uint16_t y);
void LCD_clear(uint16_t Color);
void LCD_SetWindow(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t  Yend);
void LCD_Reset(void);
void Paint_DrawImage(const unsigned char *image, uint16_t xStart, uint16_t yStart, uint16_t W_Image, uint16_t H_Image);
void LCD_DrawPaint(uint16_t x, uint16_t y, uint16_t Color);
void Paint_DrawLine(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t Color, uint8_t Line_width, uint8_t Line_Style);
void Paint_DrawPoint(uint16_t Xpoint, uint16_t Ypoint, uint16_t Color, uint8_t Dot_Pixel, uint8_t Dot_FillWay);
void Paint_DrawRectangle( uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t Color, uint8_t Line_width, uint8_t Filled );
void Paint_DrawChar(uint16_t Xpoint,uint16_t Ypoint, const char Acsii_Char, uint16_t Color_Background, uint16_t Color_Foreground);
void Paint_DrawString_EN(uint16_t Xstart, uint16_t Ystart, const char * pString, uint16_t Color_Background, uint16_t Color_Foreground );