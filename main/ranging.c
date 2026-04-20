// External includes
#include "driver/i2c.h"
#include "vl53l0x_api.h"

// Local includes
#include "ranging.h"

#define I2C_PORT I2C_NUM_0
#define SDA_PIN 14
#define SCL_PIN 15

static VL53L0X_Dev_t dev;

void ranging_init(void) {
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

  // VL53L0X sensor init
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

  status = VL53L0X_SetDeviceMode(&dev, VL53L0X_DEVICEMODE_SINGLE_RANGING);
  if (status != VL53L0X_ERROR_NONE) {
    ESP_LOGE("ranging", "SetDeviceMode failed: %d", status);
    return;
  }

  ESP_LOGI("ranging", "VL53L0X ready");
}

int32_t ranging_get_mm(void) {
  VL53L0X_RangingMeasurementData_t data;
  VL53L0X_PerformSingleRangingMeasurement(&dev, &data);

  if (data.RangeStatus != 0) {
    ESP_LOGW("ranging", "Range status: %d", data.RangeStatus);
    return -1;
  }

  return data.RangeMilliMeter;
}
