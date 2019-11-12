#pragma once

#include <functional>
#include "common.h"
#include "msgQueue.h"

class KeyEvent{
  private:
    std::function<void(uint8_t, uint32_t)> keyDownEdge;
    std::function<void(uint8_t, uint32_t)> keyUpEdge;
    std::function<void(uint8_t, uint32_t)> keyDown;
    std::function<void(uint8_t, uint32_t)> keyUp;
    bool isBrk;
  public:
    uint32_t lastKey;
    KeyEvent();
    void brk();
    void onKeyDownEdge(std::function<void(uint8_t, uint32_t)>);  
    void onKeyUpEdge(std::function<void(uint8_t, uint32_t)>);  
    void onKeyDown(std::function<void(uint8_t, uint32_t)>);  
    void onKeyUp(std::function<void(uint8_t, uint32_t)>);  
    void update(MsgQueue * keyQueue);
};
