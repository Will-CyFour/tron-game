
#include <math.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Screen
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define WHITE SSD1306_WHITE
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define MAIN_MENU 0
#define IN_GAME 1
#define DRAW 2
#define P1_WIN 3
#define P2_WIN 4
//Game States
char gameState = 0;

enum gameStates {menu, in_game, draw, p1_win, p2_win};


//Directions
#define NOT_MOVING -1
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

//Player 1 button pins
#define INPUT_LEFT_1 4
#define INPUT_RIGHT_1 6
#define INPUT_UP_1 5
#define INPUT_DOWN_1 7

//Player class
class Player
{
  public:
    int x;
    int y;
    int startingX;
    int startingY;
    char direction; //-1, is not moving, 0 is up, 1 is down, 2 is left, 3 is right

  Player(int startingX, int startingY, char direction){
    this->x = startingX;
    this->y = startingY;
    this->startingX = startingX;
    this->startingY = startingY;
    this->direction = direction;
  }


  void setDirection(char newDirection){
    direction = newDirection;

  }

  void movePlayer(){
    switch (direction){
      case UP: this->y--; break;
      case DOWN: this->y++; break;
      case LEFT: this->x--; break;
      case RIGHT: this->x++; break;
    }
  }

	bool samePointAs(Player other){
		return (x == other.x) && (y == other.y);
	}

};

Player p1(SCREEN_HEIGHT / 2, SCREEN_WIDTH * 1 / 4, RIGHT);
Player p2(SCREEN_HEIGHT / 2, SCREEN_WIDTH * 3 / 4, NOT_MOVING);

void setup() {
  Serial.begin(9600);
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();

  pinMode(INPUT_LEFT_1, INPUT_PULLUP);
  pinMode(INPUT_RIGHT_1, INPUT_PULLUP);
  pinMode(INPUT_UP_1, INPUT_PULLUP);
  pinMode(INPUT_DOWN_1, INPUT_PULLUP);
  gameState = IN_GAME;

}

void loop(){
  delay(50);
  display.drawRect(-1,-1, SCREEN_WIDTH+2, SCREEN_HEIGHT+2, WHITE);
	switch (gameState)
	{
		case MAIN_MENU: break;
		case IN_GAME: inGame(); break;
		case P1_WIN: break;
		case P2_WIN: break;
		case DRAW: break;
	}
	display.display();
}

void inGame()
{
	//Check inputs, modify directions accordingly
 turns(p1, INPUT_UP_1, INPUT_DOWN_1, INPUT_LEFT_1, INPUT_RIGHT_1);


	//Move the players
	p1.movePlayer();
	p2.movePlayer();

	//Check for crashes, change game state accordingly
	if ((hasCrashed(p1) && hasCrashed(p2)) || p1.samePointAs(p2))
		gameState = DRAW;
	else if (hasCrashed(p1))
		gameState = P2_WIN;
	else if (hasCrashed(p2))
		gameState = P1_WIN;
	else { //If there are no crashes, then draw the new positions of the players
		display.drawPixel(p1.x, p1.y, WHITE);
		display.drawPixel(p2.x, p2.y, WHITE);
	}
}

char turns(Player p, char up_pin, char down_pin, char left_pin, char right_pin){
	if (p.direction == UP || p.direction == DOWN){
		if (isPressed(left_pin) && !isPressed(right_pin))
			return LEFT;
		else if (isPressed(right_pin) && !isPressed(left_pin))
			return RIGHT;
	} else if (p.direction == LEFT || p.direction == RIGHT){

		if (isPressed(up_pin) && !isPressed(down_pin))
      p.direction = UP;
			//return UP;
		else if (isPressed(down_pin) && !isPressed(up_pin))
			return DOWN;
	}
}

bool hasCrashed(Player p){
	return gameState == IN_GAME && display.getPixel(p.x, p.y);
}

bool isPressed(int buttonPin) {
  return digitalRead(buttonPin) == LOW;
}

//void drawPixel(x, y, WHITE);
//bool getPixel(x,y);
