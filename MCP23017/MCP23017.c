#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "MCP23017.h"

#define MCP23017_BASE_ADDRESS 0x20

void mcp23017_init(MCP23017 *obj, uint8_t address)
{

    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(2, GPIO_FUNC_I2C);
    gpio_set_function(3, GPIO_FUNC_I2C);
    gpio_pull_up(2);
    gpio_pull_up(3);

	obj->address = MCP23017_BASE_ADDRESS + address;
	
	//	set addressing style
	mcp23017_write_register(obj,MCP23017_IOCON,0x00);
	
	obj->data = 0xFFFF;
	mcp23017_write_word(obj, MCP23017_IODIRB);	//	make all output
    mcp23017_write_word(obj, MCP23017_GPPUA);	//	make all output
    obj->data = 0xFFFF;
	mcp23017_write_word(obj, MCP23017_GPINTENA);	//	enable interrupts+
    mcp23017_write_word(obj, MCP23017_GPINTENB);	//	enable interrupts
	mcp23017_write_word(obj, MCP23017_INTCAPA);	//	NTERRUPT CAPTURED VALUE FOR PORT REGISTER 
    mcp23017_write_word(obj, MCP23017_INTCAPB);	//	NTERRUPT CAPTURED VALUE FOR PORT REGISTER 
}
void mcp23017_write(MCP23017 *obj)
{
	mcp23017_write_word(obj,MCP23017_GPIOB);
}

void mcp23017_write_register( MCP23017 *obj, uint8_t reg, uint8_t data)
{
    // I2C write process expects a control byte followed by data
    // this "data" can be a command or data to follow up a command

    // Co = 1, D/C = 0 => the driver expects a command
    uint8_t buf[2] = {reg, data};
    i2c_write_blocking(i2c1, (obj->address & MCP23017_WRITE_MODE), buf, 2, false);

}

uint8_t mcp23017_read_register( MCP23017 *obj,uint8_t reg)
{
    // uint8_t data = reg;
    // i2c_read_blocking(i2c1, (obj->address & MCP23017_READ_MODE), &data,1,false);
	// return data;
    i2c_write_blocking(i2c1, obj->address , &reg, 1, true);
	uint8_t data;
	i2c_read_blocking(i2c1, obj->address , &data, 1, false);
	return data;
}

void mcp23017_write_word( MCP23017 *obj, uint8_t reg )
{
    uint16_t data = obj->data;
    uint8_t buf[3] = {reg, (uint8_t)data, (uint8_t)(data>>8)};
    i2c_write_blocking(i2c1, (obj->address & MCP23017_WRITE_MODE), buf, 3, false);
}