# include <LedControl.h>

int dataIn = 12;
int loadCS = 11;
int CLK = 10;

// Documentation:
// http://wayoda.github.io/LedControl/pages/software

LedControl disp = LedControl(dataIn, CLK, loadCS, 1);

void setup() {
  // set brightness
  disp.setIntensity(0,0);

  // clear display
  disp.clearDisplay(0);
}

void loop() {
  // put your main code here, to run repeatedly:

}
