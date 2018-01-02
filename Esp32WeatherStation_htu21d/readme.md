## SHT21/HTU21D for Temperature, Humidity

The HTU21D aka. SHT21 is a I2C digital Temperature / humidity sensor with a typical accuracy of ±2% with an operating range that's optimized from 5% to 95% RH, temperature output has an accuracy of ±1°C from -30~90°C. 

THe sensor is implemented as a reusable class which will use the sensor and expose the environmental factors (namely temperature and humidity). 

#### Usage
    HTU21D htu;
    bool b = htu.begin(25, 26);
    if (b)
    {
      Serial.printf("Temp:%f, Humid:%f, dew:%f, HI%f \n", htu.getTemperature(), htu.getHumidity(), htu.getDewPoint(), htu.getHeatIndex());
    }

`begin` can pass the ESP32 IO pin numbers to be used as SDA and SCL lines for I2C communication. 

The measurements are retunrned in metric units, i.e. `Temperature` in `Celcius` and `Relative Humidity` in  `Percentage`.