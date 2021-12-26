# include <LedControl.h>
# define REFRESH_RATE 10

// Set up Joystick
int XPin = A0;
int X = 0;

int YPin = A1;
int Y = 0;

int SWPin = 2;
int SW = 2;

// Set up LED matrix
int dataIn = 12;
int loadCS = 11;
int CLK = 10;

LedControl disp = LedControl(dataIn, CLK, loadCS, 1);

void setup() {
  Serial.begin(9600);

  // set up joystick button
  pinMode(SWPin, INPUT_PULLUP);
  
  // wake up from power-saving mode
  disp.shutdown(0, false);

  // set brightness
  disp.setIntensity(0,0);

  // clear display
  disp.clearDisplay(0);

}

void loop() {
  // put your main code here, to run repeatedly:
  X = analogRead(XPin);
  Y = analogRead(YPin);

  int dispY = (int)((X / 1023.0) * 8.0);
  int dispX = (int)((Y / 1023.0) * 8.0);

  disp.setLed(0, dispX, dispY, true);

  delay(REFRESH_RATE);
  disp.setLed(0, dispX, dispY, false);
}
