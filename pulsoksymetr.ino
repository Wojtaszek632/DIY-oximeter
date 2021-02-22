/* Projekt zaliczeniowy części zadaniowej egzamniu Systemy Mikroprocesorowe 2020/21
 *  Atmega328p
 *  MAX30100 
 *  OLED SH1106 
 *  Wojciech Mańczak 141083
*/

#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include "MAX30100.h"
#include <U8g2lib.h>

#define REPORTING_PERIOD_MS 1000
#define PULSE_WIDTH MAX30100_SPC_PW_1600US_16BITS
#define IR_LED_CURRENT MAX30100_LED_CURR_40MA
#define LED_CURRENT MAX30100_LED_CURR_20_8MA
#define SAMPLING_RATE MAX30100_SAMPRATE_100HZ

U8G2_SH1106_128X32_VISIONOX_F_HW_I2C u8g2(U8G2_R0);
PulseOximeter pox;
MAX30100 sensor;

uint32_t tsLastReport = 0;

bool OledReady = false;
int HeartRate;
int SpO2;

void onBeatDetected()
{
  u8g2.setFont(u8g2_font_cursor_tf);
  u8g2.setCursor(119, 25);
  u8g2.print("^");
  u8g2.sendBuffer();
}

void init_oled()
{
  if (!OledReady)
  {
    u8g2.clearBuffer();
    u8g2.setCursor(28, 13);
    u8g2.setFont(u8g2_font_crox2hb_tr);
    u8g2.print("Laczenie z");
    u8g2.setFont(u8g2_font_crox2h_tr);
    u8g2.setCursor(31, 30);
    u8g2.print("czujnikiem...");
    u8g2.sendBuffer();
    delay(1000);

    OledReady = true;

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_crox2hb_tr);
    if (!pox.begin())
    {
      u8g2.setCursor(1, 13);
      u8g2.print("Niepowodzenie");
      u8g2.setCursor(1, 30);
      u8g2.print("Sprawdz czujnik!");
      u8g2.sendBuffer();
      for (;;)
        ;
    }
    else
    {
      u8g2.setCursor(1, 13);
      u8g2.print("Polaczona");
      u8g2.setCursor(1, 30);
      u8g2.print("Przyloz palec...");

      u8g2.setFont(u8g2_font_cursor_tf);
      u8g2.setCursor(100, 9);
      u8g2.print("➑");

      u8g2.sendBuffer();
    }
    delay(2000);
  }
}

void setup()
{
  u8g2.begin();
  init_oled();

  pox.begin();
  pox.setOnBeatDetectedCallback(onBeatDetected);
  pox.setIRLedCurrent(LED_CURRENT);
  sensor.setMode(MAX30100_MODE_SPO2_HR);
  sensor.setLedsPulseWidth(PULSE_WIDTH);
  sensor.setSamplingRate(SAMPLING_RATE);
}

void loop()
{
  pox.update();
  HeartRate = pox.getHeartRate();
  SpO2 = pox.getSpO2();

  if (millis() - tsLastReport > REPORTING_PERIOD_MS)
  {
    if (HeartRate > 30 & HeartRate<220 & SpO2> 30 & SpO2 < 100)
    {
      u8g2.clearBuffer();

      u8g2.setFont(u8g2_font_crox2h_tr);
      u8g2.setCursor(0, 12);
      u8g2.print("PR");

      u8g2.setCursor(76, 13);
      u8g2.print("Bpm");

      u8g2.setCursor(0, 31);
      u8g2.print("SpO2 ");

      u8g2.setCursor(76, 31);
      u8g2.print("%");

      u8g2.setFont(u8g2_font_fub11_tf);
      u8g2.setCursor(46, 13);
      u8g2.print(HeartRate);

      u8g2.setCursor(46, 31);
      u8g2.print(SpO2);

      if (SpO2 <= 95)
      {
        u8g2.setCursor(90, 31);
        u8g2.print("!");
      }
      u8g2.setFont(u8g2_font_cursor_tf);

      u8g2.setCursor(119, 25);
      u8g2.print("_");

      u8g2.sendBuffer();
    }
    tsLastReport = millis();
  }
}
