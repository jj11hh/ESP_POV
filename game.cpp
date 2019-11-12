#include <FixedPoints.h>
#include <FixedPointsCommon.h>

#include "game.h"
#include "common.h"
#include "display.h"
#include "msgQueue.h"
#include "FixedPointsMath.h"
#include "KeyEvent.h"

#define ENEMY_RADIU 2
#define PLAYER_RADIU 3

extern MsgQueue keyQueue;
extern uint8_t keyState;

GameState::GameState(): score(0){
}

void GameState::enter(){
  randomSeed(micros());
  player_pos[0] = 0;  
  player_pos[1] = -8;  
  enemy_pos[1] = 20;
  enemy_pos[0] = random(-10, 10);
  enemy_vol[0] = ((player_pos[0] - enemy_pos[0]) / 60);
  enemy_vol[1] = ((player_pos[1] - enemy_pos[1]) / 60);
  score = 0;
  gameOver = false;
  clearBg();
  keyQueue.flush();
  keyEvent.lastKey = keyState; 
  keyEvent.onKeyDown([this](uint8_t key, uint32_t keyState){
    if (key == KEY_UP && player_pos[1] < 0){
      player_pos[1] += 0.5;  
    }
    if (key == KEY_DOWN && player_pos[1] > -15){
      player_pos[1] -= 0.5;  
    }  
    if (key == KEY_LEFT && player_pos[0] > -10){
      player_pos[0] -= 0.5;  
    }
    if (key == KEY_RIGHT && player_pos[0] < 10){
      player_pos[0] += 0.5;  
    }
    if (key == KEY_A){
      gameOver = true;  
    }
  });

  keyEvent.onKeyUpEdge([this](uint8_t key, uint32_t keyState){
    switch (key){
      case KEY_A:
        gameOver = true;
    }  
  });
}

static void cart2polor(SQ15x16 x, SQ15x16 y, SQ15x16 * r, SQ15x16 * theta){
  *r = sqrtfp(x*x + y*y);
  *theta = atan2fp(y, x);
}

int GameState::update(){
  SQ15x16 r, theta;

  clearScr();
  // clear
  //cart2polor(player_pos[0], -player_pos[1], &r, &theta);
  //clearCircle(r, theta, PLAYER_RADIU);
  //cart2polor(enemy_pos[0], -enemy_pos[1], &r, &theta);
  //clearCircle(r, theta, ENEMY_RADIU);
  //clearText5x7(9, SCANLINE / 4 + (String(score).length() * 5 )/2, String(score).c_str() , true);
  
  //handle key events
  keyEvent.update(&keyQueue);
  
  //render enemy
  enemy_pos[0] += enemy_vol[0];
  enemy_pos[1] += enemy_vol[1];
  cart2polor(enemy_pos[0], -enemy_pos[1], &r, &theta);
  fillCircle(r, theta, ENEMY_RADIU, Color(255, 0, 0));

  //render player  
  cart2polor(player_pos[0], -player_pos[1], &r, &theta);
  fillCircle(r, theta, PLAYER_RADIU, Color(0, 255, 0));

  //render score borad
  drawText5x7(9, SCANLINE / 4 + (String(score).length() * 5 )/2, String(score).c_str(), Color(0, 255, 255), true);

  if (enemy_pos[1] < -16){
    score ++;
    enemy_pos[1] = 20;
    enemy_pos[0] = random(-10, 10);

    enemy_vol[0] = ((player_pos[0] - enemy_pos[0]) / 60) * uipowfp(SQ15x16(1.05), score);
    enemy_vol[1] = ((player_pos[1] - enemy_pos[1]) / 60) * uipowfp(SQ15x16(1.05), score);
    //enemy_vol[0] = SQ15x16(random(-10, 10))/100 * (1 + SQ15x16(0.05) * score);
    //enemy_vol[1] = -0.2  * (1 + SQ15x16(0.05) * score);
  }
  if (sqrtfp(uipowfp(player_pos[0] - enemy_pos[0], 2) 
           + uipowfp(player_pos[1] - enemy_pos[1], 2)) < ENEMY_RADIU + PLAYER_RADIU){
    score = 0;  
    gameOver = true;
  }

  if (gameOver)
    return 0;

  return 1;
}

void GameState::exit(){
  keyQueue.flush();
  keyEvent.lastKey = 0;
  drawText5x7(8, SCANLINE / 2 + (SCANLINE/2 - 9*5)/2, "Game Over", Color(255, 0, 255));
  commitBg();
};
