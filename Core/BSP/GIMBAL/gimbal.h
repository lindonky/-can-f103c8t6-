#ifndef __GIMBAL_H
#define __GIMBAL_H

#include "main.h"
#include "Emm_V5.h"
#include "pid.h"
#include "k230_parser.h"
/* 电机 ID 配置 */
#define GIMBAL_YAW_ID        4    // X轴(航向)电机ID
#define GIMBAL_PITCH_ID      2    // Y轴(俯仰)电机ID

/* 电机转向配置 (根据你的实际机械安装方向修改) 
   0 为 CW (顺时针)，1 为 CCW (逆时针) */
#define YAW_DIR_FORWARD      0
#define YAW_DIR_REVERSE      1
#define PITCH_DIR_FORWARD    0
#define PITCH_DIR_REVERSE    1

/* 暴露出 PID 结构体，方便在 debug 模式下或者串口随时在线调参 */
extern PID_Controller_t Gimbal_PID_Yaw;
extern PID_Controller_t Gimbal_PID_Pitch;

/* 云台控制函数声明 */
void Gimbal_Init(void);
void Gimbal_Enable(bool state);
void Gimbal_Vision_Track(int16_t pixel_error_x, int16_t pixel_error_y);

#endif /* __GIMBAL_H */