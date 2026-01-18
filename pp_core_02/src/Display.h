#ifndef Display_h
#define Display_h

#include <Arduino.h>
#include <Driver.h>
#include <Motors.h>
#include <Switches.h>
#include <U8g2lib.h>
#include <Wire.h>

/**
 * example code at:
 * https://wiki.dfrobot.com/Monochrome%200.91%E2%80%9D128x32%20I2C%20OLED%20Display%20SKU:DFR0648
 */
class Display {
   private:
    static U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C baseDisplay;
    static String messages[4];

   public:
    static bool powerup();
    static void addMessage(String message);
    static void drawCounters();
    static bool depower();
};

#endif