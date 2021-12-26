# include <LedControl.h>
# define MAX_BODY_LENGTH 10

// Documentation:
// http://wayoda.github.io/LedControl/pages/software

int dataIn = 12;
int loadCS = 11;
int CLK = 10;

LedControl disp = LedControl(dataIn, CLK, loadCS, 1);

int row = 0;
int col = 0;

typedef struct {
  int x;
  int y;
} Coord;

// default snake body for testing
Coord body[MAX_BODY_LENGTH] = {};
int bodyLength = 5;

Coord coordToCheck = {};

enum directions {UP, DOWN, LEFT, RIGHT};
enum directions selectedDirection = UP;

void propogate(Coord coord){
  // Propogates the snake body to a coordinate.
  // INPUT: coord = The coordinate to propogate to.
  
  // remove tail from display
  disp.setLed(0, body[bodyLength - 1].x, body[bodyLength - 1].y, false);
  
  // shift body array to right
  for (int i = bodyLength - 1; i > 0; i--){
    body[i] = body[i - 1];
  }
  
  // add head to body array
  body[0] = coordToCheck;
  
  // display head
  disp.setLed(0, body[0].x, body[0].y, true);
}

void setup() {
  Serial.begin(9600);
  
  // wake up from power-saving mode
  disp.shutdown(0, false);

  // set brightness
  disp.setIntensity(0,0);

  // clear display
  disp.clearDisplay(0);

  // generate test snake body
  for (int i = 0; i < bodyLength; i++){
    body[i].x = 3;
    body[i].y = i + 3;
    disp.setLed(0, body[i].x, body[i].y, true);
  }
  
}

void loop() {
  // get direction from user
  if (Serial.available() != 0){
    String userInput = Serial.readString();
    Serial.println(userInput);
    if (userInput == "U"){
      selectedDirection = UP;
    } else if (userInput == "D"){
      selectedDirection = DOWN;
    } else if (userInput == "L"){
      selectedDirection = LEFT;
    } else if (userInput == "R"){
      selectedDirection = RIGHT;
    }
  }

  // get coord to check
  switch (selectedDirection){
    case UP:
      coordToCheck.x = body[0].x - 1;
      coordToCheck.y = body[0].y;
      break;
    case DOWN:
      coordToCheck.x = body[0].x + 1;
      coordToCheck.y = body[0].y;
      break;
    case LEFT:
      coordToCheck.x = body[0].x;
      coordToCheck.y = body[0].y - 1;
      break;
    case RIGHT:
      coordToCheck.x = body[0].x;
      coordToCheck.y = body[0].y + 1;
      break;
    default:
      break;
  }

  // adjusts coordToCheck so it doesnt exceed the matrix boundries
  coordToCheck.x = (coordToCheck.x % 8 + 8) % 8;
  coordToCheck.y = (coordToCheck.y % 8 + 8) % 8;

  propogate(coordToCheck);

  // check coord
  // if into neck
  // do nothing

  // if into body
  // game over

  // if into nothing
  // propogate

  // buffer
  delay(1000);
}
