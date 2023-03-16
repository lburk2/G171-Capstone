#include <stdint.h>

void SD_init(void);
void write_to_sdcard(const char* filename, const char* data, uint32_t address);
char* read_from_sdcard(const char* filename, uint32_t address);