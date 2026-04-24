// External includes
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "nvs_flash.h"

// Local includes
#include "camera.h"
#include "network.h"
#include "ranging.h"

const char *wifi_ssid = "main";
const char *wifi_password = "davekitchenbob";
const char *url = "https://www.duranaero.com/upload";
const char *cert =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDhDCCAwugAwIBAgISBth2E/2vgcUL44KdyyBo0wAHMAoGCCqGSM49BAMDMDIx\n"
    "CzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MQswCQYDVQQDEwJF\n"
    "ODAeFw0yNjAzMTUwNDU1MjJaFw0yNjA2MTMwNDU1MjFaMBwxGjAYBgNVBAMTEXd3\n"
    "dy5kdXJhbmFlcm8uY29tMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEix49V3lE\n"
    "tNfLzxPupMiiEYdLtfsiq5gg0hOAwM2ytbcZn2RXwng8Sr8QgJ45pKRH19VxOvAe\n"
    "/W+6zjVfhuG5+aOCAhUwggIRMA4GA1UdDwEB/wQEAwIHgDATBgNVHSUEDDAKBggr\n"
    "BgEFBQcDATAMBgNVHRMBAf8EAjAAMB0GA1UdDgQWBBRWCBFMTFI+q+jqmqH37SbN\n"
    "FaZ6zTAfBgNVHSMEGDAWgBSPDROi9i5+0VBsMxg4XVmOI3KRyjAyBggrBgEFBQcB\n"
    "AQQmMCQwIgYIKwYBBQUHMAKGFmh0dHA6Ly9lOC5pLmxlbmNyLm9yZy8wHAYDVR0R\n"
    "BBUwE4IRd3d3LmR1cmFuYWVyby5jb20wEwYDVR0gBAwwCjAIBgZngQwBAgEwLAYD\n"
    "VR0fBCUwIzAhoB+gHYYbaHR0cDovL2U4LmMubGVuY3Iub3JnLzguY3JsMIIBBQYK\n"
    "KwYBBAHWeQIEAgSB9gSB8wDxAHYAZBHEbKQS7KeJHKICLgC8q08oB9QeNSer6v7V\n"
    "A8l9zfAAAAGc8A7eZQAABAMARzBFAiEAg6I78FV2R7rZxr5G09BJEJHxQbb7pBu2\n"
    "TOLi8UIUiSMCIDjzz9DvfVrYq++AQVQUkt2evm+MPntmvMFbR5qmjHR3AHcAyzj3\n"
    "FYl8hKFEX1vB3fvJbvKaWc1HCmkFhbDLFMMUWOcAAAGc8A7egQAABAMASDBGAiEA\n"
    "sgWm+wAaYupIMFJgLCErCglq/laE6D/f6ytSFfPss70CIQDpiL+JBfdr2Lu4/KfB\n"
    "DbDkzu6ciAD9RHlmQ6XubGwB9TAKBggqhkjOPQQDAwNnADBkAjB+qCArgymh4y/s\n"
    "Yof20eTaeF+B9XIvbf+pBWOWFdNxKir+ZfW/6bxefLTqt4kVPOwCMFMhMHzwUc5F\n"
    "LbjbdxDORZGmEGY33Jlrv+rLNoAZ5khruQa+siK23L/DRawIetosXA==\n"
    "-----END CERTIFICATE-----\n"
    "-----BEGIN CERTIFICATE-----\n"
    "MIIEVjCCAj6gAwIBAgIQY5WTY8JOcIJxWRi/w9ftVjANBgkqhkiG9w0BAQsFADBP\n"
    "MQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJuZXQgU2VjdXJpdHkgUmVzZWFy\n"
    "Y2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBYMTAeFw0yNDAzMTMwMDAwMDBa\n"
    "Fw0yNzAzMTIyMzU5NTlaMDIxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBF\n"
    "bmNyeXB0MQswCQYDVQQDEwJFODB2MBAGByqGSM49AgEGBSuBBAAiA2IABNFl8l7c\n"
    "S7QMApzSsvru6WyrOq44ofTUOTIzxULUzDMMNMchIJBwXOhiLxxxs0LXeb5GDcHb\n"
    "R6EToMffgSZjO9SNHfY9gjMy9vQr5/WWOrQTZxh7az6NSNnq3u2ubT6HTKOB+DCB\n"
    "9TAOBgNVHQ8BAf8EBAMCAYYwHQYDVR0lBBYwFAYIKwYBBQUHAwIGCCsGAQUFBwMB\n"
    "MBIGA1UdEwEB/wQIMAYBAf8CAQAwHQYDVR0OBBYEFI8NE6L2Ln7RUGwzGDhdWY4j\n"
    "cpHKMB8GA1UdIwQYMBaAFHm0WeZ7tuXkAXOACIjIGlj26ZtuMDIGCCsGAQUFBwEB\n"
    "BCYwJDAiBggrBgEFBQcwAoYWaHR0cDovL3gxLmkubGVuY3Iub3JnLzATBgNVHSAE\n"
    "DDAKMAgGBmeBDAECATAnBgNVHR8EIDAeMBygGqAYhhZodHRwOi8veDEuYy5sZW5j\n"
    "ci5vcmcvMA0GCSqGSIb3DQEBCwUAA4ICAQBnE0hGINKsCYWi0Xx1ygxD5qihEjZ0\n"
    "RI3tTZz1wuATH3ZwYPIp97kWEayanD1j0cDhIYzy4CkDo2jB8D5t0a6zZWzlr98d\n"
    "AQFNh8uKJkIHdLShy+nUyeZxc5bNeMp1Lu0gSzE4McqfmNMvIpeiwWSYO9w82Ob8\n"
    "otvXcO2JUYi3svHIWRm3+707DUbL51XMcY2iZdlCq4Wa9nbuk3WTU4gr6LY8MzVA\n"
    "aDQG2+4U3eJ6qUF10bBnR1uuVyDYs9RhrwucRVnfuDj29CMLTsplM5f5wSV5hUpm\n"
    "Uwp/vV7M4w4aGunt74koX71n4EdagCsL/Yk5+mAQU0+tue0JOfAV/R6t1k+Xk9s2\n"
    "HMQFeoxppfzAVC04FdG9M+AC2JWxmFSt6BCuh3CEey3fE52Qrj9YM75rtvIjsm/1\n"
    "Hl+u//Wqxnu1ZQ4jpa+VpuZiGOlWrqSP9eogdOhCGisnyewWJwRQOqK16wiGyZeR\n"
    "xs/Bekw65vwSIaVkBruPiTfMOo0Zh4gVa8/qJgMbJbyrwwG97z/PRgmLKCDl8z3d\n"
    "tA0Z7qq7fta0Gl24uyuB05dqI5J1LvAzKuWdIjT1tP8qCoxSE/xpix8hX2dt3h+/\n"
    "jujUgFPFZ0EVZ0xSyBNRF3MboGZnYXFUxpNjTWPKpagDHJQmqrAcDmWJnMsFY3jS\n"
    "u1igv3OefnWjSQ==\n"
    "-----END CERTIFICATE-----\n";
