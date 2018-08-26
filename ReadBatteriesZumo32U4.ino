// by Giorgio De Nunzio
// giorgio.denunzio@unisalento.it

// Licence: 
// "Do with this code what you wish. If you find it useful, please write to me and tell me about it. If you make any improvement, please let me know so we grow together!"

#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4LCD lcd;


void setup() {
}

void loop() {
  uint16_t VBAT = readBatteryMillivolts();
  float VBATVolts = VBAT/1000.;  // What happens if I use 1000 instead of 1000. as the fraction denominator? :-) Why?
  lcd.clear();
  lcd.print(VBATVolts);
  delay(500);
}
