#ifndef MOTOR_CRC_h
#define MOTOR_CRC_h

#include <stdint.h>  // 加上这一行 C 语言标准库

unsigned short CRC16(uint8_t *puchMsg, unsigned short usDataLen); //用于计算 CRC[cite: 10]

#endif