#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "freertos/idf_additions.h"

#ifndef WIFI_SSID
#define WIFI_SSID "Example WiFi SSID"
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "Example WiFi Password"
#endif

static EventGroupHandle_t event_group_wifi;
static void handler_wifi_sta_start(void *, esp_event_base_t, int32_t, void *);
static void handler_wifi_sta_disconnected(void *, esp_event_base_t, int32_t,
                                          void *);
static void handler_ip_sta_got_ip(void *, esp_event_base_t, int32_t, void *);
void wifi_init() {
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_t *sta = esp_netif_create_default_wifi_sta();
  assert(sta); // not null

  wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, WIFI_EVENT_STA_START, &handler_wifi_sta_start, NULL, NULL));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &handler_wifi_sta_disconnected,
      NULL, NULL));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      IP_EVENT, IP_EVENT_STA_GOT_IP, &handler_ip_sta_got_ip, NULL, NULL));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  wifi_config_t wifi_sta_config = {
      .sta = {
          .ssid = WIFI_SSID,
          .password = WIFI_PASSWORD,
          // .channel = 1,
          // .bssid = {0xd6, 0x35, 0x1d, 0xbd, 0x30, 0xea},
          // .bssid_set = 1,
          .threshold.authmode = WIFI_AUTH_WPA2_PSK,
          .threshold.rssi = -80,
          // .sae_pwe_h2e = WPA3_SAE_PWE_HUNT_AND_PECK,
          // .sae_h2e_identifier = "",
      }};
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_sta_config));
  ESP_ERROR_CHECK(esp_wifi_start());
}
static void handler_wifi_sta_start(void *arg, esp_event_base_t event_base,
                                   int32_t event_id, void *event_data) {
  esp_wifi_connect();
  ESP_LOGI("wifi", "Connecting...");
}
static void handler_wifi_sta_disconnected(void *arg,
                                          esp_event_base_t event_base,
                                          int32_t event_id, void *event_data) {
  ESP_LOGI("wifi", "Disconnected.");
  // ESP_LOGI("wifi", "Waiting 1 second before attempting to reconnect...");
  // vTaskDelay(pdMS_TO_TICKS(1000));
  ESP_LOGI("wifi", "Reconnecting...");
  esp_wifi_connect();
}
static void handler_ip_sta_got_ip(void *arg, esp_event_base_t event_base,
                                  int32_t event_id, void *event_data) {
  ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
  ESP_LOGI("wifi", "Connected. IP: " IPSTR, IP2STR(&event->ip_info.ip));
}
