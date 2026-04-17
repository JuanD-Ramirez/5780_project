#pragma once

// Standard includes
#include <stddef.h>
#include <stdint.h>

void camera_init(void);
uint8_t *camera_capture(size_t *len);
void camera_free(uint8_t *data);
