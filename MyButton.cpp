#include"myButton.h"

MyButton::MyButton(int pin,bool stype,int debounceDelay,int sortTime,int longTime)
{
  this->buttonPin=pin;
  this->stype = stype;
  this->DebounceTime=debounceDelay;
  this->sortTime=sortTime;
  
  this->longTime=longTime;
  if(this->stype==true)
  {
    pinMode(this->buttonPin,INPUT_PULLUP);
  }
  else
  {
    pinMode(this->buttonPin,INPUT);
  }
  
}
void MyButton::Button_loop()
{
  this->sortDetect=false;
  this->longDetect=false;
  this->currentState = digitalRead(buttonPin);
  if(this->currentState!=this->lastAfterFilterNoiseState)
  {
    this->lastDebounceTime=millis();
    this->lastAfterFilterNoiseState=this->currentState;
  }
  if((millis()-(this->lastDebounceTime))>(this->DebounceTime))
  {
    if(this->preState==true && this->currentState==false)//nhan phim
    {
      this->timePress=millis();
      this->longDetect=false;
      this->isPress=true;
      
    }
    
    else if(this->preState==false && this->currentState==true)//nha phim
    {
      this->isPress=false;
      this->timeRelease=millis();
      if((this->timeRelease)-(this->timePress)<=sortTime)//bấm ngắn
      {
        
        this->sortDetect=true;
        
        this->longDetect=false;
       
      }
      
    }
    else
    {
      
    }
    //==================================================
    
    this->preState=this->currentState;
  }
  
  if((this->isPress==true)&&(this->longDetect==false))
  {
    
    uint32_t pressDuration=millis()-(this->timePress);
    
    if(pressDuration>(this->longTime))   //bấm dài
    {
      this->sortDetect=false;
      this->longDetect=true;
      this->isPress=false;
      
    }
    
  }
  
}
bool MyButton::SortPress()
{
  return this->sortDetect;
}

bool MyButton::LongPress()
{
  return this->longDetect;
}
void MyButton::Reset()
{
  
}
