idf_component_register(
    SRCS
        "intruder-detector.c"
    INCLUDE_DIRS "."
    REQUIRES
        "esp_driver_ledc"
        "esp_wifi"
        "wpa_supplicant"
        "nvs_flash"
)
