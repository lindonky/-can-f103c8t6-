// #include "k230_parser.h"
//
// /* 实例化全局视觉数据 */
// K230_Data_t K230_Target;
//
// /* 状态机枚举 */
// typedef enum {
//     STATE_HEAD1 = 0,
//     STATE_HEAD2,
//     STATE_ERR_X_H,
//     STATE_ERR_X_L,
//     STATE_ERR_Y_H,
//     STATE_ERR_Y_L,
//     STATE_CHECKSUM,
//     STATE_TAIL
// } K230_ParseState_e;
//
// /**
//   * @brief  初始化K230数据
//   */
// void K230_Parser_Init(void)
// {
//     K230_Target.err_x = 0;
//     K230_Target.err_y = 0;
//     K230_Target.is_updated = false;
// }
//
// /**
//   * @brief  K230 串口单字节状态机解析函数
//   * @param  byte: 从串口收到的单字节数据
//   * @note   极高效率，无循环，无阻塞。适合直接放在串口中断或数据流处理任务中。
//   */
// void K230_Parse_Byte(uint8_t byte)
// {
//     static K230_ParseState_e state = STATE_HEAD1;
//     static uint8_t checksum = 0;
//
//     // 用于暂存解析中的数据，校验成功后再更新到全局变量
//     static int16_t temp_x = 0;
//     static int16_t temp_y = 0;
//
//     switch (state)
//     {
//         case STATE_HEAD1:
//             if (byte == 0x55) {
//                 state = STATE_HEAD2;
//             }
//             break;
//
//         case STATE_HEAD2:
//             if (byte == 0xAA) {
//                 state = STATE_ERR_X_H;
//                 checksum = 0; // 重置校验和
//             } else {
//                 state = STATE_HEAD1; // 匹配失败，退回初始状态
//             }
//             break;
//
//         case STATE_ERR_X_H:
//             temp_x = (int16_t)(byte << 8);
//             checksum += byte;
//             state = STATE_ERR_X_L;
//             break;
//
//         case STATE_ERR_X_L:
//             temp_x |= byte;
//             checksum += byte;
//             state = STATE_ERR_Y_H;
//             break;
//
//         case STATE_ERR_Y_H:
//             temp_y = (int16_t)(byte << 8);
//             checksum += byte;
//             state = STATE_ERR_Y_L;
//             break;
//
//         case STATE_ERR_Y_L:
//             temp_y |= byte;
//             checksum += byte;
//             state = STATE_CHECKSUM;
//             break;
//
//         case STATE_CHECKSUM:
//             if (byte == checksum) {
//                 state = STATE_TAIL;
//             } else {
//                 state = STATE_HEAD1; // 校验错，丢弃该帧
//             }
//             break;
//
//         case STATE_TAIL:
//             if (byte == 0xAF) {
//                 // 完整的一帧解析成功且校验通过，安全地更新全局变量
//                 K230_Target.err_x = temp_x;
//                 K230_Target.err_y = temp_y;
//                 K230_Target.is_updated = true;
//             }
//             state = STATE_HEAD1; // 准备接收下一帧
//             break;
//
//         default:
//             state = STATE_HEAD1;
//             break;
//     }
// }
#include "k230_parser.h"
#include <stdio.h>
#include <string.h>

/* 实例化全局视觉数据 */
K230_Data_t K230_Target;

/* 字符串接收缓冲区 */
#define RX_BUF_SIZE 32
static char rx_buf[RX_BUF_SIZE];
static uint8_t rx_idx = 0;

/**
  * @brief  初始化K230数据与缓冲区
  */
void K230_Parser_Init(void)
{
    K230_Target.err_x = 0;
    K230_Target.err_y = 0;
    K230_Target.is_updated = false;

    rx_idx = 0;
    memset(rx_buf, 0, sizeof(rx_buf));
}

/**
  * @brief  串口单字节字符串解析函数 (支持格式: "X:10,Y:-20\r\n")
  * @param  byte: 从串口收到的单字节数据
  */
void K230_Parse_Byte(uint8_t byte)
{
    // 遇到换行符 '\n'，说明一帧字符串结束，开始解析
    if (byte == '\n')
    {
        rx_buf[rx_idx] = '\0'; // 添加字符串结束符

        int temp_x = 0, temp_y = 0;

        // 使用 sscanf 提取数据。格式必须严格匹配 "X:%d,Y:%d"
        // 返回值为 2 代表成功提取到了 x 和 y 两个数字
        if (sscanf(rx_buf, "X:%d,Y:%d", &temp_x, &temp_y) == 2)
        {
            K230_Target.err_x = (int16_t)temp_x;
            K230_Target.err_y = (int16_t)temp_y;
            K230_Target.is_updated = true; // 触发主循环更新电机
        }

        // 解析完毕，清空索引准备接收下一行
        rx_idx = 0;
    }
    // 过滤掉回车符 '\r'，只存储有效字符
    else if (byte != '\r')
    {
        // 防溢出保护
        if (rx_idx < (RX_BUF_SIZE - 1))
        {
            rx_buf[rx_idx++] = (char)byte;
        }
        else
        {
            rx_idx = 0; // 如果数据过长（比如发错了），强制丢弃重来
        }
    }
}