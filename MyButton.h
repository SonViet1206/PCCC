#ifndef _MYBUTTON_H_
#define _MYBUTTON_H_
#include"Arduino.h"

class MyButton
{
  private:
    int buttonPin;
    int debounceDelay;
    bool preState;//biến lưu trạng thái trước đấy
    bool currentState ;//biến trạng thái hiện tại
    bool lastAfterFilterNoiseState;//biến khử nhiễu
    uint32_t lastDebounceTime = 0;
    uint32_t DebounceTime = 20;
    uint32_t timePress=0;
    uint32_t timeRelease=0;
    
    bool stype=true;//0 là cực máng hở , 1 là kéo lên 
    bool isPress=false;
    bool isRealse=false;
    typedef void(*funpointer)(void);
    int sortTime=1000;
 
    int longTime=4000;
   
    bool sortDetect=false;
    
    bool longDetect=false;
    
  public:
    MyButton(int pin,bool stype,int debounceDelay,int sortTime,int longTime);
    void Button_loop();
    bool SortPress();

    bool LongPress();
   
    void Reset();
   
   
};

#endif
