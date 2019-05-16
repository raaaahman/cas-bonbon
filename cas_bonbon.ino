#include <Gamebuino-Meta.h>

int SCREEN_WIDTH = 80;
int SCREEN_HEIGHT = 64;    
int GUI_HEIGHT = 6;
int BALL_RADIUS = 1;
int PADDLE_WIDTH = 14;
int PADDLE_HEIGHT = 3;
int BRICK_WIDTH = 8;
int BRICK_HEIGHT = 4;
const int GRID_WIDTH = 8;
int screen = 0;
const int SCREEN_MENU = 0;
const int SCREEN_GAME = 1;
const int SCREEN_GAME_OVER = 2;

int currentLevel;
int lives;
int posX;
int posY;
int speedX;
int speedY;
bool ballLaunched;
int padPosX;
int padPosY;

int bricks[GRID_WIDTH];


void setLevel( int levelNb ) {
    for ( int brickNb = 0; brickNb < GRID_WIDTH; brickNb++) {
      bricks[brickNb] = 1;
    }
}

void resetGame() {
  lives = 3;
  currentLevel = 1;
}

void resetBall() {

    padPosY = 59;
    padPosX = SCREEN_WIDTH / 2 - ( PADDLE_WIDTH / 2 );

    posY = padPosY - BALL_RADIUS;
    posX = padPosX + ( PADDLE_WIDTH / 2 );
    speedX = 0;
    speedY = 0;

    ballLaunched = false;
}

void startGame() {
  screen = SCREEN_GAME;
  resetGame();
  setLevel( currentLevel );
  resetBall();  
}
  

char checkCollision( int entityX, int entityY, int entityWidth, int entityHeight ) {
  char side = 'X';

  if ( posY + BALL_RADIUS >= entityY && 
  posY - BALL_RADIUS <= entityY + entityHeight &&
  posX + BALL_RADIUS >= entityX && 
  posX - BALL_RADIUS <= entityX + entityWidth ) {
    
    int lastPosX = posX - speedX;
    int lastPosY = posY - speedY;
    
    if ( lastPosX + BALL_RADIUS < entityX ) {
      side = 'L';
    } else if ( lastPosX - BALL_RADIUS > entityX + entityWidth ) {
      side = 'R';
    } else if ( lastPosY - BALL_RADIUS > entityY + entityHeight ) {
      side = 'U';
    } else if ( lastPosY + BALL_RADIUS < entityY ) {
      side = 'A';
    }
  }

  return side;
}

void mainMenu() {
  gb.display.setCursorX(0);
  gb.display.setCursorY(10);
  gb.display.setFontSize(2);
  gb.display.setColor(PINK);
  gb.display.printf("CAS'BONBON");

  gb.display.setFontSize(1);
  gb.display.setCursorY(40);
  gb.display.setColor(WHITE);
  gb.display.printf("Ready to Play ? \n Press A");

  
  if (gb.buttons.pressed(BUTTON_A)) {
    startGame();
  }
}

