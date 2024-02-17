#include "main.h"

//Khởi tạo chọn bộ nhớ EEPROM của chip Arduino
MyEEPROM::MyEEPROM(uint32_t size_in_byte){
  select_eeprom=eeprom_on_arduino;// chọn phương thức lưu trên arduino
  size_bytes = size_in_byte;
  EEPROM.begin(size_in_byte);
}

//Khởi tạo tham số chọn bộ nhớ EEPROM ngoài họ 24Cxx
MyEEPROM::MyEEPROM(uint32_t size_m, byte deviceAddress){
  m_deviceAddress = deviceAddress;
  size_memory=size_m;
  size_bytes=size_m*128; //  size_bytes=(size_m*1024)/8; 
  // tìm số byte mà ic có thể lưu trữ
  // ví dụ ic 24c04 có (4*1024)/8=512 bytes
  //byte đầu tiên là 0. byte cuối là 511
  
  select_eeprom=eeprom_on_ic;// chọn phương thức lưu trên ic
  Wire.begin();
}

void MyEEPROM::begin(){
  if(select_eeprom==eeprom_on_arduino){
    EEPROM.begin(size_bytes);
  }else if( select_eeprom==eeprom_on_ic){
    Wire.begin();
  }
  
}

void MyEEPROM::write_byte_eeprom_ic(uint32_t   address,byte  data){
  // nếu quá địa chỉ thì không ghi -> thoát
  // nếu dữ liệu đưa vào bằng dữ liệu cũ thì không cần ghi ->thoát
  //( tiết kiệm số lần ghi)
  if((address < size_bytes) && (read_1_byte(address) != data)) {
    int delay_ms = 1;
    // ví dụ ic 24c04 có (4*1024)/8=512 bytes
    //byte đầu tiên là 0. byte cuối là 511
    if( size_memory<=2){
      //24c01 và 24c02
      Wire.beginTransmission(m_deviceAddress);
      Wire.write((byte)address);//địa chỉ 8 bit 
      delay_ms = 1;
    }else if(size_memory<=16){ 
      //24c04, 24c08 và 24c16
      // vì kích thước của adress lớn hơn 8bit : ví dụ số 300.
      //khi đó ic được cấu tạo từ các đơn vị ic nhỏ hơn
      //ví dụ 24c04 thực chất được cấu tạo từ 2 ic 24c02 dùng chung 1 đường bus i2c
      //mỗi ic được gọi là 1 page
      //khi đó pin A0 sẽ được dùng giống như pin chọn chip (chọn page)
      // lúc này địa chỉ của ic là : 1.0.1.0.A2.A1.0 (page 1) và 1.0.1.0.A2.A1.1 (page 2)
      // page 1 có thể lưu được ô nhớ từ 0->255, page 2 lưu ô nhớ từ 256->511
      
      // XEM LẠI BÀI VIẾT CỦA MÌNH TRÊN TRANG ARDUINO.VN-THÁI SƠN
      Wire.beginTransmission((byte)(m_deviceAddress | ((address >> 8) & 0x07)));
      Wire.write(address & 0xFF);
      delay_ms = 1;
    } else if(size_memory<=256){
      //24c32, 24c64, 24c128, 24c256
      Wire.beginTransmission(m_deviceAddress);
      Wire.write(address >> 8);
      Wire.write(address & 0xFF);
      delay_ms = 4;
    }
    Wire.write(data);
    Wire.endTransmission();
    delay(delay_ms);
  }
  //yield(); //Reset Watchdog Timer, phải dùng khi sử dụng delayMicroseconds
}

byte MyEEPROM::read_byte_eeprom_ic(uint32_t address){
  byte data = 0;
  if(address < size_bytes){
    if( size_memory<=2){
      Wire.beginTransmission(m_deviceAddress);
      Wire.write(address);
      Wire.endTransmission();
      Wire.requestFrom(m_deviceAddress, (byte)1);
    }else if(size_memory<=16){
      //24c04, 24c08 và 24c16
      Wire.beginTransmission((byte)(m_deviceAddress | ((address >> 8) & 0x07)));
      Wire.write(address & 0xFF);
      Wire.endTransmission();
      Wire.requestFrom((byte)(m_deviceAddress | ((address >> 8) & 0x07)), (byte)1);
    } else if(size_memory<=256){
      //24c32, 24c64, 24c128, 24c256
      Wire.beginTransmission(m_deviceAddress);
      Wire.write(address >> 8);
      Wire.write(address & 0xFF);
      Wire.endTransmission();
      Wire.requestFrom(m_deviceAddress, (byte)1);
    }
    data = Wire.read();
  }
  return data;
}

