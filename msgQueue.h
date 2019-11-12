#pragma once
#include <stdint.h>
#include <stdlib.h>

#define MAX_QUEUE_SIZE 10

using Msg = uint32_t;

class MsgQueue {
  private:
  Msg _buffer[MAX_QUEUE_SIZE];
  Msg _size;

  public:
  MsgQueue(); 
  size_t getSize();
  int deQueue(Msg *);
  int enQueue(Msg);
  inline void flush(){
    while(getSize()){
      deQueue(NULL);  
    }  
  }
};