void runGame() {
  gb.display.setFontSize(1);
  gb.display.printf( "Lives %i", lives);
  gb.display.setCursorX( 40 );
  gb.display.printf( "Level %i", currentLevel);
  // THE PADDLE
  if (gb.buttons.pressed(BUTTON_RIGHT)) {  // If we press RIGHT
    padPosX = padPosX + 8;    // Add 4 to "counter"
    if (padPosX > SCREEN_WIDTH - PADDLE_WIDTH) {
      padPosX = SCREEN_WIDTH - PADDLE_WIDTH;
    }
  }  // End of IF

   if (gb.buttons.pressed(BUTTON_LEFT)) {  // If we press LEFT
    padPosX = padPosX - 8; //  Remove 4 to "counter"
    if (padPosX < 0){
      padPosX=0;
    }
  } 

  gb.display.setColor(YELLOW);
  gb.display.fillRect(padPosX, padPosY, PADDLE_WIDTH, PADDLE_HEIGHT);  // Draw the rectangle

  // THE BALL
  if ( ballLaunched == false ) {
    posX = padPosX + ( PADDLE_WIDTH / 2 );
    posY = padPosY - BALL_RADIUS;
    
    if (gb.buttons.pressed(BUTTON_A)) {
      speedY = -1;
      speedX = rand() > 0.5 ? -1 : 1;
      ballLaunched = true;
    }
  }
  posX += speedX;
  posY += speedY;

  if ( posX + BALL_RADIUS > SCREEN_WIDTH ) {
    posX = SCREEN_WIDTH - BALL_RADIUS - ( posX + BALL_RADIUS + speedX - SCREEN_WIDTH );
    speedX = -speedX;  
  } else if ( posX - BALL_RADIUS < 0 ) {
    posX = BALL_RADIUS - ( posX - BALL_RADIUS + speedX );
    speedX = -speedX;
  }

  switch ( checkCollision( padPosX, padPosY, PADDLE_WIDTH, PADDLE_HEIGHT ) ) {
    case 'A' :
      posY = padPosY - ( padPosY - ( posY + BALL_RADIUS ) ) - BALL_RADIUS;
      speedY = -speedY;
      break;
    case 'L' :
    case 'R' :
      speedX = -speedX;
      break;
    default:
      if ( posY + BALL_RADIUS > SCREEN_HEIGHT ) {
        if ( --lives == 0 ) {
          screen = SCREEN_GAME_OVER;
        } else {
          resetBall();
        }
      } else if ( posY - BALL_RADIUS < GUI_HEIGHT ) {
        posY = GUI_HEIGHT + BALL_RADIUS + ( GUI_HEIGHT - ( posY - BALL_RADIUS ) );
        speedY = -speedY;
      }
      break;
  }

  gb.display.setColor(RED);
  gb.display.fillRect(posX - BALL_RADIUS, posY - BALL_RADIUS, BALL_RADIUS * 2, BALL_RADIUS * 2);

   //THE BRICKS
   bool remaining = false;
  for ( int i = 0; i < GRID_WIDTH; i++ ) {
    int brickX =  1 + i * ( 2 + BRICK_WIDTH );
    int brickY = 20;
    
    if ( bricks[i] == 1 ) {
      switch ( checkCollision( brickX, brickY, BRICK_WIDTH, BRICK_HEIGHT ) ) {
        case 'A' :
        case 'U' : 
          speedY = -speedY;
          bricks[i] = 0;
          break;
        case 'L' :
        case 'R' :
          speedX = -speedX;
          bricks[i] = 0;
          break;
      }
    }
    
    if ( bricks[i] == 1 ) {
      remaining = true;
      gb.display.setColor(PINK);
      gb.display.fillRoundRect( brickX, brickY, BRICK_WIDTH, BRICK_HEIGHT, BRICK_HEIGHT / 2 );
    }
  }

  if ( remaining == false ) {
    setLevel( currentLevel++ );
    resetBall();
  }
}

void gameOver() {
  gb.display.setCursorX(5);
  gb.display.setCursorY(15);
  gb.display.setFontSize(2);
  gb.display.setColor(ORANGE);
  gb.display.printf( "Game Over\n" );

  gb.display.setCursorX(8);
  gb.display.setFontSize(1);
  gb.display.setColor(WHITE);
  gb.display.printf( "Replay? Press A");

  if ( gb.buttons.pressed(BUTTON_A) ) {
    startGame();
  }
}


// The Game starts
void setup() {
  gb.begin();
}

void loop() {
  while(!gb.update());
  gb.display.clear();

  switch ( screen ) {
    case SCREEN_GAME :
      runGame();
      break;
    case SCREEN_GAME_OVER :
      gameOver();
      break;
    case SCREEN_MENU :
    default : 
      mainMenu();
      break;
  }

}
