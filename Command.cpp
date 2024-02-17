#include "Main.h"

//File này chứa các hàm liên quan đến xử lý lệnh:
//Hàm này xử lý nội dung lệnh và trả kết quả lại luồng đã nhận lệnh.
//Luồng nhận lệnh có thể là cổng UART bất kỳ hoặc cổng TCP/IP
char AT_Command_Process(Stream &_stream, char *sour, char len) {
  const char AT_OK[] = "\r\nOK\r\n";
  const char AT_ERROR[] = "\r\nERROR\r\n";
  uint16_t u16Temp;
  unsigned int u32Temp;
  int i;
  
  if (memcmp(sour,"AT\r",3) == 0 || memcmp(sour,"at\r",3) == 0) { //AT echo command
    _stream.print(AT_OK);
  }
  else if (memcmp(sour,"AT+HELP\r",8) == 0 || memcmp(sour,"at+help\r",8) == 0) {
    _stream.print("\r\n[DANH SACH LENH AT]");
    _stream.print("\r\n(Lenh ket thuc boi <CR>, khong phan biet chu hoa chu thuong)\r\n");
    _stream.print("\r\n[AT] - Lenh kiem tra phan hoi");
    _stream.print("\r\n[AT+RESTART] - Khoi dong lai");
    _stream.print("\r\n[AT+TESTMEM] - Kiem tra doc ghi EEPROM");
    
    _stream.print(AT_OK);
  }
  else if (memcmp(sour,"AT+RESTART\r",11) == 0 || memcmp(sour,"at+restart\r",11) == 0) {
    _stream.print("\r\nSystem Restarting...\r\n");
    _stream.print(AT_OK);
    ESP.restart();
  }
  else if (memcmp(sour,"AT+TESTMEM\r",11) == 0 || memcmp(sour,"at+testmem\r",11) == 0) {
    eeprom.TestMem(_stream);
    _stream.print(AT_OK);
  }
  else if (memcmp(sour,"AT+DEBUG",8) == 0) {
    if (sour[8] == '?' || sour[8] == '=') {
      if (sour[8] == '=') {
        uint8_t val;
        val = strtoul(&sour[9],NULL,10);
        if (m_cfg.debug_mode != val) { //Co thay doi gia tri thi ghi tro lai flash:
          m_cfg.debug_mode = val;
          EEPROM_WriteConfig();
        }
      }
      _stream.printf("\r\nDebug Mode");
      if (m_cfg.debug_mode == 123) _stream.printf(" ON");
      else _stream.printf("=%u",m_cfg.debug_mode);
      _stream.printf("\r\n%s",AT_OK);
    }
    else _stream.printf(AT_ERROR);
  }
  else if (memcmp(sour,"AT+ASCMOD",9) == 0) {
    if (sour[9] == '?' || sour[9] == '=') {
      if (sour[9] == '=') {
        uint8_t val;
        val = strtoul(&sour[10],NULL,10);
        if ((m_cfg.ascii_mode == 0 && val != 0) || (m_cfg.ascii_mode != 0 && val == 0)) {
          //Co thay doi gia tri thi ghi tro lai flash:
          m_cfg.ascii_mode = val;
          if (m_cfg.ascii_mode != 0) m_cfg.ascii_mode = 1;
          EEPROM_WriteConfig();
        }
      }
      _stream.printf("\r\nASCII Mode=%u\r\n%s",m_cfg.ascii_mode,AT_OK);
    }
    else printf(AT_ERROR);
  }
  else if (memcmp(sour,"AT+TXPOWER=",11) == 0) {
    uint8_t val;
    val = strtoul(&sour[11],NULL,10);
    if (m_cfg.tx_power != val && val > 0 && val < 21) {
      //Co thay doi gia tri thi ghi tro lai flash:
      m_cfg.tx_power = val;
      EEPROM_WriteConfig();
    }
    _stream.printf("\r\nTX Power=%u\r\n",m_cfg.tx_power);
    _stream.printf(AT_OK);
  }
  else if (memcmp(sour,"AT+NETWORKID=",13) == 0) { //
    //AT+NETWORKID=1234\r
    if (len >= 18 && sour[17] == 13) {
      u32Temp = strtoul(&sour[13], NULL, 16);
      //sprintf(Cmd_Buffer,"\r\nu32Temp=%X\r\n",u32Temp); UartWriteS(Cmd_Buffer); //Debug
      if (u32Temp < 65536) { //Network ID hop le
        if (m_cfg.net_id != u32Temp) {
          m_cfg.net_id = u32Temp & 0xFFFF;
          EEPROM_WriteConfig();
        }
        _stream.printf("%04X\r\n%s",m_cfg.net_id,AT_OK); //UartWriteS(sBuff);
      }
      else _stream.printf("\r\n%s",AT_ERROR);
    }
  }
  else if (memcmp(sour,"AT+GROUPID=",11) == 0) { //
    //AT+NETWORKID=1234\r
    if (len >= 12 && sour[13] == 13) {
      uint8_t gr = strtoul(&sour[12], NULL, 16);
      //sprintf(Cmd_Buffer,"\r\nu32Temp=%X\r\n",u32Temp); UartWriteS(Cmd_Buffer); //Debug
      _stream.printf("%d",gr);
      if (gr < 256) { //Network ID hop le
        if (m_cfg.group_id != gr) {
          m_cfg.group_id = gr & 0xFF;
          EEPROM_WriteConfig();
        }
        _stream.printf("%02X\r\n%s",m_cfg.group_id,AT_OK); //UartWriteS(sBuff);
      }
      else _stream.printf("\r\n%s",AT_ERROR);
    }
  }
  else if (memcmp(sour,"AT+NODEID=",10) == 0) { 
    //AT+NODEID=ABCD1111\r
    //char buff[128];
    //Serial.printf("\r\n%s[%u]\r\n", S2SH(buff, sour, len, 0, NULL), len);
    //41542B4E4F444549443D41424344313131310D[19]
    if (len >= 19 && sour[18] == 13) {
      sour[18] = NULL; //Chốt hết chuỗi
      u32Temp = strtoul(&sour[10], NULL, 16);
      //Serial.printf("\r\nu32Temp=%X\r\n",u32Temp); //Debug
      if (u32Temp < 0xFFFFFFFF) { //Serial hp le
        if (m_cfg.serial != u32Temp) {
          m_cfg.serial = u32Temp;
          EEPROM_WriteConfig();
        }
        _stream.printf("\r\n%08X\r\n%s",m_cfg.serial,AT_OK);
      }
      else _stream.printf("\r\n%s",AT_ERROR);
    }
  }
//  else if (memcmp(sour,"AT+ROUTEID=",11) == 0) { //
//    if (len == 15) {
//      u32Temp = strtoul(&sour[11], NULL, 16);
//      if (u32Temp < 256) { //Route ID hop le
//        if (m_cfg.route_id != u32Temp) {
//          m_cfg.route_id = u32Temp & 0xFF;
//          EEPROM_WriteConfig();
//        }
//        _stream.printf("\r\n%02X\r\n%s",m_cfg.route_id,AT_OK);
//      }
//      else {
//        _stream.printf("\r\n%s",AT_ERROR);
//      }
//    }
//  }
  // else if (memcmp(sour,"AT+DATETIME?\r",13) == 0) { //
  //   _stream.printf("\r\n%04d-%02d-%02d %02d:%02d:%02d\r\n%s", year(), month(), day(), hour(), minute(), second(), AT_OK);
  // }
  // else if (memcmp(sour,"AT+SHOWCFG\r",11) == 0 || memcmp(sour,"at+showcfg\r",11) == 0) {
  //   EEPROM_PrintConfig(_stream);
  //   _stream.print(AT_OK);
  // }
//  else if (memcmp(sour,"AT+SHOWCNT\r",11) == 0 || memcmp(sour,"at+showcnt\r",11) == 0) {
//    EEPROM_PrintCounter(_stream);
//    _stream.print(AT_OK);
//  }
  else if (memcmp(sour,"AT+SAVECFG\r",11) == 0 || memcmp(sour,"at+savecfg\r",11) == 0) {
    EEPROM_WriteConfig();
    _stream.print(AT_OK);
  }
//  else if (memcmp(sour,"AT+RESETCNT\r",12) == 0 || memcmp(sour,"at+resetcnt\r",12) == 0) {
//    EEPROM_ResetCounter();
//    _stream.print(AT_OK);
//  }
//  else if (memcmp(sour,"AT+SAVECNT\r",11) == 0 || memcmp(sour,"at+savecnt\r",11) == 0) {
//    EEPROM_WriteCounter();
//    _stream.print(AT_OK);
//  }
  else if (memcmp(sour,"AT+DEFAULT\r",11) == 0 || memcmp(sour,"at+default\r",11) == 0) {
    EEPROM_LoadDefault();
    _stream.print(AT_OK);
  }
  else if (memcmp(sour,"AT+RELOAD\r",10) == 0 || memcmp(sour,"at+reload\r",10) == 0) {
    EEPROM_ReadConfig();
    _stream.print(AT_OK);
  }
  else if (memcmp(sour,"AT+SETSTATE=",12) == 0 || memcmp(sour,"at+setstate=",12) == 0) {
    int temp;
    sour[--len] = NULL; //Bỏ qua ký tự <CR> cuối cùng
    temp = strtod(&sour[12], NULL);
    if (temp >= STATE_NORMAL && temp <= STATE_ALARM_LINK) 
      led_state = temp;
    _stream.printf("\r\n%s", AT_OK);
  }
  // else if (memcmp(sour,"AT+DATETIME=",12) == 0 || memcmp(sour,"at+datetime=",12) == 0) {
  //   if (len >= 32) { //Kể cả ký tự CR
  //     //_stream.printf("\r\n-String Lengh=%u", len);      
  //     sour[--len] = NULL; //Bỏ qua ký tự <CR> cuối cùng
  //     //_stream.printf("\r\n-String Lengh=%u", len);      
  //     time_t t_value = Str2TimeT(&sour[12]);
  //     RTC.set(t_value);   // set the RTC and the system time to the received value
  //     setTime(t_value);
  //     _stream.printf("\r\n-Current date time: %04d-%02d-%02d %02d:%02d:%02d", year(), month(), day(), hour(), minute(), second());
  //     _stream.printf(AT_OK);
  //   }
  // }
//  //Các lệnh cài đặt tham số:
//  else if (memcmp(sour,"AT+SERIAL=",10) == 0 || memcmp(sour,"at+serial=",10) == 0) {
//    uint64_t temp;
//    temp = strtoull(&sour[10], NULL, 16);
//    //printf("\r\nu32Temp=%X\r\n",u32Temp); //Debug
//    if (temp > 0) { //Serial hop le
//      //char sBuff[65];
//      //sprintf(sBuff,"%llu",temp);
//      //temp = strtoull(sBuff, NULL, 16);
//      if (m_cfg.serial != temp) {
//        m_cfg.serial = temp;
//      }
//      _stream.printf("\r\nSerial=%llX\r\n%s",m_cfg.serial,AT_OK);
//    }
//    else 
//      _stream.printf("\r\n%s",AT_ERROR);
//  }
//  else if (memcmp(sour,"AT+SSID=",8) == 0 || memcmp(sour,"at+ssid=",8) == 0) {
//    sour[--len] = NULL; //Bỏ qua ký tự <CR> cuối cùng
//    if (len-8 < 20) {
//      sour[len] = NULL;
//      strcpy(m_cfg.ssid, &sour[8]);
//      _stream.printf("\r\nData[%u]=%s", len-8, &sour[8]);
//      _stream.printf("\r\nSSID=%s",m_cfg.ssid);
//      _stream.printf("\r\n%s", AT_OK);
//    }
//    else {//Trường hợp tên dài hơn 20 ký tự
//      _stream.printf("\r\n%s",AT_ERROR);
//    }
//  }
//  else if (memcmp(sour,"AT+PASSWORD=",12) == 0 || memcmp(sour,"at+password=",12) == 0) {
//    sour[--len] = NULL; //Bỏ qua ký tự <CR> cuối cùng
//    if (len-12 < 20) {
//      strcpy(m_cfg.password, &sour[12]); 
//      _stream.printf("\r\nData[%u]=%s", len-12, &sour[12]);
//      _stream.printf("\r\nPassword=%s",m_cfg.password);
//      _stream.printf("\r\n%s", AT_OK);
//    }
//    else {//Trường hợp dài hơn 20 ký tự
//      _stream.printf("\r\n%s",AT_ERROR);
//    }
//  }
//  else if (memcmp(sour,"AT+STATICIP=",12) == 0 || memcmp(sour,"at+staticip=",12) == 0) {
//    sour[--len] = NULL; //Bỏ qua ký tự <CR> cuối cùng
//    m_cfg.static_ip.fromString(&sour[12]);   
//    _stream.printf("\r\nData[%u]=%s", len-12, &sour[12]);
//    _stream.printf("\r\nStatic IP=%u.%u.%u.%u", m_cfg.static_ip[0], m_cfg.static_ip[1], m_cfg.static_ip[2], m_cfg.static_ip[3]);
//    _stream.printf("\r\n%s", AT_OK);
//  }
//  else if (memcmp(sour,"AT+GATEWAY=",11) == 0 || memcmp(sour,"at+gateway=",11) == 0) {
//    sour[--len] = NULL; //Bỏ qua ký tự <CR> cuối cùng
//    m_cfg.static_gw.fromString(&sour[11]);   
//    _stream.printf("\r\nData[%u]=%s", len-11, &sour[11]);
//    _stream.printf("\r\nStatic gateway=%u.%u.%u.%u", m_cfg.static_gw[0], m_cfg.static_gw[1], m_cfg.static_gw[2], m_cfg.static_gw[3]);
//    _stream.printf("\r\n%s", AT_OK);
//  }
//  else if (memcmp(sour,"AT+SUBNET=",10) == 0 || memcmp(sour,"at+subnet=",10) == 0) {
//    sour[--len] = NULL; //Bỏ qua ký tự <CR> cuối cùng
//    m_cfg.static_sn.fromString(&sour[10]);   
//    _stream.printf("\r\nData[%u]=%s", len-10, &sour[10]);
//    _stream.printf("\r\nStatic subnet=%u.%u.%u.%u", m_cfg.static_sn[0], m_cfg.static_sn[1], m_cfg.static_sn[2], m_cfg.static_sn[3]);
//    _stream.printf("\r\n%s", AT_OK);
//  }
//  else if (memcmp(sour,"AT+SERVERIP=",12) == 0 || memcmp(sour,"at+serverip=",12) == 0) {
//    sour[--len] = NULL; //Bỏ qua ký tự <CR> cuối cùng
//    m_cfg.server_ip.fromString(&sour[12]);   
//    _stream.printf("\r\nData[%u]=%s", len-12, &sour[12]);
//    _stream.printf("\r\nServer IP=%u.%u.%u.%u", m_cfg.server_ip[0], m_cfg.server_ip[1], m_cfg.server_ip[2], m_cfg.server_ip[3]);
//    _stream.printf("\r\n%s", AT_OK);
//  }
//  else if (memcmp(sour,"AT+SERVERPORT=",14) == 0 || memcmp(sour,"at+serverport=",14) == 0) {
//    uint32_t temp;
//    sour[--len] = NULL; //Bỏ qua ký tự <CR> cuối cùng
//    temp = strtoul(&sour[14], NULL, 10);
//    if (temp < 65536) { //Serial hop le
//      m_cfg.server_port = temp;
//      _stream.printf("\r\nServer port=%u", m_cfg.server_port);
//      _stream.printf("\r\n%s", AT_OK);
//    }
//    else 
//      _stream.printf("\r\n%s",AT_ERROR);
//  }
//  else if (memcmp(sour,"AT+COUNTER1=",12) == 0 || memcmp(sour,"at+counter1=",12) == 0) {
//    uint32_t temp;
//    sour[--len] = NULL; //Bỏ qua ký tự <CR> cuối cùng
//    temp = strtoul(&sour[12], NULL, 10);
//    m_led.current_count = temp;
//    //eepr24Cxx.WriteMultiByte(m_cfg.address + 0, (byte*)&m_led.last_count, 8);
//    if (m_led.last_offset > temp) {
//      m_led.last_offset = temp;
//      //eepr24Cxx.WriteMultiByte(m_cfg.address + 24, (byte*)&m_led.last_offset, 8);
//    }
//    EEPROM_WriteCounter();
//    _stream.printf("\r\nLast Counter 1=%u", m_led.current_count);
//    _stream.printf("\r\n%s", AT_OK);
//  }
//  else if (memcmp(sour,"AT+COUNTER2=",12) == 0 || memcmp(sour,"at+counter2=",12) == 0) {
//    uint32_t temp;
//    sour[--len] = NULL; //Bỏ qua ký tự <CR> cuối cùng
//    temp = strtoul(&sour[12], NULL, 10);
//    m_led.current_count2 = temp;
//    //eepr24Cxx.WriteMultiByte(m_cfg.address + 8, (byte*)&m_led.last_count2, 8);
//    if (m_led.last_offset2 > temp) {
//      m_led.last_offset2 = temp;
//      //eepr24Cxx.WriteMultiByte(m_cfg.address + 32, (byte*)&m_led.last_offset2, 8);
//    }
//    EEPROM_WriteCounter();
//    _stream.printf("\r\nLast Counter 2=%u", m_led.current_count2);
//    _stream.printf("\r\n%s", AT_OK);
//  }
//  else if (memcmp(sour,"AT+COUNTER3=",12) == 0 || memcmp(sour,"at+counter3=",12) == 0) {
//    uint32_t temp;
//    sour[--len] = NULL; //Bỏ qua ký tự <CR> cuối cùng
//    temp = strtoul(&sour[12], NULL, 10);
//    m_led.current_count3 = temp;
//    //eepr24Cxx.WriteMultiByte(m_cfg.address + 16, (byte*)&m_led.last_count3, 8);
//    if (m_led.last_offset3 > temp) {
//      m_led.last_offset3 = temp;
//      //eepr24Cxx.WriteMultiByte(m_cfg.address + 40, (byte*)&m_led.last_offset3, 8);
//    }
//    EEPROM_WriteCounter();
//    _stream.printf("\r\nLast Counter 3=%u", m_led.current_count3);
//    _stream.printf("\r\n%s", AT_OK);
//  }
//  else if (memcmp(sour,"AT+LATCHCNT\r",12) == 0 || memcmp(sour,"at+latchcnt\r",12) == 0) {
//    m_cfg.last_latch = 0; //Yêu cầu tự reset ngày mới.
//    _stream.printf(AT_OK);
//  }
//  else if (memcmp(sour,"AT+STARTTIME=",13) == 0 || memcmp(sour,"at+starttime=",13) == 0) {
//    if (len >= 33) { //Kể cả ký tự CR
//      //_stream.printf("\r\n-String Lengh=%u", len);      
//      sour[--len] = NULL; //Bỏ qua ký tự <CR> cuối cùng
//      //_stream.printf("\r\n-String Lengh=%u", len);      
//      m_led.start_time = Str2TimeT(&sour[13]);
//      //_stream.printf("\r\n-Current date time: %04d-%02d-%02d %02d:%02d:%02d", year(), month(), day(), hour(), minute(), second());
//      _stream.printf(AT_OK);
//    }
//    else {
//      _stream.printf("\r\n-Current date time: %04d-%02d-%02d %02d:%02d:%02d", year(), month(), day(), hour(), minute(), second());
//      _stream.printf(AT_ERROR);
//    }
//  }
//  else if (memcmp(sour,"AT+TOTALMINUTE=",15) == 0 || memcmp(sour,"at+totalminute=",15) == 0) {
//    uint32_t temp;
//    sour[--len] = NULL; //Bỏ qua ký tự <CR> cuối cùng
//    temp = strtoul(&sour[15], NULL, 10);
//    m_led.total_minute = temp;
//    _stream.printf("\r\nTotal Minute=%u", m_led.total_minute);
//    _stream.printf("\r\n%s", AT_OK);
//  }
//  //Các đầu vào ra:
////  else if (memcmp(sour,"AT+OUTPUT1=",11) == 0 || memcmp(sour,"at+output1=",11) == 0) {
////    int value = strtoul(&sour[11],NULL,10);
////    if (value == 0) {
////      digitalWrite(OUTPUT1_PIN, LOW);
////      _stream.printf("\r\nOutput1=0\r\n");
////    }
////    else {
////      digitalWrite(OUTPUT1_PIN, HIGH);
////      _stream.printf("\r\nOutput1=1\r\n");
////    }
////    _stream.printf(AT_OK);
////  }
//  else if (memcmp(sour,"AT+READALL\r",11) == 0 || memcmp(sour,"at+readall\r",11) == 0) {
//    _stream.printf("\r\nInput1=%u", digitalRead(INPUT1_PIN));
//    _stream.printf("\r\nInput2=%u", digitalRead(INPUT2_PIN));
//    _stream.printf("\r\nInput3=%u", digitalRead(INPUT3_PIN));
//    //_stream.printf("\r\nOutput1=%u", digitalRead(OUTPUT1_PIN));
//    _stream.printf(AT_OK);
//  }
  
//    else {
//      _stream.printf("\r\n-Current date time: %04d-%02d-%02d %02d:%02d:%02d", year(), month(), day(), hour(), minute(), second());
//      _stream.printf(AT_ERROR);
//    }
//  }
//  //Các hàm test debug:
//  else if (memcmp(sour,"AT+GETNTPTIME\r",14) == 0 || memcmp(sour,"at+getntptime\r",14) == 0) {
//    if (WiFi.status() == WL_CONNECTED) {
//      _stream.printf("\r\nNTP Time=%u", getNtpTime());
//    }
//    else {
//      _stream.printf("\r\nWIFI is not connected!", getNtpTime());
//    }
//    _stream.printf(AT_OK);
//  }
  // else if (memcmp(sour,"AT+I2CSCAN\r",11) == 0 || memcmp(sour,"at+i2cscan\r",11) == 0) {
  //   I2CScan(_stream);
  //   _stream.printf(AT_OK);
  // }
  else return 0; //Neu khong co lenh nao duoc xu ly tra ve gia tri
  return 1; //Mac dinh la lenh AT da duoc xu ly
}
