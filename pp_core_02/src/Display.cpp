#include <Display.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C Display::baseDisplay(U8G2_R0, U8X8_PIN_NONE, GPIO_NUM_36, GPIO_NUM_35);  // rotation
String Display::messages[4] = {"", "", "", ""};

bool Display::powerup() {

    // Wire.begin(GPIO_NUM_35, GPIO_NUM_36);

    bool success = Display::baseDisplay.begin();
    Display::baseDisplay.setFont(u8g2_font_5x8_mf);

    Display::baseDisplay.firstPage();
    do {
        Display::baseDisplay.setCursor(10, 7);
        Display::baseDisplay.print("PEN-PLOTTER-02");
    } while (Display::baseDisplay.nextPage());

    return success;
}

bool Display::depower() {
    Display::baseDisplay.sleepOn();
    return true;
}

void Display::addMessage(String message) {
    messages[0] = messages[1];
    messages[1] = messages[2];
    messages[2] = messages[3];
    messages[3] = message;
    Display::baseDisplay.firstPage();
    do {
        Display::baseDisplay.clearDisplay();
        Display::baseDisplay.setCursor(2, 7);
        Display::baseDisplay.print("> ");
        Display::baseDisplay.print(messages[0]);
        Display::baseDisplay.setCursor(2, 15);
        Display::baseDisplay.print("> ");
        Display::baseDisplay.print(messages[1]);
        Display::baseDisplay.setCursor(2, 23);
        Display::baseDisplay.print("> ");
        Display::baseDisplay.print(messages[2]);
        Display::baseDisplay.setCursor(2, 31);
        Display::baseDisplay.print("> ");
        Display::baseDisplay.print(messages[3]);
    } while (Display::baseDisplay.nextPage());
}

void Display::drawCounters() {
    Display::baseDisplay.firstPage();
    do {

        Display::baseDisplay.clearDisplay();
        Display::baseDisplay.setCursor(2, 7);
        Display::baseDisplay.print("COUNTERS");

        Display::baseDisplay.setCursor(64, 7);
        Display::baseDisplay.print("A: ");
        Display::baseDisplay.print(Motors::motorA.getCntrCur());
        Display::baseDisplay.setCursor(64, 15);
        Display::baseDisplay.print("B: ");
        Display::baseDisplay.print(Motors::motorB.getCntrCur());
        Display::baseDisplay.setCursor(64, 23);
        Display::baseDisplay.print("Z: ");
        Display::baseDisplay.print(Motors::motorZ.getCntrCur());
        Display::baseDisplay.setCursor(64, 31);
        Display::baseDisplay.print("R: ");
        Display::baseDisplay.print(Motors::motorR.getCntrCur());

        Display::baseDisplay.setCursor(2, 31);
        Display::baseDisplay.print(Driver::frq___mHz);

    } while (Display::baseDisplay.nextPage());
}
