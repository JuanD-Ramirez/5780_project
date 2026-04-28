# Multipurpose Camera Module
### Contributors
- JD Ramirez
- Abed Mbarushimana
- Caden Shmookler
- Clare Keeler

### Project Overview
This project consists of a motion activated camera that takes a photo when movement is detected via a laser range-finder. This camera uses the [VL53L0X](https://www.amazon.com/Qoroos-VL53L0X-Breakout-GY-VL53L0XV2-Measurement/dp/B0F1CXB42P/ref=sr_1_1_sspa?dib=eyJ2IjoiMSJ9.mW5oIEhy_KPhE4uwQJ5HG7OABGuAiHQ_MPf9F89My1-c4qGvjm5TfSTPt8h4820ej2voMGvcUDSqjgotm_abmRsvclNaoNaAFewcxPKh2NRQCaWaojJMVEDc2q1iD1XlhsBaxracdkTUujnr7hjzjvYVdYXGtkOFCUvkDpTkzl97b9Z-YjMNt7Gzr34J7MtPZvRdXqfSGvm-yqrOvODq8-ejVbd5r9qOKdJsYLpxIBU.M_Sriulo9jpBWnCtI54bohogtjjk9g5T52h28kYJtBk&dib_tag=se&keywords=vl53l0x&qid=1777338334&sr=8-1-spons&sp_csd=d2lkZ2V0TmFtZT1zcF9hdGY&th=1) Time-of-Flight (ToF) module to measure the distance of objects and triggers an interrupt when the laser changes by a distance of 100mm. The device is controlled by the [ESP32-Cam module](https://components101.com/modules/esp32-cam-camera-module), which is a combined ESP32 and camera development board. The ESP32 was chosen for its ability to immediately post captured images to a remote server via WiFi. The ESP32 communicates with the ToF module and the onboard camera via I2C and the Digital Video Port (DVP). The ESP32-Cam module does not have an onboard USB port, so it must be programmed via a USB to UART adapter ([ESP32-CAM-MB](https://www.amazon.com/ESP32-CAM-MB-Aideepen-ESP32-CAM-Bluetooth-Arduino/dp/B0948ZFTQZ?th=1)). The camera module is powered via a portable battery which allows the system to be easily moved as needed.

![Block Diagram](/Users/clarekeeler/Alarm_laser/CameraBlockDiagram)

### Purpose
This project was originally developed as an intruder detection device, however, its modular architecture, low power requirements, and wireless communication capabilities make it adaptable to a range of alternative applications. The system’s portability allows it to be deployed in environments where fixed infrastructure is not feasible, while its ease of use reduces the need for extensive setup or technical expertise. One relevant application is its use as a trail camera for wildlife monitoring. In this context, the device’s wireless functionality enables remote data transmission, eliminating the need for manual retrieval of stored images or footage. Additionally, its compact and lightweight design allows it to be easily transported and installed in remote terrain. By integrating motion detection sensors with low-power imaging components, the system can selectively capture events of interest, conserving energy and extending operational lifespan compared to traditional trail cameras. This makes it especially suitable for long-term ecological studies or monitoring in hard-to-access locations.

### Setup

1. Follow wiring diagram:
![Wiring Diagram](/Users/clarekeeler/Alarm_laser/Cammoduleschematic)
2. Change wifi SSID and password in WHATEVER THE FILE IS NAMED HERE
3. Flash ESP-32-CAM module with repo
4. Ensure module is on by pressing the button. If there is a visible LED the module is on.
5. INSTRUCTIONS FOR SSH TO SERVER FOR VIEWING PHOTOS 