const char *api_key = "super-secret-api-key-that-no-one-will-guess";

void indicate_init(void) {
  gpio_num_t led_num = GPIO_NUM_33;
  gpio_config_t led_gpio_config = {
      .pin_bit_mask = (1ULL << led_num),
      .mode = GPIO_MODE_OUTPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };
  ESP_ERROR_CHECK(gpio_config(&led_gpio_config));
  ESP_ERROR_CHECK(gpio_set_level(led_num, 0)); // reversed logic, 0 == ON
}

void nvs_init() { ESP_ERROR_CHECK(nvs_flash_init()); }

void app_main(void) {
  indicate_init();
  nvs_init();
  wifi_init(wifi_ssid, wifi_password);
  camera_init();
  ranging_init();

  ESP_LOGI("main", "Ready.");

  int32_t prev_range = -1;

  while (1) {
    vTaskDelay(pdMS_TO_TICKS(200));

    int32_t range_mm = ranging_get_mm();
    if (range_mm < 0) {
      continue;
    }

    if (prev_range == -1) {
      prev_range = range_mm;
      continue;
    }

    int32_t diff = prev_range - range_mm;
    if (diff < 0) {
      diff = -diff;
    }

    if (diff < 100) {
      continue;
    }

    size_t image_size = 0;
    uint8_t *image = camera_capture(&image_size);
    if (!image) {
      ESP_LOGE("main", "Failed to capture image.");
      esp_restart();
    }
    upload_image(image, image_size, url, cert, api_key);
    camera_free(image);

    prev_range = range_mm;
  }
}
