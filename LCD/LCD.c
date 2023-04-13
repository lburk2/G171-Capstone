#include <stdio.h>
#include <stdint.h>
#include "LCD.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "font24.h"

/*******************************************************************************
 * Global Variable Initialization
 */
static const uint8_t REG_DEVID = 0x00;

uint8_t cs_pin = 5;
uint8_t sck_pin = 6;
uint8_t mosi_pin = 4;
uint8_t miso_pin = 0;
uint8_t DC_pin = 8;
uint8_t RST_pin = 7;

/*******************************************************************************
 * Function Declarations
 */
void reg_write(const uint8_t data);

int reg_read(spi_inst_t *spi,
             const uint cs,
             const uint8_t reg,
             uint8_t *buf,
             uint8_t nbytes);

/*******************************************************************************
 * Function Definitions
 */

// Write 1 byte to the specified register
void reg_write(const uint8_t data)
{

    uint8_t msg[1];

    // Construct message (set ~W bit low, MB bit low)
    msg[0] = data;

    // Write to register

    spi_write_blocking(spi0, msg, 1);
}

int reg_read(spi_inst_t *spi,
             const uint cs,
             const uint8_t reg,
             uint8_t *buf,
             const uint8_t nbytes)
{

    int num_bytes_read = 0;
    uint8_t mb = 0;

    // Determine if multiple byte (MB) bit should be set
    if (nbytes < 1)
    {
        return -1;
    }
    else if (nbytes == 1)
    {
        mb = 0;
    }
    else
    {
        mb = 1;
    }

    // Construct message (set ~W bit high)
    uint8_t msg = 0x80 | (mb << 6) | reg;

    // Read from register
    gpio_put(cs, 0);
    spi_write_blocking(spi, &msg, 1);
    num_bytes_read = spi_read_blocking(spi, 0, buf, nbytes);
    gpio_put(cs, 1);

    return num_bytes_read;
}

