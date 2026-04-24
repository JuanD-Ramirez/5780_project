// Standard includes
#include <string.h>

// External includes
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "vl53l0x_api.h"

// Local includes
#include "ranging.h"

#define I2C_PORT I2C_NUM_0
#define SDA_PIN 3
#define SCL_PIN 1
#define INT_PIN 15

#define THRESH_LOW_MM 50
#define THRESH_HIGH_MM 4000

static VL53L0X_Dev_t dev;
static SemaphoreHandle_t range_sem;

static void IRAM_ATTR int_pin_isr(void *arg) {
  BaseType_t higher_prio_woken = pdFALSE;
  xSemaphoreGiveFromISR(range_sem, &higher_prio_woken);
  portYIELD_FROM_ISR(higher_prio_woken);
}

static void gpio_interrupt_init(void) {
  gpio_config_t cfg = {
      .pin_bit_mask = (1ULL << INT_PIN),
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_ENABLE, // INT active-low, open-drain
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_NEGEDGE, // VL53L0X pulls low on interrupt
  };
  ESP_ERROR_CHECK(gpio_config(&cfg));
  ESP_ERROR_CHECK(gpio_install_isr_service(0));
  ESP_ERROR_CHECK(gpio_isr_handler_add(INT_PIN, int_pin_isr, NULL));
}

void ranging_init(void) {
  range_sem = xSemaphoreCreateBinary();

  // I2C
  i2c_config_t conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = SDA_PIN,
      .scl_io_num = SCL_PIN,
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
      .master.clk_speed = 100000,
  };
  ESP_ERROR_CHECK(i2c_param_config(I2C_PORT, &conf));
  ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0));

  // VL53L0X init
  memset(&dev, 0, sizeof(dev));
  dev.i2c_address = 0x29;
  dev.i2c_port_num = I2C_PORT;

  VL53L0X_Error status;

  status = VL53L0X_DataInit(&dev);
  if (status != VL53L0X_ERROR_NONE) {
    ESP_LOGE("ranging", "DataInit failed: %d", status);
    return;
  }
  status = VL53L0X_StaticInit(&dev);
  if (status != VL53L0X_ERROR_NONE) {
    ESP_LOGE("ranging", "StaticInit failed: %d", status);
    return;
  }

  uint8_t vhvSettings, phaseCal;
  status = VL53L0X_PerformRefCalibration(&dev, &vhvSettings, &phaseCal);
  if (status != VL53L0X_ERROR_NONE) {
    ESP_LOGE("ranging", "RefCalibration failed: %d", status);
    return;
  }

  uint32_t refSpadCount;
  uint8_t isApertureSpads;
  status =
      VL53L0X_PerformRefSpadManagement(&dev, &refSpadCount, &isApertureSpads);
  if (status != VL53L0X_ERROR_NONE) {
    ESP_LOGE("ranging", "SpadManagement failed: %d", status);
    return;
  }

  // Continuous mode so the sensor keeps measuring and can fire interrupts
  status = VL53L0X_SetDeviceMode(&dev, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);
  if (status != VL53L0X_ERROR_NONE) {
    ESP_LOGE("ranging", "SetDeviceMode failed: %d", status);
    return;
  }

  // Use GPIO interrupt (not polling) triggered by threshold crossing
  status = VL53L0X_SetInterruptThresholds(
      &dev, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING,
      (FixPoint1616_t)(THRESH_LOW_MM << 16),
      (FixPoint1616_t)(THRESH_HIGH_MM << 16));
  if (status != VL53L0X_ERROR_NONE) {
    ESP_LOGE("ranging", "SetInterruptThresholds failed: %d", status);
    return;
  }

  // Route the interrupt condition to the GPIO pin
  status =
      VL53L0X_SetGpioConfig(&dev,
                            0, // pin index
                            VL53L0X_DEVICEMODE_CONTINUOUS_RANGING,
                            VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_OUT,
                            VL53L0X_INTERRUPTPOLARITY_LOW // active-low
      );
  if (status != VL53L0X_ERROR_NONE) {
    ESP_LOGE("ranging", "SetGpioConfig failed: %d", status);
    return;
  }

  gpio_interrupt_init();

  // Clear stale interrupts
  VL53L0X_ClearInterruptMask(&dev,
                             VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_OUT_OF_WINDOW);

  status = VL53L0X_StartMeasurement(&dev);
  if (status != VL53L0X_ERROR_NONE) {
    ESP_LOGE("ranging", "StartMeasurement failed: %d", status);
    return;
  }

  ESP_LOGI("ranging",
           "VL53L0X ready, waiting for threshold interrupts on GPIO%d",
           INT_PIN);
}

int32_t ranging_wait_for_threshold(void) {
  // Block indefinitely until the ISR fires
  xSemaphoreTake(range_sem, portMAX_DELAY);

  VL53L0X_RangingMeasurementData_t data;
  VL53L0X_Error status = VL53L0X_GetRangingMeasurementData(&dev, &data);

  // Reset the interrupt
  VL53L0X_ClearInterruptMask(&dev,
                             VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_OUT_OF_WINDOW);

  if (status != VL53L0X_ERROR_NONE) {
    ESP_LOGE("ranging", "GetRangingMeasurementData failed: %d", status);
    return -1;
  }
  if (data.RangeStatus != 0) {
    ESP_LOGW("ranging", "Range status: %d", data.RangeStatus);
    return -1;
  }

  return data.RangeMilliMeter;
}
