#include <LedControl.h>

// Documentation:
// http://wayoda.github.io/LedControl/pages/software

int dataIn = 12;
int loadCS = 11;
int CLK = 10;

int row = 0;
int col = 0;

LedControl disp = LedControl(dataIn, CLK, loadCS, 1);

void setup() {
  // put your setup code here, to run once:

  // wake up from power-saving mode
  disp.shutdown(0, false);

  // set brightness
  disp.setIntensity(0,0);

  // clear display
  disp.clearDisplay(0);
}

void loop() {
  // put your main code here, to run repeatedly:

  // light single Led
  row = 3;
  col = 3;
  disp.setLed(0, row, col, true);

  // light row
  row = 1;
  disp.setRow(0, row, 0b11111111);

  // light column
  col = 2;
  disp.setColumn(0, col, 0b11111111); 

  delay(1000);
}
