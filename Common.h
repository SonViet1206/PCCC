#ifndef __COMMON_H
#define __COMMON_H

/*
*************************************************************************************************************************************
* Cac ham xu ly du lieu dung chung duoc khai bao trong file "Commond.c"
*************************************************************************************************************************************
*/
void FillBufferValue(uint8_t *dest, uint16_t len, uint8_t value);
char*  SH2S(char *dest, char *sour, uint16_t size, int8_t add_byte);
char*  S2SH(char *dest, const char *sour, uint16_t size, int8_t add_byte, char space);

char*  U2S16(char *dest, uint16_t value, uint8_t swap, int8_t add);
char*  U2S24(char *dest, uint32_t value, uint8_t swap, int8_t add);
char*  U2S32(char *dest, uint32_t value, uint8_t swap, int8_t add);
char*  U2S40(char *dest, uint64_t value, uint8_t swap, int8_t add);
char*  U2S48(char *dest, uint64_t value, uint8_t swap, int8_t add);
char*  U2S64(char *dest, uint64_t value, uint8_t swap, int8_t add);
uint16_t  S2U16(char *sour, uint8_t swap, int8_t add);
uint32_t  S2U24(char *sour, uint8_t swap, int8_t add);
uint32_t  S2U32(char *sour, uint8_t swap, int8_t add);
uint64_t  S2U40(char *sour, uint8_t swap, int8_t add);
uint64_t  S2U48(char *sour, uint8_t swap, int8_t add);
uint64_t  S2U64(char *sour, uint8_t swap, int8_t add);

uint32_t SH2U32(char *sour, char add_byte);

uint8_t BIN2BCD8(uint8_t bin);
uint16_t BIN2BCD16(uint16_t bin);
uint32_t BIN2BCD24(uint32_t bin);
uint32_t BIN2BCD32(uint32_t bin);
uint64_t BIN2BCD64(uint64_t bin);
uint8_t BCD2BIN8(uint8_t bcd);
uint16_t BCD2BIN16(uint16_t bcd);
uint32_t BCD2BIN24(uint32_t bcd);
uint32_t BCD2BIN32(uint32_t bcd);
uint64_t BCD2BIN64(uint64_t bcd);

void FCSADD_Add(char* data, uint16_t len);
uint8_t FCSADD_Check(char* data, uint16_t len);
void FCSXOR_Add(char* data, uint16_t len);
uint8_t FCSXOR_Check(char* data, uint16_t len);

uint16_t GENIUS_CRC16(char *sour, uint8_t len);
uint16_t LG_CRC16(char *sour, uint8_t len);
void LG_CRC16_Add(char *sour, uint8_t len);
uint8_t LG_CRC16_Check(char *sour, uint8_t len);

void CC110X_CRC16_Add(char* data, uint8_t len);
uint8_t CC110X_CRC16_Check(char* data, uint8_t len);
void CC110X_Whitening(char *data, uint8_t len);

//Ham dat thoi gian bo xung:
void RTC_SetDateTime(uint8_t yy, uint8_t MM, uint8_t dd, uint8_t HH, uint8_t mm, uint8_t ss);
void RTC_SetDateTime64(uint64_t yyyyMMddwwHHmmss);
void RTC_SetDateTimeSF(char sour[]);
void PrintfCurrentDT(void);
void PrintDebug(int id, char *data, int len);

uint16_t GetAvailableMemory(void);

#endif /* __COMMON_H */
