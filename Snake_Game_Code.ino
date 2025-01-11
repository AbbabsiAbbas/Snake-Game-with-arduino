#include <LiquidCrystal_I2C.h>

#define       JOYSTICK_button                 7
#define       JOYSTICK_horizontal                 A0
#define       JOYSTICK_vertical                 A1
#define       JOYSTICK_DIRECTION_UP          1
#define       JOYSTICK_DIRECTION_DOWN        2
#define       JOYSTICK_DIRECTION_LEFT        3
#define       JOYSTICK_DIRECTION_RIGHT       4
#define       ROWS                           2 

LiquidCrystal_I2C lcd(0x27, 16, 2); //0x27 is LCD address 
boolean  gameOver, gameStarted;
int old_direction;

byte current_joystick_input = 0;   
byte previous_joystick_input = 0;   


struct snakePart
{
  int row, column, dir;     
  struct snakePart *next;    
}; 
typedef snakePart SnakePart;  // 
SnakePart *head, *tail;

byte matrix[8*ROWS*16];
unsigned int pointColumn, pointRow;//snake food
int score;
bool change_direction;

//----------------------------------------------

void displayOnScreen()
{
  byte myChar[8];
  boolean highpixle;
  int address=0;
  
  if (!gameOver)
  {
  LightPixle(pointRow, pointColumn, true);
  
  for(int r=0;r<ROWS;r++)
  {    
    for(int c=0;c<16;c++)
    {
      highpixle = false;
      for(int i=0;i<8;i++)
      {
        byte charrow=B00000;

        for (int j = 0; j < 5; j++) {
  if ((matrix[16 * (r*8 + i) + (c*5 + j) / 5] & (1 << ((c*5 + j) % 5))) >> ((c*5 + j) % 5)) {
    charrow += (B10000 >> j);
    highpixle = true;
  }
}
        myChar[i] = charrow;

      }
      if (highpixle) 
      {
        lcd.createChar(address, myChar);
        lcd.setCursor(c,r);
        lcd.write(byte(address));
        address++;
      }
      else 
      {
        lcd.setCursor(c,r);
        lcd.write(128); //empty character
    }
   }
  } 
 }
}
//----------------------------------
void freeList()
{
  SnakePart *p,*q;
  p = tail;
  while (p!=NULL)
  {
    q = p;
    p = p->next;
    free(q);
  }
  head = tail = NULL;
}
//----------------------------------
void gameOverFunction()
{
  delay(1000);
  lcd.clear();
  freeList();
  lcd.setCursor(3,0);
  lcd.print("Game Over!");
  lcd.setCursor(4,1);
  lcd.print("Score: ");
  lcd.print(score);
  delay(15000);
}
//----------------------------------
void growSnake()
{
  SnakePart *p;
  p = (SnakePart*)malloc(sizeof(SnakePart));
   p->column = tail->column;
  p->row = tail->row;
  p->dir = tail->dir;
  p->next = tail;
  tail = p;
}
//----------------------------------
// a new dot on the screen
void newPoint()
{
 

  
  SnakePart *p;
  p = tail;
  boolean newp = true;
  while (newp)
  {
    pointRow = random(8*ROWS);
    pointColumn = random(80);
    newp = false;
    while (p->next != NULL && !newp) 
    {
      if (p->row == pointRow && p->column == pointColumn) newp = true;
      p = p->next;
    }
  }
  
growSnake();
}
//----------------------------------

// head movement
void moveHead()
{
 
  switch(head->dir) // 1 step in direction
  {
    case 0: head->row--; break;
    case 1: head->row++; break;
    case 2: head->column++; break;
    case 3: head->column--; break;
    default : break;
  }
  if (head->column >= 80) head->column = 0;
  if (head->column < 0) head->column = 79;
  if (head->row >= (8*ROWS)) head->row = 0;
  if (head->row < 0) head->row = (8*ROWS - 1);
  

  
  SnakePart *p;
  p = tail;
  while (p != head && !gameOver) // self collision 
  {
    if (p->row == head->row && p->column == head->column) gameOver = true;
    p = p->next;
  }
  if (gameOver)
    gameOverFunction();
  else
  {
  LightPixle(head->row, head->column, true);
  
  if (head->row == pointRow && head->column == pointColumn) // point pickup check
  {
    score++;
   // if (gameSpeed < 25) gameSpeed+=3;
    newPoint();
  }
  }
}
//----------------------------------
// funny move
void add_Step()
{
  SnakePart *p;
  p = tail;

  LightPixle(p->row, p->column, false);
  
  while (p->next != NULL) 
  {
    p->row = p->next->row;
    p->column = p->next->column;
    p->dir = p->next->dir;
    p = p->next;
  }
  moveHead();
}
//----------------------------------

// create a snake
void createSnake(int n) // n = snake initial size
{
  for (unsigned int i=0;i<(8*ROWS*16);i++)
    matrix[i] = 0;
    
  SnakePart *p, *q;
  tail = (SnakePart*)malloc(sizeof(SnakePart));
  tail->row = 8;
  tail->column = 6;
  tail->dir = 3;
  q = tail;
  
LightPixle(tail->row, tail->column, true);
   delay(5000);
  for (int i = 0; i < n-1; i++) // build snake from tail to head
  {
    p = (SnakePart*)malloc(sizeof(SnakePart));
    p->row = q->row;
    p->column = q->column - 1; //initial snake id placed horizoltally like this ****
    
    LightPixle(p->row, p->column, true);
    
    p->dir = q->dir;
    q->next = p;
    q = p;
  }
  if (n>1)
  {
    p->next = NULL;
    head  = p;
  }
  else 
  {
    tail->next = NULL;
    head = tail;
  }
}

