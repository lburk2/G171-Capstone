#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "OLED.h"
#include "font.h"

uint8_t g_lineNum = 0;
uint8_t g_cursorLocation = 0;

void oled_init() {
    // some of these commands are not strictly necessary as the reset
    // process defaults to some of these but they are shown here
    // to demonstrate what the initialization sequence looks like

    // some configuration values are recommended by the board manufacturer
    struct render_area frame_area = {start_col: 0, end_col : OLED_WIDTH - 1, start_page : 0, end_page : OLED_NUM_PAGES -
                                                                                                        1};

    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(2, GPIO_FUNC_I2C);
    gpio_set_function(3, GPIO_FUNC_I2C);

    oled_send_cmd(OLED_SET_DISP | 0x00); // set display off

    /* memory mapping */
    oled_send_cmd(OLED_SET_MEM_ADDR); // set memory address mode
    oled_send_cmd(0x01); // vertical addressing mode

    /* resolution and layout */
    oled_send_cmd(OLED_SET_DISP_START_LINE); // set display start line to 0

    oled_send_cmd(OLED_SET_SEG_REMAP | 0x01); // set segment re-map
    // column address 127 is mapped to SEG0

    oled_send_cmd(OLED_SET_MUX_RATIO); // set multiplex ratio
    oled_send_cmd(OLED_HEIGHT - 1); // our display is only 32 pixels high

    oled_send_cmd(OLED_SET_COM_OUT_DIR | 0x00); // set COM (common) output scan direction
    // scan from top to bottom, COM0 to COM[N-1]

    oled_send_cmd(OLED_SET_DISP_OFFSET); // set display offset
    oled_send_cmd(0x00); // no offset

    oled_send_cmd(OLED_SET_COM_PIN_CFG); // set COM (common) pins hardware configuration
    oled_send_cmd(0x02); // manufacturer magic number

    /* timing and driving scheme */
    oled_send_cmd(OLED_SET_DISP_CLK_DIV); // set display clock divide ratio
    oled_send_cmd(0x80); // div ratio of 1, standard freq

    oled_send_cmd(OLED_SET_PRECHARGE); // set pre-charge period
    oled_send_cmd(0xF1); // Vcc internally generated on our board

    oled_send_cmd(OLED_SET_VCOM_DESEL); // set VCOMH deselect level
    oled_send_cmd(0x30); // 0.83xVcc

    /* display */
    oled_send_cmd(OLED_SET_CONTRAST); // set contrast control
    oled_send_cmd(0xFF);

    oled_send_cmd(OLED_SET_ENTIRE_ON); // set entire display on to follow RAM content

    oled_send_cmd(OLED_SET_NORM_INV); // set normal (not inverted) display

    oled_send_cmd(OLED_SET_CHARGE_PUMP); // set charge pump
    oled_send_cmd(0x14); // Vcc internally generated on our board

    oled_send_cmd(OLED_SET_SCROLL | 0x00); // deactivate horizontal scrolling if set
    // this is necessary as memory writes will corrupt if scrolling was enabled

    oled_send_cmd(OLED_SET_DISP | 0x01); // turn display on

    calc_render_area_buflen(&frame_area);

    uint8_t buf[OLED_BUF_LEN];
    oled_fill(buf,0x00);
    oled_render(buf, &frame_area);
}

void oled_fill(uint8_t buf[], uint8_t fill) {
    // fill entire buffer with the same byte
    for (int i = 0; i < 32*128; i++) {
        buf[i] = fill;
    }
};

void oled_render(uint8_t *buf, struct render_area *area) {
    // update a portion of the display with a render area
    oled_send_cmd(OLED_SET_COL_ADDR);
    oled_send_cmd(area->start_col);
    oled_send_cmd(area->end_col);

    oled_send_cmd(OLED_SET_PAGE_ADDR);
    oled_send_cmd(area->start_page);
    oled_send_cmd(area->end_page);

    oled_send_buf(buf, area->buflen);
}


void oled_send_cmd(uint8_t cmd) {
    // I2C write process expects a control byte followed by data
    // this "data" can be a command or data to follow up a command

    // Co = 1, D/C = 0 => the driver expects a command
    uint8_t buf[2] = {0x80, cmd};
    i2c_write_blocking(i2c_default, (OLED_ADDR & OLED_WRITE_MODE), buf, 2, false);
}
void oled_send_buf(uint8_t buf[], int buflen) {
    // in horizontal addressing mode, the column address pointer auto-increments
    // and then wraps around to the next page, so we can send the entire frame
    // buffer in one gooooooo!

    uint8_t *temp_buf = malloc(buflen + 1);

    for (int i = 1; i < buflen + 1; i++) {
        temp_buf[i] = buf[i - 1];
    }
    // Co = 0, D/C = 1 => the driver expects data to be written to RAM
    temp_buf[0] = 0x40;
    i2c_write_blocking(i2c_default, (OLED_ADDR & OLED_WRITE_MODE), temp_buf, buflen + 1, false);

    free(temp_buf);
}
void calc_render_area_buflen(struct render_area *area) {
    // calculate how long the flattened buffer will be for a render area
    area->buflen = (area->end_col - area->start_col + 1) * (area->end_page - area->start_page + 1);
}
void oled_setCursor(uint8_t x, uint8_t y){
    // set column start and end address
    oled_send_cmd(0x21);
    oled_send_cmd(x);
    oled_send_cmd(127);
    // set page adderss
    oled_send_cmd(0x22);
    oled_send_cmd(y);
    oled_send_cmd(7);
    // set start line to 0
    uint8_t buf[1] = {0x40};
    i2c_write_blocking(i2c_default, (OLED_ADDR & OLED_WRITE_MODE), buf, 1, false);

    g_lineNum = x;
    g_cursorLocation = y;
}
void oled_WriteCharacter(char character)
{ // check for null terminator
    if (character != '\n')
    {
        character = character - 0x20;
        uint8_t i = 1;
    
        uint8_t buf[9];
        buf[0] = 0x40;

        while(i<9)
        {
            buf[i] = ASCII[(uint8_t)character][i-1];
            g_cursorLocation++;
            i++;
        }

        i2c_write_blocking(i2c_default, (OLED_ADDR & OLED_WRITE_MODE), buf, 9, false);
    }
}
void oled_WriteString(char *msg)
{

    uint8_t i = 0;

    while (msg[i] != '\n')
    {
        oled_WriteCharacter(msg[i]);
        i++;
    }
}
void oled_WriteCharacter16(char character)
{ // check for null terminator
    if (character != '\n')
    {
        character = character - 0x20;
        uint8_t i = 1;
        uint8_t j = 0;

        uint8_t buf1[17];
        buf1[0] = 0x40;

        while(i<17)
        {
            buf1[i] = ASCII16[(uint8_t)character][j];
            g_cursorLocation++;
            i++;
            j++;
        }
        oled_send_cmd(OLED_SET_COL_ADDR);
        oled_send_cmd(g_lineNum);
        oled_send_cmd(g_lineNum+7);

        oled_send_cmd(OLED_SET_PAGE_ADDR);
        oled_send_cmd(0x01);
        oled_send_cmd(0x02);

        oled_send_buf(buf1, 17);
    }
}
void oled_WriteString16(char *msg)
{

    uint8_t i = 0;

    while (msg[i] != '\n')
    {
        oled_WriteCharacter16(msg[i]);
        i++;
        g_lineNum+=8;
    }
}