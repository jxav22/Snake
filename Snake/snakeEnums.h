typedef struct {
  int x;
  int y;
} Coord;

enum Directions {UP, DOWN, LEFT, RIGHT};
enum boardObjects {BODY, SPACE, FOOD, TAIL};

Directions oppositeDirection(enum Directions inputDirection){
    switch (inputDirection){
    case UP:
      return DOWN;
      break;
    case DOWN:
      return UP;
      break;
    case LEFT:
      return RIGHT;
      break;
    case RIGHT:
      return LEFT;
      break;
    default:
      break;
  }
}
