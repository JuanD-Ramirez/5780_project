#pragma once

// Standard includes
#include <stddef.h>
#include <stdint.h>

// External includes
#include "esp_err.h"

void wifi_init(const char *ssid, const char *pass);
esp_err_t upload_image(uint8_t *data, size_t len, const char *url,
                       const char *cert, const char *api_key);
