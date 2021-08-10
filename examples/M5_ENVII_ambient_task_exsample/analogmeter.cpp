#include "analogmeter.h"
#include <M5Stack.h>

// #########################################################################
//  Draw the analogue meter on the screen
// #########################################################################
void ANALOGMETER::drawMeter(){

  xOffset = (319 - M_SIZE*239) / 2;
  // Meter outline
  M5.Lcd.fillRect(xOffset, 0, M_SIZE*239, M_SIZE*126, DARKGREY);
  M5.Lcd.fillRect(xOffset + 5, 3, M_SIZE*230, M_SIZE*119, WHITE);

  M5.Lcd.setTextColor(BLACK);  // Text colour

  // Draw ticks every 5 degrees from -50 to +50 degrees (100 deg. FSD swing)
  for (int i = -50; i < 51; i += 5) {
    // Long scale tick length
    int tl = 15;

    // Coordinates of tick to draw
    float sx = cos((i - 90) * 0.0174532925);
    float sy = sin((i - 90) * 0.0174532925);
    uint16_t x0 = sx * (M_SIZE*100 + tl) + M_SIZE*120;
    uint16_t y0 = sy * (M_SIZE*100 + tl) + M_SIZE*140;
    uint16_t x1 = sx * M_SIZE*100 + M_SIZE*120;
    uint16_t y1 = sy * M_SIZE*100 + M_SIZE*140;

    // Coordinates of next tick for zone fill
    float sx2 = cos((i + 5 - 90) * 0.0174532925);
    float sy2 = sin((i + 5 - 90) * 0.0174532925);
    int x2 = sx2 * (M_SIZE*100 + tl) + M_SIZE*120;
    int y2 = sy2 * (M_SIZE*100 + tl) + M_SIZE*140;
    int x3 = sx2 * M_SIZE*100 + M_SIZE*120;
    int y3 = sy2 * M_SIZE*100 + M_SIZE*140;

    //zone limit 1
    if (i >= -50 && i < ZoneLimit[0]) {
      M5.Lcd.fillTriangle(xOffset + x0, y0, xOffset + x1, y1, xOffset + x2, y2, ZoneColor[0]);
      M5.Lcd.fillTriangle(xOffset + x1, y1, xOffset + x2, y2, xOffset + x3, y3, ZoneColor[0]);
    }

    //zone limit 2
    if (i >= ZoneLimit[0] && i < ZoneLimit[1]) {
      M5.Lcd.fillTriangle(xOffset + x0, y0, xOffset + x1, y1, xOffset + x2, y2, ZoneColor[1]);
      M5.Lcd.fillTriangle(xOffset + x1, y1, xOffset + x2, y2, xOffset + x3, y3, ZoneColor[1]);
    }

    //zone limit 3
    if (i >= ZoneLimit[1] && i < ZoneLimit[2]) {
      M5.Lcd.fillTriangle(xOffset + x0, y0, xOffset + x1, y1, xOffset + x2, y2, ZoneColor[2]);
      M5.Lcd.fillTriangle(xOffset + x1, y1, xOffset + x2, y2, xOffset + x3, y3, ZoneColor[2]);
    }

    //zone limit 4
    if (i >= ZoneLimit[2] && i < ZoneLimit[3]) {
      M5.Lcd.fillTriangle(xOffset + x0, y0, xOffset + x1, y1, xOffset + x2, y2, ZoneColor[3]);
      M5.Lcd.fillTriangle(xOffset + x1, y1, xOffset + x2, y2, xOffset + x3, y3, ZoneColor[3]);
    }

    //zone limit 5
    if (i >= ZoneLimit[3] && i < ZoneLimit[4]) {
      M5.Lcd.fillTriangle(xOffset + x0, y0, xOffset + x1, y1, xOffset + x2, y2, ZoneColor[4]);
      M5.Lcd.fillTriangle(xOffset + x1, y1, xOffset + x2, y2, xOffset + x3, y3, ZoneColor[4]);
    }

    // Short scale tick length
    if(i % 25 != 0){
      tl = 8;
    }

    // Recalculate cords incase tick length changed
    x0 = sx * (M_SIZE*100 + tl) + M_SIZE*120;
    y0 = sy * (M_SIZE*100 + tl) + M_SIZE*140;
    x1 = sx * M_SIZE*100 + M_SIZE*120;
    y1 = sy * M_SIZE*100 + M_SIZE*140;

    // Draw tick
    M5.Lcd.drawLine(xOffset + x0, y0, xOffset + x1, y1, BLACK);

    // Check if labels should be drawn, with position tweaks
    if(i % 25 == 0){
      // Calculate label positions
      x0 = sx * (M_SIZE*100 + tl + 10) + M_SIZE*120;
      y0 = sy * (M_SIZE*100 + tl + 10) + M_SIZE*140;
      switch (i / 25){
        case -2: M5.Lcd.drawCentreString(MeterLabel[0], xOffset + x0, y0 - 12, 1); break;
        case -1: M5.Lcd.drawCentreString(MeterLabel[1], xOffset + x0, y0 - 9, 1); break;
        case 0: M5.Lcd.drawCentreString(MeterLabel[2], xOffset + x0, y0 - 7, 1); break;
        case 1: M5.Lcd.drawCentreString(MeterLabel[3], xOffset + x0, y0 - 9, 1); break;
        case 2: M5.Lcd.drawCentreString(MeterLabel[4], xOffset + x0, y0 - 12, 1); break;
      }
    }

    // Now draw the arc of the scale
    sx = cos((i + 5 - 90) * 0.0174532925);
    sy = sin((i + 5 - 90) * 0.0174532925);
    x0 = sx * M_SIZE*100 + M_SIZE*120;
    y0 = sy * M_SIZE*100 + M_SIZE*140;
    // Draw scale arc, don't draw the last part
    if (i < 50) M5.Lcd.drawLine(xOffset + x0, y0, xOffset + x1, y1, BLACK);
  }

  M5.Lcd.drawCentreString(Title, xOffset + M_SIZE*120, M_SIZE*80, 2);
  M5.Lcd.drawRect(xOffset + 5, 3, M_SIZE*230, M_SIZE*119, BLACK); // Draw bezel line

//  plotNeedle(0, 0); // Put meter needle at 0
}

