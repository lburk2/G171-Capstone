#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

// Pin definitions
#define CS_PIN 20
#define SCK_PIN 18
#define MOSI_PIN 19
#define MISO_PIN 16

void SD_init(void){
     // Ports
    spi_inst_t *spi = spi0;
    // Initialize SPI interface
    spi_init(spi, 1000000);
    gpio_set_function(SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(MISO_PIN, GPIO_FUNC_SPI);

    // Set CS pin as output and low
    gpio_init(CS_PIN);
    gpio_set_dir(CS_PIN, GPIO_OUT);
    gpio_put(CS_PIN, 0);

}


// Function to write data to SD card
void write_to_sdcard(const char* filename, const char* data, uint32_t address) {
    // Ports
    spi_inst_t *spi = spi0;
    // Send command to write to file at specified address
    char command[20];
    sprintf(command, "W %s %ld", filename, address);
    spi_write_blocking(spi, command, strlen(command));

    // Write data to file
    spi_write_blocking(spi, data, strlen(data));

    // Set CS pin as output and high
    gpio_put(CS_PIN, 1);
}

// Function to read data from SD card
char* read_from_sdcard(const char* filename, uint32_t address) {
    // Ports
    spi_inst_t *spi = spi0;
    // Send command to read from file at specified address
    char command[20];
    sprintf(command, "R %s %ld", filename, address);
    spi_write_blocking(spi, command, strlen(command));

    // Read data from file
    char buffer[100] = {0};
    size_t i = 0;
    
    buffer[i] = spi_read_blocking(spi, 0x00, buffer, 1);
    // Set CS pin as output and high
    gpio_put(CS_PIN, 1);

    // Allocate memory for buffer and return
    char* data = malloc(i+1);
    memcpy(data, buffer, i+1);
    return data;
}