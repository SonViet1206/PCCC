#include "Main.h"

/********************************************************
/*  Debug Print                                         *
/********************************************************/
void dbg_printf ( const char *format, ... ) {
    char sbuf[1400];                                                            // For debug lines
    va_list varArgs;                                                            // For variable number of params
    va_start ( varArgs, format );                                               // Prepare parameters
    vsnprintf ( &sbuf[10], sizeof ( sbuf ) - 10, format, varArgs );                       // Format the message
    va_end ( varArgs );                                                         // End of using parameters
    strcpy(sbuf, "\r\n*[DEBUG]"); 
    Serial.print ( sbuf );
}

void I2CScan(Stream &_stream)
{
  byte error, address;
  int nDevices;
 
  _stream.println("\r\n***I2C Scanning (1-127)...");
 
  nDevices = 0;
  for(address = 1; address < 128; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)
    {
      _stream.print("I2C device found at address 0x");
      if (address<16) _stream.print("0");
      _stream.print(address,HEX);
      _stream.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
      _stream.print("Unknown error at address 0x");
      if (address<16) _stream.print("0");
      _stream.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    _stream.println("No I2C devices found\r\n");
  else
    _stream.println("done\r\n");
}

char* SH2S(char* dest, char* sour, uint16_t size, int8_t add_byte) {
  uint16_t len, i=0;
  char buff[3], *str;

  if (size) len = size;
  else len = strlen(sour);
  
  if (len > 0) {
    if (len % 2 != 0) { //So ky tu HEXA le
      buff[0] = '0';
      buff[1] = sour[0];
      str = &sour[1];
    }
    else {
      buff[0] = sour[0];
      buff[1] = sour[1];
      str = &sour[2];
    }
    buff[2] = NULL;
    dest[0] = (char) (strtol(buff, NULL, 16) + add_byte); 
    dest[1] = NULL; //Khoa chuoi

    len /= 2;
    if (len > 0) {
      for (i=0;i<len;i++) {
        buff[0] = str[i*2];
        buff[1] = str[i*2+1];
        buff[2] = NULL;
        dest[i+1] = (char) (strtol(buff, NULL, 16) + add_byte); 
      }
      dest[i+1] = NULL; //Khoa chuoi
    }
    
    return dest;
  }
  return NULL; //
}

char* S2SH(char *dest, const char *sour, uint16_t size, int8_t add_byte, char space) {
  char hexArray[] = "0123456789ABCDEF";
  uint16_t len, i=0;
  uint8_t temp;
  
  if (size) len = size;
  else len = strlen(sour);
  
  if (len > 0) {
    for (i = 0; i < len; i++ ) {
      temp = sour[i] + add_byte; 
      if (space) {
        dest[i * 3] = hexArray[temp >> 4];
        dest[i * 3 + 1] = hexArray[temp & 0x0F];
        dest[i * 3 + 2] = space;
      }
      else {
        dest[i * 2] = hexArray[temp >> 4];
        dest[i * 2 + 1] = hexArray[temp & 0x0F];
      }
    }
    if (space) 
      dest[i * 3-1] = NULL;
    else
      dest[i * 2] = NULL;
    return dest; //
  }
  return NULL; //
}

uint16_t  S2U16(char *sour, uint8_t swap, int8_t add) {
  uint16_t retValue = 0;
  if (swap) {
    retValue = sour[1] + add;
    retValue = (retValue<<8) | (sour[0] + add);
  }
  else {
    retValue = sour[0] + add;
    retValue = (retValue<<8) + (sour[1] + add);
  }
  return retValue;
}

uint64_t S2U64(char *sour, uint8_t swap, int8_t add) {
  uint64_t retValue = 0;
  if (swap) {
    retValue = sour[7] + add;
    retValue = (retValue<<8) | (sour[6] + add);
    retValue = (retValue<<8) | (sour[5] + add);
    retValue = (retValue<<8) | (sour[4] + add);
    retValue = (retValue<<8) | (sour[3] + add);
    retValue = (retValue<<8) | (sour[2] + add);
    retValue = (retValue<<8) | (sour[1] + add);
    retValue = (retValue<<8) | (sour[0] + add);
  }
  else {
    retValue = sour[0] + add;
    retValue = (retValue<<8) | (sour[1] + add);
    retValue = (retValue<<8) | (sour[2] + add);
    retValue = (retValue<<8) | (sour[3] + add);
    retValue = (retValue<<8) | (sour[4] + add);
    retValue = (retValue<<8) | (sour[5] + add);
    retValue = (retValue<<8) | (sour[6] + add);
    retValue = (retValue<<8) | (sour[7] + add);
  }
  return retValue;
}

uint64_t S2U48(char *sour, uint8_t swap, int8_t add) {
  uint64_t retValue = 0;
  if (swap) {
    retValue = sour[5] + add;
    retValue = (retValue<<8) | (sour[4] + add);
    retValue = (retValue<<8) | (sour[3] + add);
    retValue = (retValue<<8) | (sour[2] + add);
    retValue = (retValue<<8) | (sour[1] + add);
    retValue = (retValue<<8) | (sour[0] + add);
  }
  else {
    retValue = sour[0] + add;
    retValue = (retValue<<8) | (sour[1] + add);
    retValue = (retValue<<8) | (sour[2] + add);
    retValue = (retValue<<8) | (sour[3] + add);
    retValue = (retValue<<8) | (sour[4] + add);
    retValue = (retValue<<8) | (sour[5] + add);
  }
  return retValue;
}

uint64_t S2U40(char *sour, uint8_t swap, int8_t add) {
  uint64_t retValue = 0;
  if (swap) {
    retValue = sour[4] + add;
    retValue = (retValue<<8) | (sour[3] + add);
    retValue = (retValue<<8) | (sour[2] + add);
    retValue = (retValue<<8) | (sour[1] + add);
    retValue = (retValue<<8) | (sour[0] + add);
  }
  else {
    retValue = sour[0] + add;
    retValue = (retValue<<8) | (sour[1] + add);
    retValue = (retValue<<8) | (sour[2] + add);
    retValue = (retValue<<8) | (sour[3] + add);
    retValue = (retValue<<8) | (sour[4] + add);
  }
  return retValue;
}

uint32_t S2U24(char *sour, uint8_t swap, int8_t add) {
  uint32_t retValue = 0;
  if (swap) {
    retValue = (retValue<<8) | (sour[2] + add);
    retValue = (retValue<<8) | (sour[1] + add);
    retValue = (retValue<<8) | (sour[0] + add);
  }
  else {
    retValue = (retValue<<8) | (sour[0] + add);
    retValue = (retValue<<8) | (sour[1] + add);
    retValue = (retValue<<8) | (sour[2] + add);
  }
  return retValue;
}

uint32_t S2U32(char *sour, uint8_t swap, int8_t add) {
  uint32_t retValue = 0;
  if (swap) {
    retValue = sour[3] + add;
    retValue = (retValue<<8) | (sour[2] + add);
    retValue = (retValue<<8) | (sour[1] + add);
    retValue = (retValue<<8) | (sour[0] + add);
  }
  else {
    retValue = sour[0] + add;
    retValue = (retValue<<8) | (sour[1] + add);
    retValue = (retValue<<8) | (sour[2] + add);
    retValue = (retValue<<8) | (sour[3] + add);
  }
  return retValue;
}

//
char*  U2S16(char *dest, uint16_t value, uint8_t swap, int8_t add) {
  if (swap) {
    dest[1] = ((value >> 8) + add) & 0xFF; 
    dest[0] = (value + add) & 0xFF; 
  }
  else {
    dest[0] = ((value >> 8) + add) & 0xFF; 
    dest[1] = (value + add) & 0xFF; 
  }
  //sprintf(Cmd_Buffer,"Data=[%0X][%0X][%0X][%d]\r\n",value,dest[0],dest[1],add);  UartPutS(Cmd_Buffer);
  return dest;
}

char*  U2S24(char *dest, uint32_t value, uint8_t swap, int8_t add) {
  if (swap) {
    dest[2] = ((value >> 16) + add) & 0xFF; 
    dest[1] = ((value >> 8) + add) & 0xFF; 
    dest[0] = (value + add) & 0xFF; 
  }
  else {
    dest[0] = ((value >> 16) + add) & 0xFF; 
    dest[1] = ((value >> 8) + add) & 0xFF; 
    dest[2] = (value + add) & 0xFF; 
  }
  return dest;
}

char*  U2S32(char *dest, uint32_t value, uint8_t swap, int8_t add) {
  if (swap) {
    dest[3] = ((value >> 24) + add) & 0xFF; 
    dest[2] = ((value >> 16) + add) & 0xFF; 
    dest[1] = ((value >> 8) + add) & 0xFF; 
    dest[0] = (value + add) & 0xFF; 
  }
  else {
    dest[0] = ((value >> 24) + add) & 0xFF; 
    dest[1] = ((value >> 16) + add) & 0xFF; 
    dest[2] = ((value >> 8) + add) & 0xFF; 
    dest[3] = (value + add) & 0xFF; 
  }
  return dest;
}

char*  U2S64(char *dest, uint64_t value, uint8_t swap, int8_t add) {
  if (swap) {
    dest[7] = ((value >> 56) + add) & 0xFF; 
    dest[6] = ((value >> 48) + add) & 0xFF; 
    dest[5] = ((value >> 40) + add) & 0xFF; 
    dest[4] = ((value >> 32) + add) & 0xFF; 
    dest[3] = ((value >> 24) + add) & 0xFF; 
    dest[2] = ((value >> 16) + add) & 0xFF; 
    dest[1] = ((value >> 8) + add) & 0xFF; 
    dest[0] = (value + add) & 0xFF; 
  }
  else {
    dest[0] = ((value >> 56) + add) & 0xFF; 
    dest[1] = ((value >> 48) + add) & 0xFF; 
    dest[2] = ((value >> 40) + add) & 0xFF; 
    dest[3] = ((value >> 32) + add) & 0xFF; 
    dest[4] = ((value >> 24) + add) & 0xFF; 
    dest[5] = ((value >> 16) + add) & 0xFF; 
    dest[6] = ((value >> 8) + add) & 0xFF; 
    dest[7] = (value + add) & 0xFF; 
  }
  return dest;
}

char*  U2S40(char *dest, uint64_t value, uint8_t swap, int8_t add) {
  if (swap) {
    dest[4] = ((value >> 32) + add) & 0xFF; 
    dest[3] = ((value >> 24) + add) & 0xFF; 
    dest[2] = ((value >> 16) + add) & 0xFF; 
    dest[1] = ((value >> 8) + add) & 0xFF; 
    dest[0] = (value + add) & 0xFF; 
  }
  else {
    dest[0] = ((value >> 32) + add) & 0xFF; 
    dest[1] = ((value >> 24) + add) & 0xFF; 
    dest[2] = ((value >> 16) + add) & 0xFF; 
    dest[3] = ((value >> 8) + add) & 0xFF; 
    dest[4] = (value + add) & 0xFF; 
  }
  return dest;
}

char*  U2S48(char *dest, uint64_t value, uint8_t swap, int8_t add) {
  if (swap) {
    dest[5] = ((value >> 40) + add) & 0xFF; 
    dest[4] = ((value >> 32) + add) & 0xFF; 
    dest[3] = ((value >> 24) + add) & 0xFF; 
    dest[2] = ((value >> 16) + add) & 0xFF; 
    dest[1] = ((value >> 8) + add) & 0xFF; 
    dest[0] = (value + add) & 0xFF; 
  }
  else {
    dest[0] = ((value >> 40) + add) & 0xFF; 
    dest[1] = ((value >> 32) + add) & 0xFF; 
    dest[2] = ((value >> 24) + add) & 0xFF; 
    dest[3] = ((value >> 16) + add) & 0xFF; 
    dest[4] = ((value >> 8) + add) & 0xFF; 
    dest[5] = (value + add) & 0xFF; 
  }
  return dest;
}

uint32_t SH2U32(char *sour, char add_byte) {
  uint32_t retValue = 0;
  uint16_t len, i;
  char sBuff[] = {'0','0','0','0','0','0','0','0',NULL}; //8+1 byte
  //SOURCE = 00FF1112 - 8 ky tu HEXA
  len = strlen(sour);
  if (len > 0) {
    for (i=0;i<8;i++) {
      if (len-i-1 >= 0) sBuff[7-i] =  sour[len-i-1] + add_byte;
    }
    retValue = strtoul(sBuff, NULL, 16);
  }
  return retValue; 
}

/* bin to 8421BCD */
uint8_t BIN2BCD8(uint8_t bin) { return (bin%10) + ((bin/10)%10)*0x10; }
uint16_t BIN2BCD16(uint16_t bin) { 
  uint16_t retVal;
  retVal = (bin%10);
  retVal += ((bin/10)%10)*0x10;
  retVal += ((bin/100)%10)*0x100;
  retVal += ((bin/1000)%10)*0x1000;
  return retVal;
}
uint32_t BIN2BCD24(uint32_t bin) { 
  uint32_t retVal = 0;
  retVal = (bin%10);
  retVal += ((bin/10)%10)*0x10;
  retVal += ((bin/100)%10)*0x100;
  retVal += ((bin/1000)%10)*0x1000;
  retVal += ((bin/10000)%10)*0x10000;
  retVal += ((bin/100000)%10)*0x100000;
  return retVal;
}
uint32_t BIN2BCD32(uint32_t bin) { 
  uint32_t retVal = 0;
  retVal = (bin%10);
  retVal += ((bin/10)%10)*0x10;
  retVal += ((bin/100)%10)*0x100;
  retVal += ((bin/1000)%10)*0x1000;
  retVal += ((bin/10000)%10)*0x10000;
  retVal += ((bin/100000)%10)*0x100000;
  retVal += ((bin/1000000)%10)*0x1000000;
  retVal += ((bin/10000000)%10)*0x10000000;
  return retVal;
}
uint64_t BIN2BCD64(uint64_t bin) { 
  uint8_t i, shift = 0;
  uint64_t retVal = 0, factor = 1;
  for (i=0;i<16;i++) {
    //retVal += ((bin/10)%10)*0x10;
    //retVal += ((bcd>>shift) & 0xF)*factor;
    shift += 4;
    factor *= 10;
  }
  return retVal;
} 
/* 8421BCD to bin*/
uint8_t BCD2BIN8(uint8_t bcd) { return (bcd & 0xF) + ((bcd>>4) & 0xF)*10; } 
uint16_t BCD2BIN16(uint16_t bcd) { 
  uint16_t retVal;
  
  retVal = BCD2BIN8((bcd>>8) & 0xFF); retVal <<= 8;
  retVal += BCD2BIN8(bcd & 0xFF);
  
  return retVal;
  //return (bcd & 0xF) + ((bcd>>4) & 0xF)*10 + ((bcd>>8) & 0xF)*100 + ((bcd>>12) & 0xF)*1000; 
} 

uint32_t BCD2BIN24(uint32_t bcd) { 
  uint32_t retVal;
  
  retVal = BCD2BIN8((bcd>>16) & 0xFF); retVal <<= 8;
  retVal += BCD2BIN8((bcd>>8) & 0xFF); retVal <<= 8;
  retVal += BCD2BIN8(bcd & 0xFF);
  
  //retVal = (bcd & 0xF);
  //retVal += ((bcd>>4) & 0xF)*10;
  //retVal += ((bcd>>8) & 0xF)*100;
  //retVal += ((bcd>>12) & 0xF)*1000;
  //retVal += ((bcd>>16) & 0xF)*10000;
  //retVal += ((bcd>>20) & 0xF)*100000; 
  return retVal;
} 
uint32_t BCD2BIN32(uint32_t bcd) { 
  uint32_t retVal;
  
  retVal = BCD2BIN16((bcd>>16) & 0xFFFF); retVal <<= 16;
  retVal += BCD2BIN16(bcd & 0xFFFF);
  
  //retVal = (bcd & 0xF);
  //retVal += ((bcd>>4) & 0xF)*10;
  //retVal += ((bcd>>8) & 0xF)*100;
  //retVal += ((bcd>>12) & 0xF)*1000;
  //retVal += ((bcd>>16) & 0xF)*10000;
  //retVal += ((bcd>>20) & 0xF)*100000; 
  //retVal += ((bcd>>24) & 0xF)*1000000;
  //retVal += ((bcd>>28) & 0xF)*10000000;
  return retVal;
} 
uint64_t BCD2BIN64(uint64_t bcd) { 
  //uint8_t i, shift = 0;
  //uint64_t factor = 1;
  uint64_t retVal;
  retVal = BCD2BIN32((bcd>>32) & 0xFFFFFFFF); retVal <<= 32;
  retVal += BCD2BIN32(bcd & 0xFFFFFFFF);
  
  //for (i=0;i<16;i++) {
  //  retVal += ((bcd>>shift) & 0xF)*factor;
  //  shift += 4;
  //  factor *= 10;
  //}
  return retVal;
} 

//Tinh toan byte FCS ADD va dat vao vi tri cuoi chuoi
void FCSADD_Add(char* data, uint16_t len) {
  uint16_t i;
  uint8_t FCS = 0;
  if (len > 0) {
    for (i = 0; i < len-1; i++) FCS += data[i]; 
    data[len-1]  = FCS;
  }
}

//Kiem tra byte FCS ADD mac dinh la byt cuoi cung cua chuoi
uint8_t FCSADD_Check(char* data, uint16_t len) {
  uint16_t i;
  uint8_t FCS = 0;
  //Serial.printf("\r\n***FCSADD_Check");
  for (i = 0; i < len-1; i++) FCS  += data[i];  
  if (FCS == data[len-1]) return 1;
  printf("\r\nADD FSC Error: %02X/%02X\r\n", FCS, data[len-1]); //Debug
  return 0;
}

//Tinh toan byte FCS XOR va dat vao vi tri cuoi chuoi
void FCSXOR_Add(char* data, uint16_t len) {
  uint16_t i;
  uint8_t FCS = 0;
  if (len > 0) {
    for (i = 0; i < len-1; i++) FCS ^= data[i]; 
    data[len-1]  = FCS;
  }
}

//Kiem tra byte FCS XOR mac dinh la byt cuoi cung cua chuoi
uint8_t FCSXOR_Check(char* data, uint16_t len) {
  uint16_t i;
  uint8_t FCS = 0;
  //char sBuff[RF_PACKET_LENGTH*3];
  for (i = 0; i < len-1; i++) FCS  ^= data[i];
  //printf("Data=[%s] FCS=[%X][%X]\r\n", S2SH(sBuff, data, len, 0, NULL),FCS,data[len-1]);
  if (FCS == data[len-1]) return 1;
  return 0;
}

/*
//                                                       16   12   5
// this is the CCITT CRC 16 polynomial X  + X  + X  + 1.
// This works out to be 0x1021, but the way the algorithm works
// lets us use 0x8408 (the reverse of the bit pattern).  The high
// bit is always assumed to be set, thus we only use 16 bits to
// represent the 17 bit value.
*/
uint16_t LG_CRC16(char *sour, uint8_t len)
{
  uint8_t i;
  uint16_t data;
  uint16_t crc = 0xffff;
  uint16_t u8len = len;

      do
      {
            for (i=0, data=(uint16_t)0xff & *sour++; i < 8; i++, data >>= 1){
                  if ((crc & 0x0001) ^ (data & 0x0001)) crc = (crc >> 1) ^ 0x8408;
                  else  crc >>= 1;
            }
      } while (--u8len);

      crc = ~crc;
      data = crc;
      crc = (crc << 8) | (data >> 8 & 0xff);

      return crc;
}

//Tinh toan CRC va dat vao vi tri cuoi cung cua chuoi:
void LG_CRC16_Add(char *sour, uint8_t len)
{
  uint16_t crc;
  if (len > 2) {
    crc = LG_CRC16(sour, len -2);
    sour[len-2] = (char)((crc >> 8) & 0xFF);
    sour[len-1] = (char)((crc) & 0xFF);
  }
}

//Kiem tra CRC dat o vi tri cuoi cung cua chuoi:
uint8_t LG_CRC16_Check(char *sour, uint8_t len)
{
  uint16_t crc;
  if (len > 2) {
    crc = LG_CRC16(sour, len -2);
    if (sour[len-2] == (char)((crc >> 8) & 0xFF) && sour[len-1] == (char)((crc) & 0xFF)) return 1;
  }
  return 0;
}

uint16_t GENIUS_CRC16(char *sour, uint8_t len)
{
  uint16_t i, j, k, num, crc;
  crc = 0;
  for (i = 0; i < len; i++) {
    num = (crc >> 8) ^ sour[i];
    k = num << 8;
    num = 0;
    for (j = 0; j <= 7; j++) {
      if ((num ^ k) & 0x8000) {
        num = (num << 1) ^ 0x1021;
      } else {
        num <<= 1;
      }
      k <<= 1;
    }
    crc = (crc << 8) ^ num;
  }
  return crc;
}

//--------------------------------------------------------------
uint16_t CCITT_CRC16(char *data, int len) {
  uint16_t crc = 0xFFFF; //CRC Init
  uint8_t i,j;
  for(i = 0; i < len; i++) {
    crc ^= (uint16_t)data[i] << 8;
    for (j = 0; j < 8; j++) {
      if ((crc & 0x8000) > 0)
        crc = (crc << 1) ^ 0x8005; //CRC16_POLY 
      else
        crc <<= 1;
    }
    /*        
    uint8_t temp;
    temp = data[i];
    for (j = 0; j < 8; j++) { 
      if (((crc & 0x8000) >> 8) ^ (temp & 0x80)) 
          crc = (crc << 1) ^ 0x8005; //CRC16_POLY 
      else 
          crc = (crcReg << 1); 
      temp <<= 1; 
    } 
    */
  }
  return crc; 
}

void CCITT_CRC16_Add(char *data, uint8_t len) {
  uint16_t crc;
  PrintDebug((int)1, data, (int)len);
  if (len > 2) {
    crc = CCITT_CRC16(data, len-2);
    data[len-2] = (char)((crc >> 8) & 0xFF); //High Byte
    data[len-1] = (char)(crc & 0xFF); //Low Byte
    Serial.printf("\r\n***CRC=[%04X][%u|%u", m_cfg.crc_check, data[len-2], data[len-1]);
    PrintDebug((int)2, data, (int)len);
  }
}

uint8_t CCITT_CRC16_Check(char *data, uint8_t len) {
  uint8_t retOK = 0;
  uint16_t crc;
  //char sBuff[64];
  if (len > 2) {
    crc = CCITT_CRC16(data, len-2);
    if (data[len-2] == ((crc >> 8) & 0xFF) && data[len-1] == (crc & 0xFF)) retOK = 1;
  }
  //sprintf(sBuff,"CRC Length=%u, Return=%u\r\n",len,retOK); UartPutS(sBuff); //Debug
  return retOK;
}

void CC110X_Whitening(char *data, uint8_t len)
{
  uint16_t PN9, temp1,temp2;
  uint8_t byte_Xor, i, j;
  //char sBuff[64];
  PN9 = 0xFFFF;
  for(i=0; i<len; i++)
  {
    byte_Xor = (uint8_t)(PN9 & 0x00ff);
    //sprintf(Cmd_Buffer,"\r\nByte Xor[%d] = [%.2X]\r\n",i,byte_Xor); UartPutS(Cmd_Buffer);
    data[i] ^= byte_Xor;
    //sprintf(Cmd_Buffer,"\r\nData Xor[%d] = [%.2X]\r\n",i,data[i]); UartPutS(Cmd_Buffer);
    for(j=0; j<8; j++)
    {
      temp1 = (PN9 >> 5) & 0x0001;
      temp2 = PN9 & 0x0001;
      //bit 0 Xor bit 5:
      if(temp1 ^ temp2) PN9 |= 0x0200; //bit 8 = bit 0 Xor bit 5
      else PN9 &= 0xfdff;
      PN9 = PN9 >> 1;
    }
  }
  //sprintf(sBuff,"Whitening Length=%u -> OK!\r\n",len); UartPutS(sBuff); //Debug
}

time_t Str2TimeT(const char* str_dt)
{
  //char *p = str_dt;
  struct tm timeinfo;
  timeinfo.tm_mon  = atoi(&str_dt[5]) - 1;
  timeinfo.tm_mday = atoi(&str_dt[8]);
  timeinfo.tm_year = atoi(&str_dt[2]) + 100;
  timeinfo.tm_hour = atoi(&str_dt[11]);
  timeinfo.tm_min  = atoi(&str_dt[14]);
  timeinfo.tm_sec  = atoi(&str_dt[17]);
  return (time_t)mktime(&timeinfo);  
}

uint32_t Str2Time(const char* str_time)
{
  uint32_t retValue;
  retValue = atoi(&str_time[0]) * 3600;
  retValue += atoi(&str_time[3]) * 60;
  retValue += atoi(&str_time[6]);
  return retValue;
}

char* TimeT2Str(char *s_buffer, time_t value)
{
  strftime (s_buffer, 20,"%Y-%m-%d %H:%M:%S\0", localtime(&value));
  return s_buffer;
}

char* TimeT2StrHMS(char *s_buffer, time_t value)
{
  strftime (s_buffer, 20,"%H:%M:%S\0", localtime(&value));
  return s_buffer;
}

char* IP2Str(char *s_buffer, IPAddress value)
{
  sprintf(s_buffer, "%u.%u.%u.%u", value[0], value[1], value[2], value[3]);
  return s_buffer;
}

//char* U2SH64(char *s_buffer, uint64_t value)
//{
//  sprintf(s_buffer, "%llX", value);
//  return s_buffer;
//}
//
//char* U2S64(char *s_buffer, uint64_t value)
//{
//  sprintf(s_buffer, "%llu", value);
//  return s_buffer;
//}
//
//char* U2S32(char *s_buffer, uint32_t value)
//{
//  sprintf(s_buffer, "%u", value);
//  return s_buffer;
//}

void PrintDebug(int id, char *data, int len) {
  char sBuff[1024]; //[DEBUG000][11240000B0E90091600021000000000000][017]
  int size = len;
  if (size > 256) size = 256;
  sprintf(sBuff,"\r\n[DEBUG%03u][", id);
  S2SH(&sBuff[13],data,size,0,NULL); //For debug toi da 62 byte
  sprintf(&sBuff[13+size*2],"][%u]\r\n", size);
  Serial.print(sBuff); 
}

//uint16_t CCITT_CRC16(unsigned char *data, int len) {
//  uint16_t crc = 0xFFFF; //CRC Init
//  uint8_t i,j;
//  for(i = 0; i < len; i++) {
//    crc ^= (uint16_t)data[i] << 8;
//    for (j = 0; j < 8; j++) {
//      if ((crc & 0x8000) > 0)
//        crc = (crc << 1) ^ 0x8005; //CRC16_POLY 
//      else
//        crc <<= 1;
//    }
//  }
//  //return ((crc >> 8) & 0xFF) | ((crc << 8) & 0xFF00); //Đảo byte cao thấp
//  return crc;
//}
//
//uint16_t CCITT_CRC16_Add(uint8_t *data, int len) {
//  uint16_t crc = 0;
//  if (len > 4) {
//    crc = CCITT_CRC16(data, len-4);
//    data[len-3] = (char)((crc >> 8) & 0xFF); //High Byte
//    data[len-4] = (char)(crc & 0xFF); //Low Byte
//  }
//  return crc;
//}
//
//uint16_t CCITT_CRC16_Check(uint8_t *data, int len) {
//  uint16_t crc = 0;
//  if (len > 4) {
//    crc = CCITT_CRC16(data, len-4);
//    //Serial.printf("\r\nCRC Mem=%02X %02X", data[len-3], data[len-4]);
//    //Serial.printf("\r\nCRC Chk=%02X %02X", crc & 0xFF, (crc >> 8) & 0xFF);
//    if ((data[len-3] == ((crc >> 8) & 0xFF)) && (data[len-4] == (crc & 0xFF))) 
//      return crc;
//  }
//  //sprintf(sBuff,"CRC Length=%u, Return=%u\r\n",len,retOK); UartPutS(sBuff); //Debug
//  return 0;
//}

void EEPROM_PrintConfig(Stream &_stream) {
  char s_buff[32];
  _stream.printf("\r\n***EEPROM CONFIG INFORMATION:");
  _stream.printf("\r\n-Built: %s %s", __DATE__, __TIME__);
  _stream.printf("\r\n-Serial: %08X",m_cfg.serial); //Số chế tạo lưu dạng HEXA TEXT
  
  _stream.printf("\r\n[RF LORA]");
  _stream.printf("\r\n-Net ID: %02X", m_cfg.net_id);
  _stream.printf("\r\n-Next Timeout: %u", m_cfg.next_timeout);
  _stream.printf("\r\n-Next Execute: %u", m_cfg.next_execute);
  _stream.printf("\r\n-Next Random:  %u", m_cfg.next_random);
  _stream.printf("\r\n-Max Retry:    %u", m_cfg.max_retry);
  
  _stream.printf("\r\n[WIFI]");
  _stream.printf("\r\n-SSID:         %s", m_cfg.ssid);
  _stream.printf("\r\n-Password:     %s",m_cfg.password);
  _stream.printf("\r\n-Static IP:    %u.%u.%u.%u", m_cfg.static_ip[0], m_cfg.static_ip[1], m_cfg.static_ip[2], m_cfg.static_ip[3]); 
  _stream.printf("\r\n-Subnet Mask:  %u.%u.%u.%u", m_cfg.static_sn[0], m_cfg.static_sn[1], m_cfg.static_sn[2], m_cfg.static_sn[3]); 
  _stream.printf("\r\n-Gateway:      %u.%u.%u.%u", m_cfg.static_gw[0], m_cfg.static_gw[1], m_cfg.static_gw[2], m_cfg.static_gw[3]); 
  _stream.printf("\r\n-Server IP:    %u.%u.%u.%u", m_cfg.server_ip[0], m_cfg.server_ip[1], m_cfg.server_ip[2], m_cfg.server_ip[3]); 
  _stream.printf("\r\n-Server Port:  %u", m_cfg.server_port);
  
  _stream.printf("\r\n[COMMON INFORMATION]");
  _stream.printf("\r\n-CRC Read=[%04X]", m_cfg.crc_check);
  //_stream.printf("\r\n[%04d-%02d-%02d %02d:%02d:%02d]\r\n%s", year(), month(), day(), hour(), minute(), second(), AT_OK);
  _stream.printf("\r\n-Last Reset Counter: %s", TimeT2Str(s_buff, m_cfg.last_latch));
  _stream.printf("\r\n-EEPROM Start Address: %u", m_cfg.address);
  _stream.printf("\r\n-EEPROM Loop Count: %u", m_cfg.loop_count);


  //_stream.printf("\r\n-Kp: %0.2f", (float)m_cfg.k_p);
  //_stream.printf("\r\n-Ki: %0.2f", (float)m_cfg.k_i);
  //_stream.printf("\r\n-Kd: %0.2f", (float)m_cfg.k_d);
  
  
  _stream.printf("\r\n");
}

void EEPROM_LoadDefault(void) {
  char sBuff[64];
  int MM, i, dd, yy, HH, mm, ss;
  //BUILT_DATE_TIME: 
  //0   4  7    12 15 18 
  //Nov  9 2015 14:17:38
  //Nov 11 2015 11:15:52
  yy = atoi(&BUILT_DATE_TIME[9]);
  for (i = 0; i < 12; i++) if (memcmp(BUILT_DATE_TIME, MONTHS[i],3) == 0) MM = i + 1;
  dd = atoi(&BUILT_DATE_TIME[4]);
  HH = atoi(&BUILT_DATE_TIME[12]);
  mm = atoi(&BUILT_DATE_TIME[15]);
  ss = atoi(&BUILT_DATE_TIME[18]);
      
  Serial.printf("\r\n***Eeprom configuration load default!");
  sprintf(&sBuff[0], "%02d", yy);
  sprintf(&sBuff[2], "%02d", MM);
  sprintf(&sBuff[4], "%04X", (dd * 24 * 60 + mm)); //Khác nhau sau mỗi 1 phút
  //m_cfg.serial = SH2U32(sBuff, 0);
  m_cfg.serial = (uint32_t)(((uint64_t)ESP.getEfuseMac()) >> 16); 
  m_cfg.net_id = 0;
  
  m_cfg.next_timeout = 100;
  m_cfg.next_execute = 300;
  m_cfg.next_random = 20;
  m_cfg.max_retry = 2;
    
  strcpy(m_cfg.ssid, "OPEN_WIFI\0"); //String        _ssid                   = "";
  m_cfg.password[0] = NULL; //String        _pass                   = "";
  m_cfg.static_ip.fromString("0.0.0.0"); //Địa chỉ IP tĩnh
  m_cfg.static_gw.fromString("8.8.8.8"); //Địa chỉ Gateway
  m_cfg.static_sn = IPAddress(255,255,255,255); //Sub Net Mask
  m_cfg.server_ip.fromString("192.168.50.1"); //
  m_cfg.server_port = 8081; //
  m_cfg.last_latch = 0; //

  m_cfg.crc_check = 0;
  
  Serial.printf("\r\n***Default Config Load! Reset Hardware for new config...\r\n");
}

void EEPROM_ReadConfig(void) {
  int block_size = sizeof(m_cfg);
  
  eeprom.ReadMultiByte(0, (byte*)&m_cfg, block_size);
  //Thong tin cau hinh
  EEPROM_PrintConfig(Serial);
  
  //int crc_ret = CCITT_CRC16((uint8_t*)&m_cfg, block_size - 4);
  //int crc_ret = CCITT_CRC16_Check((uint8_t*)&m_cfg, block_size);
  //Serial.printf("\r\n-CRC Check=[%04X]:%s", crc_ret, (crc_ret != 0) ? "OK" : "ERROR");
  
  if ( CCITT_CRC16_Check((char*)&m_cfg, block_size) != 0
    && m_cfg.next_timeout != 0 && m_cfg.next_execute != 0 && m_cfg.next_random != 0 && m_cfg.max_retry != 0 ) {
    Serial.printf("\r\nEeprom configuration load OK!\r\n");
  }
  else {
    Serial.printf("\r\nEeprom configuration failed, default loaded!\r\n");  
    EEPROM_LoadDefault();
    EEPROM_WriteConfig();
  }
}

void EEPROM_WriteConfig(void) {
  int block_size = sizeof(m_cfg);
  CCITT_CRC16_Add((char*)&m_cfg, block_size);
  eeprom.WriteMultiByte(0, (byte*)&m_cfg, block_size);
  Serial.printf("\r\n***Eeprom configuration saved! CRC=[%04X]", m_cfg.crc_check);
}
