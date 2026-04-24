idf_component_register(
    SRCS
        "intruder_detector.c"
        "ranging.c"
        "camera.c"
        "network.c"
    INCLUDE_DIRS "."
    REQUIRES
        "esp_driver_gpio"
        "nvs_flash"
        "esp_wifi"
        "wpa_supplicant"
        "esp_http_client"
)
