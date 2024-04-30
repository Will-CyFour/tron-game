
#include <math.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Creative
#define SOUND 12
#define HAP 11
#define RED 1
#define BLUE 0

//Screen
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define WHITE SSD1306_WHITE
#define BLACK SSD1306_BLACK
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


//Directions
#define NOT_MOVING -1
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

//Player 1 button pins
#define INPUT_LEFT_1 4
#define INPUT_RIGHT_1 6
#define INPUT_UP_1 5
#define INPUT_DOWN_1 7

//Player 1 button pins
#define INPUT_LEFT_2 10
#define INPUT_RIGHT_2 13
#define INPUT_UP_2 9
#define INPUT_DOWN_2 8

bool isPressed(int buttonPin) {
  return digitalRead(buttonPin) == LOW;
}

bool isAnyPressed() {
  return 
  isPressed(INPUT_LEFT_1) || isPressed(INPUT_LEFT_2) || 
  isPressed(INPUT_RIGHT_1) || isPressed(INPUT_RIGHT_2) ||
  isPressed(INPUT_UP_1) || isPressed(INPUT_UP_2) ||
  isPressed(INPUT_DOWN_1) || isPressed(INPUT_DOWN_2);
}

//Player class
class Player
{
  public:
    int x;
    int y;
    int startingX;
    int startingY;

    char up_pin;
    char down_pin;
    char left_pin;
    char right_pin;
    int initialDirection;
    int direction; //-1, is not moving, 1 is up, 2 is down, 3 is left, 4 is right
    int newDirection;

  Player(int startingX, int startingY, int direction, char up, char down, char left, char right){
    x = startingX;
    y = startingY;
    up_pin = up;
    down_pin = down;
    left_pin = left;
    right_pin = right;
    this->startingX = startingX;
    this->startingY = startingY;
    this->direction = direction;
    initialDirection = direction;
    newDirection = direction;
  }

  void setNewDirection(){
	if (direction = UP || direction == DOWN){
		if (isPressed(left_pin) && !isPressed(right_pin))
			newDirection = LEFT;
		else if (isPressed(right_pin) && !isPressed(left_pin))
			newDirection = RIGHT;
	} else if (direction == LEFT || direction == RIGHT){
		if (isPressed(up_pin) && !isPressed(down_pin))
			newDirection = UP;
		else if (isPressed(down_pin) && !isPressed(up_pin))
			newDirection =  DOWN;
	}
}

  void movePlayer(){
    direction = newDirection;
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

  bool hasCrashed(){
    return display.getPixel(x, y);
  }

  void resetPosition(){
    x = startingX;
    y = startingY;
    direction = initialDirection;
    newDirection = initialDirection;
  }

};

Player p1(SCREEN_WIDTH * 1 / 4, SCREEN_HEIGHT / 2, RIGHT, INPUT_UP_1, INPUT_DOWN_1, INPUT_LEFT_1, INPUT_RIGHT_1);
Player p2(SCREEN_WIDTH * 3 / 4, SCREEN_HEIGHT / 2, LEFT, INPUT_UP_2, INPUT_DOWN_2, INPUT_LEFT_2, INPUT_RIGHT_2);

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
  pinMode(INPUT_LEFT_2, INPUT_PULLUP);
  pinMode(INPUT_RIGHT_2, INPUT_PULLUP);
  pinMode(INPUT_UP_2, INPUT_PULLUP);
  pinMode(INPUT_DOWN_2, INPUT_PULLUP);

  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);

  changeState(MAIN_MENU);
  
}

void loop(){
  delay(30);


  if (gameState == IN_GAME){
    inGame();
  } else {
    main_menu();
  }
	switch (gameState)
	{
		case P1_WIN: blinking(RED); break;
		case P2_WIN: blinking(BLUE); break;
	}
	display.display();
}

void inGameSetup(){
  display.clearDisplay();
  display.drawRect(0,0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE); //background box
  p1.resetPosition();
  p2.resetPosition();
  display.display();
  tone(SOUND, 440, 500); delay(500);
  tone(SOUND, 494, 500); delay(500);
  tone(SOUND, 590, 300); delay(500);
}

