idf_component_register(
    SRCS
        "intruder_detector.c"
        "ranging.c"
        "camera.c"
        "network.c"
    INCLUDE_DIRS "."
    REQUIRES
        "esp_wifi"
        "esp_http_client"
        "wpa_supplicant"
        "nvs_flash"
)
