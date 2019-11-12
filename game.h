#pragma once
#include <FixedPoints.h>
#include <FixedPointsCommon.h>
#include "KeyEvent.h"

class GameState{
  private:
  SQ15x16 player_pos[2];  
  SQ15x16 enemy_pos[2];  
  SQ15x16 enemy_vol[2];  
  uint32_t score;
  bool gameOver;
  KeyEvent keyEvent;
  bool dirtyPlayer;
  bool dirtyEnemy;
  bool dirtyScore;

  public:
  GameState();
  virtual void enter();
  virtual int update();
  virtual void exit();
};
