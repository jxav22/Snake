# include <LedControl.h>
# include "snakeEnums.h"
# define MAX_BODY_LENGTH 20
# define INITIAL_BODY_LENGTH 3
# define REFRESH_RATE 200

// Documentation:
// http://wayoda.github.io/LedControl/pages/software

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
  for (int i = 0; i < score; i++){
    disp.setLed(0, i / 8, i % 8, true);
    delay(500);
  }

  delay(2000);
}

enum Directions selectedDirection = UP;
enum Directions movementDirection = UP;

bool isCoordEqual(Coord coord1, Coord coord2){
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
  
  // display head
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
  //    coord = The coordinate to check.
  //  OUTPUT:
  //    An enum representing the object at the specified coordinate.

  // if coord is food
  if (isCoordEqual(coord, foodLocation)){
    Serial.println("FOUND FOOD");
    return FOOD;
  }
  
  // if coord is a body part
  for (int i = 0; i < bodyLength; i++){
    if (isCoordEqual(coord, body[i])){
       return BODY;
    }
  }

  // if coord is nothing
  return SPACE;
}

Coord foodLocation;

void spawnFood(Coord *foodLocation, Coord body[], int bodyLength){
  // Spawns a single unit of food at a random location.
  // INPUT:
  //  foodLocation = Stores the coordinate of the food.
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
  *bodyLength = INITIAL_BODY_LENGTH; 
  
  body[0].x = 5;
  body[0].y = 5;

  disp.setLed(0, body[0].x, body[0].y, true);
//  for (int i = 0; i < bodyLength; i++){
//    body[i].x = 3;
//    body[i].y = i + 2;
//    disp.setLed(0, body[i].x, body[i].y, true);
//  }
}

void despawnBody(Coord body[], int bodyLength){
  for (int i = 0; i < bodyLength; i++){
    disp.setLed(0, body[i].x, body[i].y, false);
  }
}

void blinkBody(Coord body[], int bodyLength){
  int i;
  
  for (i = 0; i < bodyLength; i++){
    disp.setLed(0, body[i].x, body[i].y, false);
  }

  delay(REFRESH_RATE * 2);

  for (i = 0; i < bodyLength; i++){
    disp.setLed(0, body[i].x, body[i].y, true);
  }

  delay(REFRESH_RATE * 2);
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

  // spawn test food
  spawnFood(&foodLocation, body, bodyLength);

  
}

void loop() {
  // get direction from user
//  if (Serial.available() != 0){
//    String userInput = Serial.readString();
//    Serial.println(userInput);
//    if (userInput == "U"){
//      selectedDirection = UP;
//    } else if (userInput == "D"){
//      selectedDirection = DOWN;
//    } else if (userInput == "L"){
//      selectedDirection = LEFT;
//    } else if (userInput == "R"){
//      selectedDirection = RIGHT;
//    }
//  }

  X = analogRead(XPin);
  Y = analogRead(YPin);

//  SW = digitalRead(SWPin);
//
//  Serial.print(X);
//  Serial.print(",");
//  Serial.println(Y);
//  Serial.print(",");
//  Serial.println(SW * 1023);

  if (Y > 550){
    selectedDirection = DOWN;
  } else if (Y < 150){
    selectedDirection = UP;
  } else if (X < 500){
    selectedDirection = LEFT;
  } else if (X > 700){
    selectedDirection = RIGHT;
  } 

  // process direction
  if ( (selectedDirection != movementDirection) && (selectedDirection != oppositeDirection(movementDirection)) ){
    movementDirection = selectedDirection;
  }

  // check coordinate at direction
  Coord coordToCheck = getCoordFromDirection(body, movementDirection);
  enum boardObjects objectAtCoord = checkCoord(body, bodyLength, foodLocation, coordToCheck);

  // process object at coordinate
  if (objectAtCoord == SPACE){
    
    // move snake
    propogate(body, bodyLength, coordToCheck);
  } else if (objectAtCoord == BODY){
    // game over
        
    blinkBody(body, bodyLength);
    blinkBody(body, bodyLength);
    blinkBody(body, bodyLength);
    
    despawnBody(body, bodyLength);

    displayScore(score);
    
    spawnBody(body, &bodyLength);
    
    delay(5000);
    
  } else if (objectAtCoord == FOOD){
        
    if (bodyLength < MAX_BODY_LENGTH){
      bodyLength++;
    }
    
    // move snake
    propogate(body, bodyLength, coordToCheck);

    // respawn food
    spawnFood(&foodLocation, body, bodyLength);

    // add to score
    score++;
  }
  
  // buffer
  delay(REFRESH_RATE);
}
