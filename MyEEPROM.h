#ifndef my_eeprom_h
#define my_eeprom_h

#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>

#define eeprom_on_arduino 0  // chọn eeprom của arduino
#define eeprom_on_ic 1       // chọn eeprom của ic ngoại vi

class MyEEPROM 
{
  public:
    MyEEPROM(uint32_t size_in_byte);
    MyEEPROM(uint32_t size_m, byte deviceAddress);
    void begin();
    void write_1_byte(uint32_t address, byte data);
    byte read_1_byte(uint32_t address);
    
    int ReadMultiByte(uint16_t data_addr, byte *buff_data, int quantity);
    int WriteMultiByte(uint16_t data_addr, const byte *buff_data, int quantity);

    bool TestMem(Stream &_stream);
    unsigned long size_bytes;
    
  private:
    void write_byte_eeprom_ic(uint32_t address,byte  data);
    byte read_byte_eeprom_ic( uint32_t address);
    void write_byte_eeprom_arduino(uint32_t address,byte  data);
    byte read_byte_eeprom_arduino( uint32_t address);
    byte select_eeprom;
    // select eeprom=0 // arduino
    //select eeprom=1 // ic eeprom 
    uint32_t size_memory;
    byte m_deviceAddress;
};

#endif
