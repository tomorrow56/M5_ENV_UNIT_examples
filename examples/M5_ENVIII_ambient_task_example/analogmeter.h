#ifndef analogmetere_h
#define analogmeter_h

#include <M5Stack.h>

// Define meter size as 1 for M5.Lcd.rotation(0) or 1.3333 for M5.Lcd.rotation(1)
#define M_SIZE 1

class ANALOGMETER{
public:
  String MeterLabel[5] = {"0", "25", "50", "75", "100"};
  int ZoneLimit[5] = {10, 25, 30, 35, 50};   // -50 to 50
  int ZoneColor[5] = {BLUE, GREEN, YELLOW, ORANGE, RED};
  int xOffset;
  String Title = "default";
  float ltx = 0;    // Saved x coord of bottom of needle
  uint16_t osx = M_SIZE*120, osy = M_SIZE*120; // Saved x & y coords

  int old_analog =  -999; // Value last displayed

  //  Draw the analogue meter on the screen
  void drawMeter();
  // Update needle position
  void plotNeedle(int value, byte ms_delay);

private:
};

#endif
