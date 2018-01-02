//Copyright @ Adys Tech
//Author : mvadu@adystech.com

#include "htu21d.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <esp_adc_cal.h>
#include <driver/adc.h>
#include <driver/gpio.h>
#include <esp_err.h>
#include <apps/sntp/sntp.h>
#include <inttypes.h>

//''''''''''''''''''global''''''''''''''''''''''''''''''
esp_adc_cal_characteristics_t characteristics;
RTC_DATA_ATTR static int bootCount = 0;
time_t now = 0;
//frequency of wake cycles
const long samplingFrequency = 300;
//''''''''''''''''''''''''''''''''''''''''''''''''''''''

bool ConnectToWiFi(int timeout)
{
  if (WiFi.status() == WL_CONNECTED)
    return true;

  unsigned long max = timeout * 1000;
  unsigned long t = millis();
  const char *ssid = "";
  const char *password = "";

  IPAddress local_IP(192, 168, 23, 135);
  IPAddress gateway(192, 168, 23, 1);
  IPAddress subnet(255, 255, 0, 0);
  IPAddress dns(192, 168, 23, 1);

  if (!WiFi.config(local_IP, gateway, subnet, dns, dns))
  {
    Serial.println("STA Failed to configure");
    return false;
  }

  WiFi.begin(ssid, password);
  while (max > millis() - t)
  {
    //digitalWrite(13, !digitalRead(13));
    ledcWrite(1, (millis() - t) * 4);
    if (WiFi.status() == WL_CONNECTED)
    {
      ledcWrite(1, 16384);
      break;
    }
    delay(50);
  }

  Serial.printf("After Connection loop: %d, duration: %d \n", WiFi.status(), (millis() - t));
  return (WiFi.status() == WL_CONNECTED);
}

bool DisconnectFromWiFi(int timeout)
{
  if (WiFi.status() == WL_NO_SHIELD)
    return true;
  unsigned long max = timeout * 1000;
  unsigned long t = millis();
  WiFi.disconnect(true);

  while (max > millis() - t)
  {
    if (WiFi.status() == WL_NO_SHIELD)
    {
      digitalWrite(13, LOW);
      break;
    }
    timeout--;
    delay(5);
  }

  Serial.printf("After disconnect loop: %d, duration: %d \n", WiFi.status(), (millis() - t));
  return (WiFi.status() == WL_NO_SHIELD);
}

//Syncs with ntp server, provided the time passed in is not a valid time or the forced flag is set.
long obtainTime(time_t *t, bool forced)
{
  const char *sntpServer = "pool.ntp.org";
  const long minEpoch = 1465345377; //seconds for Jun  8 2016 00:22:57, ESP32 default timestamp
  time(t);

  long oldTime = *t > minEpoch ? *t : minEpoch;

  if (*t < minEpoch || forced)
  {
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, (char *)sntpServer);
    sntp_init();
    // wait for time to be set
    unsigned int timeout = 10 * 1000;
    unsigned int m = millis();
    while (*t < minEpoch && (timeout > millis() - m))
    {
      delay(5);
      time(t);
    }
    Serial.printf("Time received %d\n", *t);
  }
  return *t - oldTime;
}

float readBatteryVoltage()
{
  // Read ADC and obtain result in mV, its from a voltage devider, so should be multiplied by 2
  return adc1_to_voltage(ADC1_CHANNEL_7, &characteristics) * 2.0 / 1000.0;
}

bool PostToInflux(String post)
{
  HTTPClient http;
  http.begin("http://raspberrypi:8086/write?db=environment&precision=s");
  http.addHeader("Content-Type", "text/json");

  int httpResponseCode = http.POST(post);
  http.end();

  Serial.printf("%d : %s \n", httpResponseCode, post);
  return httpResponseCode == 204;
}

void setup()
{
  unsigned long startTime = millis();

  //used for battery reading
  //https://esp-idf.readthedocs.io/en/latest/api-reference/peripherals/adc.html#adc-calibration
  // ESP32 0x4485 chip read 1090mV
  // Configure ADC
  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_11db);

  // Calculate ADC characteristics i.e. gain and offset factors
  esp_adc_cal_get_characteristics(1090, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, &characteristics);

  Serial.begin(115200);
  pinMode(13, OUTPUT); // set the LED pin mode

  ledcAttachPin(13, 1);
  ledcSetup(1, 12000, 16);

  Serial.printf("Boot count %d \n", ++bootCount);
  char value[256];
  float vBat = readBatteryVoltage();
  Serial.printf("Battery Voltage: %f V\n", vBat);
  if (vBat > 3.4)
  {
    ConnectToWiFi(10);

    int drift = obtainTime(&now, bootCount % (24 * 3600 / samplingFrequency) == 0);
    uint64_t chipid = ESP.getEfuseMac();

    Serial.printf("Time Drift: %d sec\n", drift);

    if (drift != 0 && bootCount > 0)
    {
      sprintf(value, "rtc,board=0x%04X%08X drift=%f,bootcount=%di %d", (uint16_t)(chipid >> 32), (uint32_t)chipid, drift, bootCount, now);
      PostToInflux(value);
    }
    HTU21D htu;
    bool b = htu.begin(25, 26);
    if (b)
    {
      Serial.printf("Temp:%f, Humid:%f, dew:%f, HI%f \n", htu.getTemperature(), htu.getHumidity(), htu.getDewPoint(), htu.getHeatIndex());
      sprintf(value, "weather,board=0x%04X%08X,sensor=htu21d,region=outside,variable=Temperature,unit=Celsius value=%f %d", (uint16_t)(chipid >> 32), (uint32_t)chipid, htu.getTemperature(), now);
      PostToInflux(value);
      sprintf(value, "weather,board=0x%04X%08X,sensor=htu21d,region=outside,variable=Relative_Humidity,unit=Percentage value=%f %d", (uint16_t)(chipid >> 32), (uint32_t)chipid, htu.getHumidity(), now);
      PostToInflux(value);
      sprintf(value, "weather,board=0x%04X%08X,sensor=htu21d,region=outside,variable=Dew_Point,unit=Celsius value=%f %d", (uint16_t)(chipid >> 32), (uint32_t)chipid, htu.getDewPoint(), now);
      PostToInflux(value);
      sprintf(value, "weather,board=0x%04X%08X,sensor=htu21d,region=outside,variable=Heat_Index,unit=Celsius value=%f %d", (uint16_t)(chipid >> 32), (uint32_t)chipid, htu.getHeatIndex(), now);
      PostToInflux(value);
    }

    sprintf(value, "battery,board=0x%04X%08X voltage=%f,bootcount=%di %d", (uint16_t)(chipid >> 32), (uint32_t)chipid, vBat, bootCount, now);

    PostToInflux(value);
    DisconnectFromWiFi(10);
    Serial.printf("Start: %u, finish: %u, sleep: %u ", startTime, millis(), (samplingFrequency * 1000 - (millis() - startTime)));

    esp_deep_sleep(1000LL * (samplingFrequency * 1000 - (millis() - startTime)));
  }
  else
  {
    esp_deep_sleep(1000000LL * 3600);
  }
}

void loop()
{
}
