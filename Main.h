#ifndef _MAIN_H_
#define _MAIN_H_


#include <Arduino.h>
#include <time.h>
#include <sys/time.h>
#include <cstring>
// #include <TimeLib.h>
// #include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t
#include "OTA.h"
#include <Wire.h>
#include "HT_SSD1306Wire.h"

#include <SPI.h>
#include <RadioLib.h>

#include "MyEEPROM.h"

#include "Common.h"
#include "Mesh.h"
#include"myButton.h"

//#define MASTER_PROCESS        //Dinh huong trinh bien dich MASTER module
#define CONTROL_PROCESS       //Dinh huong trinh bien dich MESH module

#define RF_PACKET_LENGTH 64

//Cac dinh nghia trnag thai:
#define STATE_NORMAL 0     //
#define STATE_ALARM 1   //
#define STATE_ALARM_LINK 2   //

//Cac dinh nghia chan ngoại vi:
#define BUILDIN_LED_PIN    35

#define LED_OUT_RED_PIN 6     //
#define LED_OUT_GREEN_PIN 1   //
#define BUZ_OUT_PIN 20  //

#define BTN_IN_RED_PIN 37     //
#define BTN_IN_GREEN_PIN 46   //

#define OLED_RST_PIN 21
#define OLED_SDA_PIN 17
#define OLED_SCL_PIN 18

#define LORA_RST_PIN 12
#define LORA_CS_PIN 8
#define LORA_DI0_PIN 14
#define LORA_BUSY_PIN 13
#define LORA_SCK_PIN 9
#define LORA_MISO_PIN 11
#define LORA_MOSI_PIN 10

#define BAND 868000000  //868E6

extern int led_state;
extern int led_state_last;
extern const char BUILT_DATE_TIME[];
extern const char *MONTHS[];
extern char n_nodeName[20];
extern SSD1306Wire  display;
//Cấu trúc của struct trong bộ nhớ khai báo là uint16_t vẫn cần lưu trữ bằng 4 byte
//Vì vậy mọi biến nên khai báo tối thiểu 4 byte
struct EEPROM_CONFIG_TYPE {
  uint32_t serial;  //8 Số định danh của bộ đếm, duy nhất trong hệ thống để phân biệt
  uint16_t net_id;  //

  uint8_t route_id;  //

  uint8_t rf_type;   //0: Default, 1:Star2013, 2:Star2015, 3:Gelex
  uint8_t tx_power;  //SILAB output Tx power: 7=100mW, 6=50mW, 5=25mW, 4=12.5
  uint8_t lora_mode;
  uint8_t debug_mode;

  uint16_t next_timeout;  //Thoi gian timeout de thuc hien lenh noroute tiep theo
  uint16_t next_execute;  //Thoi gian cho tra loi ket qua

  uint8_t next_random;  //Khoang thoi gian cho ngau nhien
  uint8_t max_retry;    //So lan thuc hien chuyen tiep toi da
  uint8_t ascii_mode;   //Chi anh huong voi MASTER.
  uint16_t time_OK=5000;
  uint16_t time_ALARM=1000;
  uint8_t maxLever=10;
  char nodeName[20]={0};
  //char aes128_password[17];



  char ssid[20];         //20 String        _ssid                   = "";
  char password[20];     //20 String        _pass                   = "";
  IPAddress static_ip;   //4 Địa chỉ IP tĩnh
  IPAddress static_gw;   //4 Địa chỉ Gateway
  IPAddress static_sn;   //4 Sub Net Mask
  IPAddress server_ip;   //4
  uint32_t server_port;  //2
  time_t last_latch;     //Thời điểm reset số liệu lần cuối.
  uint32_t address;      //Địa chỉ bắt đầu của ghi trên EEPROM, tịnh tiến sau mỗi lần bật tắt điện.
  uint32_t loop_count;   //Số lần bộ nhớ quay lại địa chỉ ban đầu

  //double offset_pitch = 0;
  //double k_p; //Set this first
  //double k_d; //Set this secound
  //double k_i; //Finally set this

  //Vi dia chi FLASH truy cap 4 byte
  uint16_t aaa;
  uint32_t u32Temp;    //Byte cuoi cung khong quan trong co duoc doc/ghi hay khong
  uint32_t crc_check;  //2 Biến dùng để kiểm tra cấu trúc config hợp lệ

  uint8_t group_id;  //
};

extern EEPROM_CONFIG_TYPE m_cfg;  //Bien luu thong tin doc tu FLASH
extern MyEEPROM eeprom;



void EEPROM_LoadDefault(void);
void EEPROM_ReadConfig(void);
void EEPROM_WriteConfig(void);
void EEPROM_PrintConfig(Stream &_stream);

void I2CScan(Stream &_stream);

char AT_Command_Process(Stream &_stream, char *sour, char len);
void WriteDebugRX(char *packet);

int LoRa_sendMessage(char *txBuffer, size_t tx_size);

#endif