// External includes
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "nvs_flash.h"

// Local includes
#include "camera.h"
#include "network.h"

void nvs_init() { ESP_ERROR_CHECK(nvs_flash_init()); }

void app_main(void) {
  // indicate_init();

  const char *wifi_ssid = "main";
  const char *wifi_password = "davekitchenbob";
  nvs_init();
  wifi_init(wifi_ssid, wifi_password);

  camera_init();
  size_t image_size = 0;
  uint8_t *image = camera_capture(&image_size);
  if (!image) {
    ESP_LOGE("main", "Failed to capture image.");
    esp_restart();
  }
  camera_free(image);

  ESP_LOGI("main", "Done.");

  while (1) {
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}
