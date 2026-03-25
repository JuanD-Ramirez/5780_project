// External includes
#include "driver/gpio.h"
#include "esp_err.h"
#include "freertos/idf_additions.h"
#include "nvs_flash.h"

// // Local includes
// #define WIFI_SSID "Guest"
// #define WIFI_PASSWORD ""
// #include "wifi-driver.h"

void indicate_init(void) {
  gpio_num_t led_num = GPIO_NUM_8;
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
  // wifi_init();

  while (1) {
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}