// #########################################################################
// Update needle position
// This function is blocking while needle moves, time depends on ms_delay
// 10ms minimises needle flicker if text is drawn within needle sweep area
// Smaller values OK if text not in sweep area, zero for instant movement but
// does not look realistic... (note: 100 increments for full scale deflection)
// #########################################################################
void ANALOGMETER::plotNeedle(int value, byte ms_delay){

//  M5.Lcd.setTextColor(BLACK, WHITE);
//  char buf[8]; dtostrf(value, 4, 0, buf);
//  M5.Lcd.drawRightString(buf, M_SIZE*40, M_SIZE*(119 - 20), 2);

  // Limit value to emulate needle end stops
  if(value < -10){
    value = -10;
  }

  if(value > 110){
    value = 110;
  }

  // Move the needle until new value reached
  while(!(value == old_analog)){
    if(old_analog < value){
       old_analog++;
    }else{
       old_analog--;
    }

    // Update immediately if delay is 0
    if(ms_delay == 0){
      old_analog = value;
    }

    float sdeg = map(old_analog, -10, 110, -150, -30); // Map value to angle
    // Calcualte tip of needle coords
    float sx = cos(sdeg * 0.0174532925);
    float sy = sin(sdeg * 0.0174532925);

    // Calculate x delta of needle start (does not start at pivot point)
    float tx = tan((sdeg + 90) * 0.0174532925);

    // Erase old needle image
    M5.Lcd.drawLine(xOffset + M_SIZE*(120 + 20 * ltx - 1), M_SIZE*(140 - 20), xOffset + osx - 1, osy, WHITE);
    M5.Lcd.drawLine(xOffset + M_SIZE*(120 + 20 * ltx), M_SIZE*(140 - 20), xOffset + osx, osy, WHITE);
    M5.Lcd.drawLine(xOffset + M_SIZE*(120 + 20 * ltx + 1), M_SIZE*(140 - 20), xOffset + osx + 1, osy, WHITE);

    // Re-plot text under needle
    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.drawCentreString(Title, xOffset + M_SIZE*120, M_SIZE*80, 2);

    // Store new needle end cords for next erase
    ltx = tx;
    osx = M_SIZE*(sx * 98 + 120);
    osy = M_SIZE*(sy * 98 + 140);

    // Draw the needle in the new postion, magenta makes needle a bit bolder
    // draws 3 lines to thicken needle
    M5.Lcd.drawLine(xOffset + M_SIZE*(120 + 20 * ltx - 1), M_SIZE*(140 - 20), xOffset + osx - 1, osy, RED);
    M5.Lcd.drawLine(xOffset + M_SIZE*(120 + 20 * ltx), M_SIZE*(140 - 20), xOffset + osx, osy, MAGENTA);
    M5.Lcd.drawLine(xOffset + M_SIZE*(120 + 20 * ltx + 1), M_SIZE*(140 - 20), xOffset + osx + 1, osy, RED);

    // Slow needle down slightly as it approaches new postion
    if(abs(old_analog - value) < 10){
      ms_delay += ms_delay / 5;
    }

    // Wait before next update
    delay(ms_delay);
  }
}