void MyEEPROM::write_byte_eeprom_arduino(uint32_t   address,byte  data){
  // nếu dữ liệu đưa vào bằng dữ liệu cũ thì không cần ghi ->thoát ( tiết kiệm số lần ghi)
  if(read_1_byte(address) != data){
    EEPROM.write(address, data);
    //EEPROM.commit();
    //delay(1);  
  }
}

byte MyEEPROM::read_byte_eeprom_arduino( uint32_t address){
  byte data = EEPROM.read(address);
  return data;
}

// hàm chuyên biệt
void MyEEPROM::write_1_byte (uint32_t address,byte data){
  if(select_eeprom==eeprom_on_arduino){
    write_byte_eeprom_arduino(address, data);
  }else if( select_eeprom==eeprom_on_ic){
    write_byte_eeprom_ic(address, data);
  }
}

byte MyEEPROM::read_1_byte (uint32_t address){
  byte byt = 0;
  if(select_eeprom==eeprom_on_arduino){
    byt = read_byte_eeprom_arduino(address);
  }else if( select_eeprom==eeprom_on_ic){
    byt = read_byte_eeprom_ic(address);
  }
  return byt;
}

int MyEEPROM::ReadMultiByte(uint16_t data_addr, byte *buff_data, int quantity)
{
  for(int i=0;i<quantity;i++)
  {
    buff_data[i] = read_1_byte(data_addr++);
  }
  return quantity;
}
    
int MyEEPROM::WriteMultiByte(uint16_t data_addr, const byte *buff_data, int quantity)
{
  byte dataR;
  for (int i=0; i<quantity; i++)  
  {
    write_1_byte(data_addr, *buff_data);
    //Thủ tục này ghi có xác nhận thành công
    dataR = read_1_byte(data_addr);
    if (dataR != *buff_data) return i; //Tra lai vi tri ghi gap loi
    ++buff_data;
    ++data_addr;
  }
  if(select_eeprom==eeprom_on_arduino) 
  {
    EEPROM.commit();
    //delay(100);
  }
  return -1; //Khang dinh ghi thanh cong
}

bool MyEEPROM::TestMem(Stream &_stream) {
  bool retOK = true;
  bool testOK;
  uint8_t buffW[] = "012345678901234567890123456789012345678901"; //=42
  uint8_t buffB[33];
  uint8_t buffR[33];

  uint32_t test_time = millis();
   
  _stream.printf("\r\nTest %s EEPROM, memory size=%u bytes", (select_eeprom==eeprom_on_arduino ? "internal":"external"), size_bytes);
    
  for (int i = 0; i < (size_bytes / 32); i++) {
    //Luu lai gia tri ROM cu:
    ReadMultiByte(32 * i, buffB, 32);
    //Ghi chuoi kiem tra:
    WriteMultiByte(32 * i, &buffW[i%10], 32);
    //Doc lai chuoi kiem tra:
    memset(buffR, 0, 33);
    ReadMultiByte(32 * i, buffR, 32);
    testOK = (memcmp(&buffW[i%10], buffR, 32) == 0);
    if (testOK == false) retOK = false;
    _stream.printf("\r\nRead/Write(%u-%u)=%s - %s", 32 * i, 32 * (i + 1) - 1, buffR, testOK ? "OK" : "NG");
    //Ghi lai chuoi du lieu goc:
    WriteMultiByte(32 * i, buffB, 32);
  } 
  test_time = millis() - test_time;
  _stream.printf("\r\nTest completed in %u(ms). Result=%s\r\n", test_time, retOK ? "OK" : "NG");
  return retOK;
}
