#pragma once
#include <stdint.h>

const int flashPersistSector_DataSize = 4096;

void flashPersistSector_initialize();
void flashPersistSector_read(uint8_t *bytes4096);
void flashPersistSector_write(uint8_t *bytes4096);
