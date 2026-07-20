#ifndef __K230_PARSER_H
#define __K230_PARSER_H

#include <stdint.h>
#include <stdbool.h>

/* K230 视觉数据结构体 */
typedef struct {
    int16_t err_x;       // X轴像素误差
    int16_t err_y;       // Y轴像素误差
    bool    is_updated;  // 数据更新标志位
} K230_Data_t;

/* 暴露给外部的全局变量 */
extern  K230_Data_t K230_Target;

/* 函数声明 */
void K230_Parser_Init(void);
void K230_Parse_Byte(uint8_t byte);
uint8_t Fast_Parse_Vision(uint8_t *buf, int16_t *out_x, int16_t *out_y);
#endif /* __K230_PARSER_H */