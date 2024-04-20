/**
 @file c_crc32.h
 @brief Header fuer c_crc32.c
*/
#ifndef CRC32_H
#define CRC32_H
#ifdef  __cplusplus
extern "C" {
#endif

// CRC Funktionen ALT und bewaehrt
//_________________________________

unsigned long AddStringCRC (char *Buffer, unsigned long crc32);
unsigned long AddCRCByte (char byte, unsigned long crc32);
unsigned long AddCRC (int LenOfBuffer, char *Buffer, unsigned long crc32);
int CRC32File(char *filename, unsigned int *out_crc32);

#ifdef  __cplusplus
}
#endif
#endif
