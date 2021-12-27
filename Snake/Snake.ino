# include <LedControl.h>
# include "snakeEnums.h"
# define MAX_BODY_LENGTH 30
# define INITIAL_BODY_LENGTH 3
# define REFRESH_RATE 100

// Documentation:
// http://wayoda.github.io/LedControl/pages/software

// Set up Joystick
int XPin = A0;
int X = 0;

int YPin = A1;
int Y = 0;

int SWPin = 2;
int SW = 2;

Coord centre = {.x = 512, .y = 512};

// set lower for more accuracy
int joystickAccuracy = 20;

void calibrateJoystick(){
  centre.x = analogRead(XPin);
  centre.y = analogRead(YPin);
}

// Set up LED matrix
int dataIn = 12;
int loadCS = 11;
int CLK = 10;

LedControl disp = LedControl(dataIn, CLK, loadCS, 1);

// Set up game logic

/*
 * SNAKE BODY ANATOMY REFERENCE:
 * 
 * (movement occurs here)
 * <head>
 * <neck>
 * <body>
 * <body>
 * <body>
 * ...
 * <tail>
 * 
 * The whole thing is referred to as the body sometimes.
 */

int score = 0;

void displayScore(int score){
  // Briefly displays the score to the user, through an animation.
  // INPUT:
  //  score = The score to display.
  
  for (int i = 0; i < score; i++){
    disp.setLed(0, i / 8, i % 8, true);
    delay(500);
  }

  delay(2000);

  for (int i = 0; i < score; i++){
    disp.setLed(0, i / 8, i % 8, false);
  }
}

enum Directions selectedDirection = UP;
enum Directions movementDirection = UP;

bool isCoordEqual(Coord coord1, Coord coord2){
  // Checks if two coordinates are equal.
  // INPUT:
  //  coord1 = The first coordinate to compare.
  //  coord2 = The second coordinate to compare.
  // OUTPUT:
  //  A booleans. Displays true if the coordinates are equal and false otherwise.
  return (coord1.x == coord2.x) && (coord1.y == coord2.y);
}


// snake body
Coord body[MAX_BODY_LENGTH] = {};
int bodyLength = INITIAL_BODY_LENGTH;

void propogate(Coord body[], int bodyLength, Coord coord){
  // Propogates the snake body to a coordinate.
  // INPUT: 
  //  body = The body to propogate.
  //  bodyLength = The length of the body to propogate.
  //  coord = The coordinate to propogate to.
  
  // remove tail from display
  disp.setLed(0, body[bodyLength - 1].x, body[bodyLength - 1].y, false);
  
  // shift body array to right
  for (int i = bodyLength - 1; i > 0; i--){
    body[i] = body[i - 1];
  }
  
  // add head to body array
  body[0] = coord;
  
  // add head to display
  disp.setLed(0, body[0].x, body[0].y, true);
}

Coord getCoordFromDirection(Coord body[], enum Directions selectedDirection){
  // Gets a coordinate from the head of a body in the selected direction.
  // INPUTS:
  //  body = The body to use as reference.
  //  selectedDirection = The direction to get a coordinate.
  // OUTPUT:
  //  The coordinate in the selected direction, relative to the body head.
  
  Coord coord;
  
  // get coord
  switch (selectedDirection){
    case UP:
      coord.x = body[0].x - 1;
      coord.y = body[0].y;
      break;
    case DOWN:
      coord.x = body[0].x + 1;
      coord.y = body[0].y;
      break;
    case LEFT:
      coord.x = body[0].x;
      coord.y = body[0].y - 1;
      break;
    case RIGHT:
      coord.x = body[0].x;
      coord.y = body[0].y + 1;
      break;
    default:
      break;
  }

  // adjusts coord so it doesnt exceed the matrix boundries
  coord.x = (coord.x % 8 + 8) % 8;
  coord.y = (coord.y % 8 + 8) % 8;

  return coord;
}

enum boardObjects checkCoord(Coord body[], int bodyLength, Coord foodLocation, Coord coord){
  // Determines the object at the specified coordinate.
  //  INPUT:
  //    body = The snake body.
  //    bodyLength = The length of the snake body.
  //    foodLocation = The coordinate of the food.
  //    coord = The coordinate to check.
  //  OUTPUT:
  //    An enum representing the object at the specified coordinate.

  // if coord is food
  if (isCoordEqual(coord, foodLocation)){
    return FOOD;
  }
  
  // if coord is a body
  for (int i = 1; i < bodyLength - 1; i++){
    if (isCoordEqual(coord, body[i])){
       return BODY;
    }
  }

  // if coord is a tail
  if (isCoordEqual(coord, body[bodyLength - 1])){
    return TAIL;
  }

  // if coord is nothing
  return SPACE;
}

Coord foodLocation;

void spawnFood(Coord *foodLocation, Coord body[], int bodyLength){
  // Spawns a single unit of food at a random location.
  // INPUT:
  //  foodLocation = A pointer, pointing to the address of the coordinate of the food.
  //  body = The snake body.
  //  bodylength = The length of the snake body.
  
  Coord location;

  do {
    location.x = random(0, 7);
    location.y = random(0, 7);
  } while (checkCoord(body, bodyLength, *foodLocation, location) != SPACE);
  
  // set location
  foodLocation->x = location.x;
  foodLocation->y = location.y;

  // light LED
  disp.setLed(0, foodLocation->x, foodLocation->y, true);
}

void spawnBody(Coord body[], int *bodyLength){
  // Spawns a snake at a random point on the display).
  // INPUTS:
  //  body = The snake body.
  //  bodyLength = A pointer, pointing to the address of the length of the snake.
  *bodyLength = INITIAL_BODY_LENGTH; 
  
  body[0].x = random(0,7);
  body[0].y = random(0,7);

  for (int i = 1; i < *bodyLength; i++){
    body[i] = body[0];
  }

  disp.setLed(0, body[0].x, body[0].y, true);
}