void print_introduction()
{
  byte Snakeshape[8][8] = 
{
{ B00000,
  B00000,
  B00011,
  B00110,
  B01100,
  B11000,
  B00000,
},
{ B00000,
  B11000,
  B11110,
  B00011,
  B00001,
  B00000,
  B00000,
},
{ B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B01110,
},
{ B00000,
  B00000,
  B00011,
  B01111,
  B11000,
  B00000,
  B00000,
},
{ B00000,
  B11100,
  B11111,
  B00001,
  B00000,
  B00000,
  B00000,
},
{ B00000,
  B00000,
  B00000,
  B11000,
  B01101,
  B00111,
  B00000,
},
{ B00000,
  B00000,
  B01110,
  B11011,
  B11111,
  B01110,
  B00000,
},
{ B00000,
  B00010,
  B00100,
  B01000,
  B11110,
  B01000,
  B00010,
}
};
  
  gameOver = false;
  gameStarted = false;

  
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Snake Game!");
  for(int i=0;i<8;i++)
  {
    lcd.createChar(i,Snakeshape[i]);//store the char in ith place
    lcd.setCursor(i+2,1);// choosing the char cordinate 
    lcd.write(byte(i)); // print the char 
  }
  delay(12000);
  score = 0;
  createSnake(4);
}
//----------------------------------
void setup()
{
    
  lcd.begin();
  print_introduction();
}
//----------------------------------
void loop()
{
  if (!gameOver && !gameStarted) 
  {
   current_joystick_input = SnakeDirection(); 
    // Check the joystick
    if (current_joystick_input != previous_joystick_input)    
    {
      delay(50);  
      current_joystick_input = SnakeDirection();
      if (current_joystick_input != previous_joystick_input)    
      {   
      previous_joystick_input = current_joystick_input;
        if (current_joystick_input > 0)
        {
          old_direction = head->dir;
        }
      }
    }
  }
  
  else if (!gameOver && gameStarted)
  // If the game is running
  {
    change_direction = false;
    
    current_joystick_input = SnakeDirection(); 
    if (current_joystick_input != previous_joystick_input)   
    {
      delay(50);  
      current_joystick_input = SnakeDirection();
      if (current_joystick_input != previous_joystick_input)    
      {   
        previous_joystick_input = current_joystick_input;
        if (current_joystick_input > 0)
        {
          
          old_direction = head->dir;
          if (current_joystick_input == JOYSTICK_DIRECTION_RIGHT && head->dir != 3) head->dir = 2;
          else if (current_joystick_input == JOYSTICK_DIRECTION_UP && head->dir != 1) head->dir = 0;
          else if (current_joystick_input == JOYSTICK_DIRECTION_DOWN && head->dir != 0) head->dir = 1;
          else if (current_joystick_input == JOYSTICK_DIRECTION_LEFT && head->dir != 2) head->dir = 3;
          
          if (old_direction != head->dir)
          {
            change_direction = true;
            add_Step();   // Move the snake
            displayOnScreen(); // Draw the updated matrix
          }
        }
      }
    }

    // No need for speed control, just move the snake and update the display
    if (!change_direction)
    {
      add_Step();   
      displayOnScreen(); 
    }
  }

  if (gameOver) // If the game is over
  {
    current_joystick_input = SnakeDirection(); // Check the joystick
    if (current_joystick_input != previous_joystick_input)   
    {
      delay(50);  
      current_joystick_input = SnakeDirection();
      if (current_joystick_input != previous_joystick_input)    
      {   
        previous_joystick_input = current_joystick_input;
        if (current_joystick_input > 0) // Start the game when the joystick is moved
        {
          print_introduction(); // Show the introduction again and restart the game
        }
      }
    }
  }
}

//-----------------------------------
// check the joystick
byte SnakeDirection()
{
  byte Direction = 0;
  unsigned int X_Value = analogRead(JOYSTICK_horizontal);
  unsigned int Y_Value = analogRead(JOYSTICK_vertical);
  
  if ((Y_Value >= 900) && (X_Value >= 400))
  {
    Direction = JOYSTICK_DIRECTION_DOWN;
  }
  else if ((Y_Value <= 400) && (X_Value >= 400))
  {
    Direction = JOYSTICK_DIRECTION_UP;
  }
  else if ((Y_Value >= 400) && (X_Value >= 900))
   {
    Direction =  JOYSTICK_DIRECTION_LEFT ;
   }
  else if ((Y_Value >= 400) && (X_Value <= 400))
  {
    Direction = JOYSTICK_DIRECTION_RIGHT   ;
  }
  
  return Direction;
}

void LightPixle(unsigned int RowVal, unsigned int ColVal, boolean NewVal)
{
    // Calculate the byte index in the Field array
    int byteIndex = 16 * RowVal + (ColVal / 5);// ColVal / 5 because in each character theres 5 columns 
    
    // Calculate the bit position within the byte (0 to 4)
    int bitPosition = ColVal % 5;
    
    // Clear the bit (set the pixel to OFF)
    matrix[byteIndex] &= ~(1 << bitPosition);

    // If NewVal is true, set the bit (turn the pixel ON)
    if (NewVal)
    {
        matrix[byteIndex] |= (1 << bitPosition);
    }
}
