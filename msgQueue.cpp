#include "msgQueue.h"

MsgQueue::MsgQueue(void) : _size(0) {}
int MsgQueue::enQueue(Msg key){
  if (_size >= MAX_QUEUE_SIZE){
    return 0;  
  }
  _buffer[_size++]  = key;
  return 1; 
}
int MsgQueue::deQueue(Msg * key){
  if (_size == 0){
    return 0;  
  }  
  if (key)
    *key = _buffer[0];
  for (size_t i = 1; i < _size; i ++){
    _buffer[i - 1] = _buffer[i];  
  }

  _size --;
  return 1;
}

size_t MsgQueue::getSize(){
  return _size;  
}
