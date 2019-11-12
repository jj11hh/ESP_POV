#include "KeyEvent.h"

static std::function<void(uint8_t, uint32_t)> null_handle = [](uint8_t key, uint32_t keyState){};

KeyEvent::KeyEvent():lastKey(0), keyDownEdge(null_handle), keyUpEdge(null_handle), keyDown(null_handle), keyUp(null_handle), isBrk(false){}
void KeyEvent::onKeyDownEdge(std::function<void(uint8_t, uint32_t)> handle){
  keyDownEdge = handle;
}
void KeyEvent::onKeyUpEdge(std::function<void(uint8_t, uint32_t)> handle){
  keyUpEdge = handle;
} 
void KeyEvent::onKeyDown(std::function<void(uint8_t, uint32_t)> handle){
  keyDown = handle;
}
void KeyEvent::onKeyUp(std::function<void(uint8_t, uint32_t)> handle){
  keyUp = handle;
} 
void KeyEvent::update(MsgQueue * keyQueue){
  uint32_t key;
  if (!keyQueue->getSize()){
    key = lastKey;
    for (uint8_t i = 0; i < 6; i ++){
      if (key & (1 << i)){
        keyDown(1 << i, key);
      }
      else {
        keyUp(1 << i, key); 
      }
      if (isBrk){
        isBrk = false;
        return;  
      }
    }
  }
  while(keyQueue->getSize()){
    keyQueue->deQueue(&key);
    for (uint8_t i = 0; i < 6; i ++){
      if (key & (1 << i)){
        keyDown(1 << i, key);
        if (isBrk){
          isBrk = false;
          return;  
        }
        if (!(lastKey & (i << i))){
          keyDownEdge(1 << i, key);  
        }
        if (isBrk){
          isBrk = false;
          return;  
        }
      }
      else {
        keyUp(1 << i, key); 
        if (isBrk){
          isBrk = false;
          return;  
        }
        if (lastKey & (i << i)){
          keyUpEdge(1 << i, key);
        } 
        if (isBrk){
          isBrk = false;
          return;  
        }
      }
    }
    lastKey = key;
  }
}

void KeyEvent::brk(){isBrk = true;}