void despawnBody(Coord body[], int *bodyLength){
  // Despawns a snake, removing it from the display.
  // INPUT:
  //  body = The snake body.
  //  bodyLength = The length of the snake.
  for (int i = 0; i < *bodyLength; i++){
    disp.setLed(0, body[i].x, body[i].y, false);
  }

  *bodyLength = 0;
}

void blinkBody(Coord body[], int bodyLength, int times){
  // Blinks a snake body.
  // INPUT:
  //  body = The snake body.
  //  bodyLength = The length of the snake.
  //  times = The amount of times to blink the snake body.
  int i;
  int j;

  for (i = 0; i < times; i++){
  
    for (j = 0; j < bodyLength; j++){
      disp.setLed(0, body[j].x, body[j].y, false);
    }
  
    delay(REFRESH_RATE * 2);
  
    for (j = 0; j < bodyLength; j++){
      disp.setLed(0, body[j].x, body[j].y, true);
    }
  
    delay(REFRESH_RATE * 2);
  }
}

void waitForJoyStickPress(){
  // Debugging tool
  blinkBody(body, bodyLength, 1);
  while(digitalRead(SWPin) == HIGH){
    delay(100);
  }
}

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

  // generate snake body
  spawnBody(body, &bodyLength);

  // spawn food
  spawnFood(&foodLocation, body, bodyLength);

  // calibrate joystick
  calibrateJoystick();
 
}

void loop() {

/* 
// USER INPUT:  SERIAL METHOD
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
*/

/*
// USER INPUT:  JOYSTICK METHOD (OLD)
  if (Y > 550){
    selectedDirection = DOWN;
  } else if (Y < 450){
    selectedDirection = UP;
  } else if (X < 450){
    selectedDirection = LEFT;
  } else if (X > 550){
    selectedDirection = RIGHT;
  } 
*/

/*
// CALIBRATE JOYSTICK
//  SW = digitalRead(SWPin);

  Serial.print(X);
  Serial.print(",");
  Serial.println(Y);
  
//  Serial.print(",");
//  Serial.println(SW * 1023);
*/

/*
  // USER INPUT:  JOYSTICK METHOD (NEW)

  if ( (Y > abs(X - centre.x) + centre.x) && (Y > centre.y + joystickAccuracy) ){
    selectedDirection = DOWN;
  } else if ( (Y < -abs(X - centre.x) + centre.x ) && (Y < centre.y - joystickAccuracy) ){
    selectedDirection = UP;
  } else if (X < centre.x - joystickAccuracy) {
    selectedDirection = LEFT;
  } else if (X > centre.x + joystickAccuracy) {
    selectedDirection = RIGHT;
  }
*/

/*
  for (int i = 0; i < REFRESH_RATE; i+= 10){
    X = analogRead(XPin);
    Y = analogRead(YPin);
    
    // USER INPUT: JOYSTICK METHOD (DYNAMIC)
  
    if ( (Y > abs(X - centre.x) + centre.x) && (Y > centre.y + 50) ){
      selectedDirection = DOWN;
    } else if ( (Y < -abs(X - centre.x) + centre.x) && (Y < centre.y - 50) ){
      selectedDirection = UP;
    } else if (X < 450){
      selectedDirection = LEFT;
    } else if (X > 550){
      selectedDirection = RIGHT;
    }
    
    delay(10);
  }

  centre.x = X;
  centre.y = Y;
*/

  // Get user input

  for (int i = 0; i < REFRESH_RATE; i+= 10){
    X = analogRead(XPin);
    Y = analogRead(YPin);
    
    // USER INPUT:  JOYSTICK METHOD (NEW)
  
    if ( (Y > abs(X - centre.x) + centre.x) && (Y > centre.y + joystickAccuracy) ){
      selectedDirection = DOWN;
    } else if ( (Y < -abs(X - centre.x) + centre.x ) && (Y < centre.y - joystickAccuracy) ){
      selectedDirection = UP;
    } else if (X < centre.x - joystickAccuracy) {
      selectedDirection = LEFT;
    } else if (X > centre.x + joystickAccuracy) {
      selectedDirection = RIGHT;
    }
    
    delay(10);
  }

  // process direction
  if ( (selectedDirection != movementDirection) && (selectedDirection != oppositeDirection(movementDirection)) ){
    movementDirection = selectedDirection;
  }

  // check coordinate at direction
  Coord coordToCheck = getCoordFromDirection(body, movementDirection);
  enum boardObjects objectAtCoord = checkCoord(body, bodyLength, foodLocation, coordToCheck);

  // process object at coordinate
  if (objectAtCoord == SPACE || objectAtCoord == TAIL){
    // move snake
    
    propogate(body, bodyLength, coordToCheck);
    
  } else if (objectAtCoord == FOOD){

    Coord tailCoord = body[bodyLength - 1]; 
         
    // move snake
    propogate(body, bodyLength, coordToCheck);

    // increase snake
    if (bodyLength < MAX_BODY_LENGTH){
      bodyLength++;
      body[bodyLength - 1] = tailCoord;
    }
    
    // respawn food
    spawnFood(&foodLocation, body, bodyLength);

    // add to score
    score++;
    
  } else if (objectAtCoord == BODY){
    // game over
        
    blinkBody(body, bodyLength, 3);
    
    despawnBody(body, &bodyLength);

    //despawn food
    disp.setLed(0, foodLocation.x, foodLocation.y, false);

    displayScore(score);

    // respawn
    score = 0;
    spawnBody(body, &bodyLength);
    spawnFood(&foodLocation, body, bodyLength);
    
    delay(3000);
  } 
  
}
