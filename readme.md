## ESP32 Weather Station
Espressif ESP32 and I2C environment sensors to gather Temperature, Humidity and Pressure data. The data points gathereed are posted to an InfluxDB running in Intranet.

### Common Features
1. Connects to WiFi using a static IP to minimize awake time 
2. Automatically syncs with [SNTP server](pool.ntp.org) to post with accurate epoch time
2. Uses very accurate [calibrated ADC](https://esp-idf.readthedocs.io/en/latest/api-reference/peripherals/adc.html#adc-calibration) to monitor the battery connected, and prevents overdrain (cut off at 3.4V)
3. Configurable sampling frequency
4. Posts all measured values to InfluxDB (provided a DB named `environment` is pre-created), which can be easily visualized with grafana with provided templates.


### Hardware
1. [Adafruit Huzzah32 feather](https://www.adafruit.com/product/3405)
2. Nokia BL-5C Li-ion Battery 1020mAh 3.7V
3. HTU21D or BME280 weather sensors

### Software stack
1. [Espressif Arduino](https://github.com/espressif/arduino-esp32)


### [SHT21/HTU21D for Temperature, Humidity](https://github.com/AdysTech/Esp32-Weather/Esp32WeatherStation_htu21d)
The HTU21D aka. SHT21 is a I2C digital Temperature / humidity sensor with a typical accuracy of ±2% with an operating range that's optimized from 5% to 95% RH, temperature output has an accuracy of ±1°C from -30~90°C. 

### [BME280 for Temperature, Humidity and Pressure](https://github.com/AdysTech/Esp32-Weather/Esp32WeatherStation_bme280)
BME280 is precision multi sensor from Bosch for measuring humidity with ±3% accuracy, barometric pressure with ±1 hPa absolute accuraccy, and temperature with ±1.0°C accuracy