void inGame()
{
  //display.drawRect(0,0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE); //background box
	//Check inputs, modify directions accordingly
  int newDirection = p1.direction;
  int up_pin = INPUT_UP_1; int down_pin = INPUT_DOWN_1; int left_pin = INPUT_LEFT_1; int right_pin = INPUT_RIGHT_1;
	if (p1.direction == UP || p1.direction == DOWN){
		if (isPressed(left_pin) && !isPressed(right_pin))
			newDirection = LEFT;
		else if (isPressed(right_pin) && !isPressed(left_pin))
			newDirection = RIGHT;
	} else if (p1.direction == LEFT || p1.direction == RIGHT){
		if (isPressed(up_pin) && !isPressed(down_pin))
			newDirection = UP;
		else if (isPressed(down_pin) && !isPressed(up_pin))
			newDirection = DOWN;
	}
  p1.newDirection = newDirection;


   int newDirection2 = p2.direction;
  up_pin = INPUT_UP_2;  down_pin = INPUT_DOWN_2;  left_pin = INPUT_LEFT_2;  right_pin = INPUT_RIGHT_2;
	if (p2.direction == UP || p2.direction == DOWN){
		if (isPressed(left_pin) && !isPressed(right_pin))
			newDirection2 = LEFT;
		else if (isPressed(right_pin) && !isPressed(left_pin)){
			newDirection2 = RIGHT;
      }
	} else if (p2.direction == LEFT || p2.direction == RIGHT){
		if (isPressed(up_pin) && !isPressed(down_pin))
			newDirection2 = UP;
		else if (isPressed(down_pin) && !isPressed(up_pin))
			newDirection2 = DOWN;
	}
  p2.newDirection = newDirection2;


  if ((p1.newDirection != p1.direction) || (p2.newDirection != p2.direction)){
    noTone(SOUND);
    tone(SOUND, 220, 100);
  }
	//Move the players
	p1.movePlayer();
	p2.movePlayer();

	//Check for crashes, change game state accordingly
	if ((p1.hasCrashed() && p2.hasCrashed()) || p1.samePointAs(p2))
		changeState(DRAW);
	else if (p1.hasCrashed())
		changeState(P2_WIN);
	else if (p2.hasCrashed())
		changeState(P1_WIN);
	else { //If there are no crashes, then draw the new positions of the players
		display.drawPixel(p1.x, p1.y, WHITE);
		display.drawPixel(p2.x, p2.y, WHITE);
	}

  display.display();
}

void mainMenuSetup(){
  display.clearDisplay();
  display.setTextColor(WHITE);

  display.setTextSize(5);
  display.setCursor(5, 3);
  display.println("TRON");

  display.setCursor(7, 50);
  display.setTextSize(1);
  display.println("Press any button...");
  display.display();
}

void winScreenSetup(int winner){//winner = 0 is a draw
  crashSound();
  display.clearDisplay();
  display.setTextColor(WHITE);

  display.setTextSize(2);
  display.setCursor(5, 3);
  

  switch (winner){
    case 1: display.println("P1 WINS!"); blinking(RED); break;
    case 2: display.println("P2 WINS!"); blinking(BLUE); break;
    default: display.println("DRAW!"); break;
  }

  display.setCursor(5, 40);
  display.setTextSize(1);
  display.println("Press any button\n  to play again...");
  display.display();
}



void main_menu(){
  if (isAnyPressed())
    changeState(IN_GAME);
}



void changeState(char newState){
  digitalWrite(RED, LOW);
  digitalWrite(BLUE, LOW);
  gameState = newState;
  switch (newState){
    case MAIN_MENU: mainMenuSetup(); break;
    case IN_GAME: inGameSetup(); break;
    case DRAW: winScreenSetup(0); break;
    case P1_WIN: winScreenSetup(1); break;
    case P2_WIN: winScreenSetup(2);break;
  }
}
void crashSound(){
  analogWrite(HAP, 100);
  delay(200);
  
  tone(SOUND, 800, 150); // Descending scale (starting lower)
  delay(150);

  analogWrite(HAP, 0);

  tone(SOUND, 700, 150);
  delay(150);

  tone(SOUND, 600, 150);
  delay(150);

  tone(SOUND, 500, 150);
  delay(150);

  tone(SOUND, 400, 150); // Lowest note
  delay(150);

  tone(SOUND, 300, 300); // Lowest note
  delay(300);
}

void blinking(int pinNum){
    if (millis() % 1000 < 500)
    digitalWrite(pinNum, HIGH);
   else 
    digitalWrite(pinNum, LOW);
}
  




