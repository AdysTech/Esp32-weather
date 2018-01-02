## BME280 for Temperature, Humidity and Pressure

BME280 is precision multi sensor from Bosch for measuring humidity with ±3% accuracy, barometric pressure with ±1 hPa absolute accuraccy, and temperature with ±1.0°C accuracy

THe sensor is implemented as a reusable class which will use the sensor in **`forced`** mode reading the three environmental factors (namely temperature, humidity and pressure) once. 

#### Usage
    BME280 bme;
    bool b = bme.begin(25, 26);
    if (b)
    {
      Serial.printf("Temperature:%f C, Relative_Humidity:%f%%, pressure:%fhPa, Dew_Point:%fC, Heat_Index:%fC \n", bme.getTemperature(), bme.getHumidity(), bme.getPressure(), bme.getDewPoint(), bme.getHeatIndex());
    }

`begin` can pass the ESP32 IO pin numbers to be used as SDA and SCL lines for I2C communication. 

The measurements are retunrned in metric units, i.e. `Temperature` - `Celcius`, `Relative Humidity` in  `Percentage` and `Pressure` in `hectapascal`.