void LCD_init(void)
{
    // Buffer to store raw reads
    uint8_t data[6];

    // Ports
    spi_inst_t *spi = spi0;

    // Initialize chosen serial port
    stdio_init_all();

    // Initialize CS pin high
    gpio_init(cs_pin);
    gpio_set_dir(cs_pin, GPIO_OUT);
     gpio_pull_up(cs_pin);
    // Initialize DC pin LOW for command mode
    gpio_init(RST_pin);
    gpio_set_dir(RST_pin, GPIO_OUT);
    gpio_pull_up(RST_pin);
    // Initialize RST pin HIGH
    gpio_init(DC_pin);
    gpio_set_dir(DC_pin, GPIO_OUT);
    gpio_pull_down(DC_pin);

    // Initialize SPI port at 125 MHz
    spi_init(spi, 12500000);

    // Set SPI format
    spi_set_format(spi,        // SPI instance
                   8,          // Number of bits per transfer
                   SPI_CPOL_0, // Polarity (CPOL)
                   SPI_CPHA_0, // Phase (CPHA)
                   SPI_MSB_FIRST);

    // Initialize SPI pins
    gpio_init(sck_pin);
    gpio_set_function(sck_pin, GPIO_FUNC_SPI);
    gpio_init(mosi_pin);
    gpio_set_function(mosi_pin, GPIO_FUNC_SPI);
    gpio_init(miso_pin);
    gpio_set_function(miso_pin, GPIO_FUNC_SPI);

    reg_read(spi, cs_pin, REG_DEVID, data, 1);

    LCD_Reset();

    LCD_Write_Command(0x36);
    LCD_WriteData_Byte(0xA0);

    LCD_Write_Command(0x3A);
    LCD_WriteData_Byte(0x55);
    sleep_ms(10);

    LCD_Write_Command(0x21);

    LCD_Write_Command(0x2A);
    LCD_WriteData_Byte(0x00);
    LCD_WriteData_Byte(0x01);
    LCD_WriteData_Byte(0x00);
    LCD_WriteData_Byte(0x3F);

    LCD_Write_Command(0x2B);
    LCD_WriteData_Byte(0x00);
    LCD_WriteData_Byte(0x00);
    LCD_WriteData_Byte(0x00);
    LCD_WriteData_Byte(0xEF);

    LCD_Write_Command(0xB2);
    LCD_WriteData_Byte(0x01);
    LCD_WriteData_Byte(0x01);
    LCD_WriteData_Byte(0x00);
    LCD_WriteData_Byte(0x33);
    LCD_WriteData_Byte(0x33);

    LCD_Write_Command(0xB7);
    LCD_WriteData_Byte(0x35);

    LCD_Write_Command(0xBB);
    LCD_WriteData_Byte(0x1F);

    LCD_Write_Command(0xC0);
    LCD_WriteData_Byte(0x2C);

    LCD_Write_Command(0xC2);
    LCD_WriteData_Byte(0x01);

    LCD_Write_Command(0xC3);
    LCD_WriteData_Byte(0x12);

    LCD_Write_Command(0xC4);
    LCD_WriteData_Byte(0x20);

    LCD_Write_Command(0xC6);
    LCD_WriteData_Byte(0x01);

    LCD_Write_Command(0xD0);
    LCD_WriteData_Byte(0xA4);
    LCD_WriteData_Byte(0xA3);

    LCD_Write_Command(0xE0);
    LCD_WriteData_Byte(0xD0);
    LCD_WriteData_Byte(0x08);
    LCD_WriteData_Byte(0x11);
    LCD_WriteData_Byte(0x08);
    LCD_WriteData_Byte(0x0C);
    LCD_WriteData_Byte(0x15);
    LCD_WriteData_Byte(0x39);
    LCD_WriteData_Byte(0x33);
    LCD_WriteData_Byte(0x50);
    LCD_WriteData_Byte(0x36);
    LCD_WriteData_Byte(0x13);
    LCD_WriteData_Byte(0x14);
    LCD_WriteData_Byte(0x29);
    LCD_WriteData_Byte(0x2D);

    LCD_Write_Command(0xE1);
    LCD_WriteData_Byte(0xD0);
    LCD_WriteData_Byte(0x08);
    LCD_WriteData_Byte(0x10);
    LCD_WriteData_Byte(0x08);
    LCD_WriteData_Byte(0x06);
    LCD_WriteData_Byte(0x06);
    LCD_WriteData_Byte(0x39);
    LCD_WriteData_Byte(0x44);
    LCD_WriteData_Byte(0x51);
    LCD_WriteData_Byte(0x0B);
    LCD_WriteData_Byte(0x16);
    LCD_WriteData_Byte(0x14);
    LCD_WriteData_Byte(0x2F);
    LCD_WriteData_Byte(0x31);
    LCD_Write_Command(0x21);

    LCD_Write_Command(0x11);

    LCD_Write_Command(0x21);

    LCD_Write_Command(0x29);
}
void LCD_WriteData_word(uint16_t data)
{
    gpio_put(cs_pin, 0);
    gpio_put(DC_pin, 1);
    reg_write((data >> 8) & 0xff);
    reg_write(data);
    gpio_put(cs_pin, 1);
}
void LCD_WriteData_Byte(uint8_t data)
{
    gpio_put(cs_pin, 0);
    gpio_put(DC_pin, 1);
    reg_write(data);
    gpio_put(cs_pin, 1);
}
void LCD_Write_Command(uint8_t data)
{
    gpio_put(cs_pin, 0);
    gpio_put(DC_pin, 0);
    reg_write(data);
}
void LCD_SetCursor(uint16_t x, uint16_t y)
{
    LCD_Write_Command(0x2a);
    LCD_WriteData_Byte(x >> 8);
    LCD_WriteData_Byte(x);
    LCD_WriteData_Byte(x >> 8);
    LCD_WriteData_Byte(x);

    LCD_Write_Command(0x2b);
    LCD_WriteData_Byte(y >> 8);
    LCD_WriteData_Byte(y);
    LCD_WriteData_Byte(y >> 8);
    LCD_WriteData_Byte(y);

    LCD_Write_Command(0x2C);
}
void LCD_clear(uint16_t Color)
{
    unsigned int i, j;
    LCD_SetWindow(0, 0, LCD_WIDTH, LCD_HEIGHT);
    gpio_put(DC_pin, 1);
    for (i = 0; i < LCD_WIDTH; i++)
    {
        for (j = 0; j < LCD_HEIGHT; j++)
        {
            reg_write((Color >> 8) & 0xff);
            reg_write(Color);
        }
    }
}

