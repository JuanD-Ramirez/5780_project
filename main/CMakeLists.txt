idf_component_register(
    SRCS
        "intruder_detector.c"
        "network.c"
        "camera.c"
    INCLUDE_DIRS "."
    REQUIRES
        "esp_wifi"
        "esp_http_client"
        "wpa_supplicant"
        "nvs_flash"
)
