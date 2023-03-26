#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "MCP23017.h"

#define MCP23017_BASE_ADDRESS 0x40

void mcp23017_init(MCP23017 *obj, uint8_t address)
{

    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    
	obj->address = MCP23017_BASE_ADDRESS + address;
	
	//	set addressing style
	mcp23017_write_register(obj,MCP23017_IOCON,0x00);
	
	obj->data = 0x0000;
	mcp23017_write_word(obj,MCP23017_IODIRA);	//	make all output
}
void mcp23017_write(MCP23017 *obj)
{
	mcp23017_write_word(obj,MCP23017_GPIOA);
}

void mcp23017_write_register( MCP23017 *obj, uint8_t reg, uint8_t data)
{
    // I2C write process expects a control byte followed by data
    // this "data" can be a command or data to follow up a command

    // Co = 1, D/C = 0 => the driver expects a command
    uint8_t buf[2] = {reg, data};
    i2c_write_blocking(i2c_default, (obj->address & MCP23017_WRITE_MODE), buf, 2, false);

}

uint8_t mcp23017_read_register( MCP23017 *obj,uint8_t reg)
{
    uint8_t data = 0;
    i2c_read_blocking(i2c_default, (obj->address & MCP23017_READ_MODE), &data,1,false);
	return data;
}

void mcp23017_write_word( MCP23017 *obj, uint8_t reg )
{
    uint16_t data = obj->data;
    uint8_t buf[3] = {reg, (uint8_t)data, (uint8_t)(data>>8)};
    i2c_write_blocking(i2c_default, (obj->address & MCP23017_WRITE_MODE), buf, 3, false);
}