#ifndef __GIMBAL_H
#define __GIMBAL_H

#include "main.h"
#include "Emm_V5.h"
#include "pid.h"
#include "k230_parser.h" // 视你的代码结构而定

/* 电机 ID 配置 */
#define GIMBAL_YAW_ID        4    // X轴(航向)电机ID[cite: 5]
#define GIMBAL_PITCH_ID      2    // Y轴(俯仰)电机ID[cite: 5]

/* 电机转向配置 */
#define YAW_DIR_FORWARD      1
#define YAW_DIR_REVERSE      0
#define PITCH_DIR_FORWARD    0
#define PITCH_DIR_REVERSE    1

/* ================= 混合控制架构 ================= */
/* Yaw轴：串级 PID (视觉外环 + 陀螺仪内环) */
extern PID_Controller_t Gimbal_Vision_PID_Yaw;   // 视觉像素 -> 目标角度
extern PID_Controller_t Gimbal_Gyro_PID_Yaw;     // 角度误差 -> 电机速度

/* Pitch轴：单环 PID (视觉直接控速) */
extern PID_Controller_t Gimbal_Vision_PID_Pitch; // 视觉像素 -> 电机速度

/* Yaw 轴全局目标角度 */
extern float Target_Angle_Yaw;

/* 云台控制函数声明 */
void Gimbal_Init(void);
void Gimbal_Enable(bool state);

// 拆分后的任务调度接口
void Gimbal_OuterLoop_Vision_80Hz(int16_t pixel_error_x, int16_t pixel_error_y);
void Gimbal_InnerLoop_Gyro_200Hz(void);

#endif /* __GIMBAL_H */