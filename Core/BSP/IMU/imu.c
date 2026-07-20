#include "imu.h"
#include "main.h"
uint8_t test_comm[20];

// void test(void)
// {
// 	test_comm[0]=0xaa;
// 	test_comm[1]=0x06;
// 	test_comm[2]=0x01;
// 	test_comm[3]=0x02;
// 	test_comm[4]=0x01;
// 	uint16_t aa;
// 	aa = CRC16(test_comm, 5);
//
// 	 test_comm[5] = aa & 0xFF;        // CRC_L
//    test_comm[6] = (aa >> 8) & 0xFF; // CRC_H
//
// }

extern UART_HandleTypeDef huart2;

volatile int16_t gyro_angle_raw;
volatile int16_t gyro_dps_raw;
volatile uint8_t gyro_rx_done;


void Gyro_ParseFrame(uint8_t data)
{
	//test();
    static uint8_t state = 0;
    static uint8_t frame[9];
    static uint8_t idx = 0;
    uint16_t crc_calc;
    uint16_t crc_recv;

    switch(state)
    {
        case 0:     // 地址
            if(data == 0x0A)
            {
                idx = 0;
                frame[idx++] = data;
                state = 1;
            }
            break;

        case 1:     // 功能码
            if(data == 0x03)
            {
                frame[idx++] = data;
                state = 2;
            }
            else
            {
                state = 0;
            }
            break;

        case 2:     // 字节数
            if(data == 0x04)
            {
                frame[idx++] = data;
                state = 3;
            }
            else
            {
                state = 0;
            }
            break;

        case 3:     // 接收剩余6字节
            frame[idx++] = data;

            if(idx >= 9)
            {
                /* CRC校验 */
                // crc_calc = CRC16(frame, 7);
                // crc_recv = frame[7] |
                //           ((uint16_t)frame[8] << 8);

                // if(crc_calc == crc_recv)
                // {
                    /* 角度 */
                    gyro_angle_raw =
                        (int16_t)(
                            ((uint16_t)frame[3] << 8) |
                             frame[4]);

                    /* 角速度 */
                    gyro_dps_raw =
                        (int16_t)(
                            ((uint16_t)frame[5] << 8) |
                             frame[6]);

                    gyro_rx_done = 1;
                // }

                state = 0;
            }
            break;

        default:
            state = 0;
            break;
    }
}


	
void Gyro_ConfigReportRateRx(void)//AA 06 01 01 01 AD 00
{

		uint8_t idx = 0;
		uint8_t frame[20];

    frame[idx++] = 0xAA;        // 从站地址
    frame[idx++] = 0x06;        // 功能码：写单个保持寄存器
    frame[idx++] = 0x01;        // 起始寄存器高字节
	
	
    frame[idx++] = 0x01;        // 起始寄存器低字节

  
    // 寄存器 1
    frame[idx++] = 0x01;

//    uint16_t crc = CRC16(frame, idx);
//	frame[idx++] = crc & 0xFF;
//	frame[idx++] = (crc >> 8) & 0xFF;
    frame[idx++] = 0xad;
    frame[idx++] = 0x00;

    // 发送
    HAL_UART_Transmit(&huart2, frame, idx, 100);
}