void LCD_SetWindow(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend)
{
    LCD_Write_Command(0x2a);
    LCD_WriteData_Byte(0x00);
    LCD_WriteData_Byte(Xstart & 0xff);
    LCD_WriteData_Byte((Xend - 1) >> 8);
    LCD_WriteData_Byte((Xend - 1) & 0xff);

    LCD_Write_Command(0x2b);
    LCD_WriteData_Byte(0x00);
    LCD_WriteData_Byte(Ystart & 0xff);
    LCD_WriteData_Byte((Yend - 1) >> 8);
    LCD_WriteData_Byte((Yend - 1) & 0xff);

    LCD_Write_Command(0x2C);
}

void LCD_Reset(void)
{
    sleep_ms(200);
    gpio_put(RST_pin, 0);
    sleep_ms(200);
    gpio_put(RST_pin, 1);
    sleep_ms(200);
}

void LCD_DrawPaint(uint16_t x, uint16_t y, uint16_t Color)
{

    LCD_SetCursor(x, y);
    LCD_WriteData_word(Color);
}

void Paint_DrawImage(const unsigned char *image, uint16_t xStart, uint16_t yStart, uint16_t W_Image, uint16_t H_Image)
{
    LCD_Write_Command(0x10);
    int k = 0;
    for (int j = 0; j < H_Image; j++)
    {
        for (int i = 0; i < W_Image; i++)
        {
            if (i < LCD_WIDTH && j < LCD_HEIGHT) // Exceeded part does not display
                LCD_DrawPaint(i, j, (image[k] << 8 | image[k + 1]));
            k += 2;
        }
    }
    LCD_Write_Command(0x11);
}

void Paint_DrawLine(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t Color, uint8_t Line_width, uint8_t Line_Style)
{
    uint16_t Xpoint = Xstart;
    uint16_t Ypoint = Ystart;
    int dx = (int)Xend - (int)Xstart >= 0 ? Xend - Xstart : Xstart - Xend;
    int dy = (int)Yend - (int)Ystart <= 0 ? Yend - Ystart : Ystart - Yend;

    // Increment direction, 1 is positive, -1 is counter;
    int XAddway = Xstart < Xend ? 1 : -1;
    int YAddway = Ystart < Yend ? 1 : -1;

    // Cumulative error
    int Esp = dx + dy;
    char Dotted_Len = 0;

    for (;;)
    {
        Dotted_Len++;
        // Painted dotted line, 2 point is really virtual
        if (Line_Style == 1 && Dotted_Len % 3 == 0)
        {
            // Debug("LINE_DOTTED\r\n");
            Paint_DrawPoint(Xpoint, Ypoint, IMAGE_BACKGROUND, Line_width, 1);
            Dotted_Len = 0;
        }
        else
        {
            Paint_DrawPoint(Xpoint, Ypoint, Color, Line_width, 1);
        }
        if (2 * Esp >= dy)
        {
            if (Xpoint == Xend)
                break;
            Esp += dy;
            Xpoint += XAddway;
        }
        if (2 * Esp <= dx)
        {
            if (Ypoint == Yend)
                break;
            Esp += dx;
            Ypoint += YAddway;
        }
    }
}
void Paint_DrawPoint(uint16_t Xpoint, uint16_t Ypoint, uint16_t Color, uint8_t Dot_Pixel, uint8_t Dot_FillWay)
{
    int16_t XDir_Num, YDir_Num;
    if (Dot_FillWay == 1)
    {
        for (XDir_Num = 0; XDir_Num < 2 * Dot_Pixel - 1; XDir_Num++)
        {
            for (YDir_Num = 0; YDir_Num < 2 * Dot_Pixel - 1; YDir_Num++)
            {
                if (Xpoint + XDir_Num - Dot_Pixel < 0 || Ypoint + YDir_Num - Dot_Pixel < 0)
                    break;
                // printf("x = %d, y = %d\r\n", Xpoint + XDir_Num - Dot_Pixel, Ypoint + YDir_Num - Dot_Pixel);
                LCD_DrawPaint(Xpoint + XDir_Num - Dot_Pixel, Ypoint + YDir_Num - Dot_Pixel, Color);
            }
        }
    }
    else
    {
        for (XDir_Num = 0; XDir_Num < Dot_Pixel; XDir_Num++)
        {
            for (YDir_Num = 0; YDir_Num < Dot_Pixel; YDir_Num++)
            {
                LCD_DrawPaint(Xpoint + XDir_Num - 1, Ypoint + YDir_Num - 1, Color);
            }
        }
    }
}
void Paint_DrawRectangle(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t Color, uint8_t Line_width, uint8_t Filled)
{
    if (Filled)
    {
        uint16_t Xpoint;
        for (Xpoint = Xstart; Xpoint < Xend; Xpoint++)
        {
            Paint_DrawLine(Xpoint, Ystart, Xpoint, Yend, Color, Line_width, 0);
        }
    }
    else
    {
        Paint_DrawLine(Xstart, Ystart, Xend, Ystart, Color, Line_width, 0);
        Paint_DrawLine(Xstart, Ystart, Xstart, Yend, Color, Line_width, 0);
        Paint_DrawLine(Xend, Yend, Xend, Ystart, Color, Line_width, 0);
        Paint_DrawLine(Xend, Yend, Xstart, Yend, Color, Line_width, 0);
    }
}

void Paint_DrawChar(uint16_t Xpoint, uint16_t Ypoint, const char Acsii_Char, uint16_t Color_Background, uint16_t Color_Foreground)
{

    uint16_t Page, Column;

    uint32_t Char_Offset = (Acsii_Char - ' ') * 24 * (17 / 8 + (17 % 8 ? 1 : 0));
    uint8_t ptr = 0;

    for (Page = 0; Page < 24; Page++)
    {
        for (Column = 0; Column < 17; Column++)
        {

            // To determine whether the font background color and screen background color is consistent
            if (FONT_BACKGROUND == Color_Background)
            { // this process is to speed up the scan
                if (Font24[Char_Offset+ptr] & (0x80 >> (Column % 8)))
                    LCD_DrawPaint(Xpoint + Column, Ypoint + Page, Color_Foreground);
            }
            else
            {
                if (Font24[Char_Offset+ptr] & (0x80 >> (Column % 8)))
                {
                    LCD_DrawPaint(Xpoint + Column, Ypoint + Page, Color_Foreground);
                }
                else
                {
                    LCD_DrawPaint(Xpoint + Column, Ypoint + Page, Color_Background);
                }
            }
            // One pixel is 8 bits
            if (Column % 8 == 7)
            {
                ptr++;
            }
        } /* Write a line */
        if (17 % 8 != 0)
        {
            ptr++;
        }
    } /* Write all */
}

void Paint_DrawString_EN(uint16_t Xstart, uint16_t Ystart, const char * pString, uint16_t Color_Background, uint16_t Color_Foreground )
{
  uint16_t Xpoint = Xstart;
  uint16_t Ypoint = Ystart;
  uint16_t i = 0;

  while (pString[i] != '\0') {

    Paint_DrawChar(Xpoint, Ypoint, pString[i], Color_Background, Color_Foreground);

    //The next character of the address
    i++;

    //The next word of the abscissa increases the font of the broadband
    Xpoint += 17;
  }
}