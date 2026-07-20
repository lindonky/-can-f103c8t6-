#include "gimbal.h"
#include <math.h>

/* 实例化控制器 */
PID_Controller_t Gimbal_Vision_PID_Yaw;
PID_Controller_t Gimbal_Gyro_PID_Yaw;
PID_Controller_t Gimbal_Vision_PID_Pitch;

float Target_Angle_Yaw = 0.0f;
extern volatile int16_t gyro_angle_raw;
extern volatile uint8_t gyro_rx_done;
/**
  * @brief  云台初始化（配置混合 PID 参数）
  */
void Gimbal_Init(void)
{
    /* ========= Yaw轴 串级控制 ========= */
    // 外环: 像素偏差 -> 目标角度补偿步长
    PID_Init(&Gimbal_Vision_PID_Yaw, 0.05f, 0.0f, 0.01f, 10.0f, 5.0f, 5.0f);
    // 内环: 角度偏差 -> 目标速度 (最大限制500RPM)[cite: 4]
    PID_Init(&Gimbal_Gyro_PID_Yaw, 15.0f, 0.0f, 1.0f, 500.0f, 1000.0f, 0.2f);

    /* ========= Pitch轴 单环控制 ========= */
    // 视觉单环: 像素偏差 -> 目标速度 (最大限制400RPM, 10像素死区)[cite: 4]
    PID_Init(&Gimbal_Vision_PID_Pitch, 0.5f, 0.0f, 0.1f, 400.0f, 1000.0f, 10.0f);
}

void Gimbal_Enable(bool state)
{
    Emm_V5_En_Control(GIMBAL_YAW_ID, state, false);
    Emm_V5_En_Control(GIMBAL_PITCH_ID, state, false);
}

/**
  * @brief  视觉处理环 (约80Hz)：处理 Yaw 外环，直出 Pitch 速度指令
  */
void Gimbal_OuterLoop_Vision_80Hz(int16_t pixel_error_x, int16_t pixel_error_y)
{
    // ================= 处理 Yaw 轴外环 (累加目标角度) =================
    float yaw_step = PID_Calculate(&Gimbal_Vision_PID_Yaw, (float)pixel_error_x);
    Target_Angle_Yaw += yaw_step;

    // ================= 处理 Pitch 轴单环 (直接执行 CAN 动作) =================
    float pitch_target_rpm = PID_Calculate(&Gimbal_Vision_PID_Pitch, (float)pixel_error_y);

    if (pitch_target_rpm == 0.0f)
    {
        Emm_V5_Stop_Now(GIMBAL_PITCH_ID, false);
    }
    else
    {
        uint16_t pitch_abs_rpm = (uint16_t)fabs(pitch_target_rpm);
        uint8_t pitch_dir = (pitch_target_rpm > 0.0f) ? PITCH_DIR_FORWARD : PITCH_DIR_REVERSE;
        Emm_V5_Vel_Control(GIMBAL_PITCH_ID, pitch_dir, pitch_abs_rpm, 0, false);
    }
}

/**
  * @brief  陀螺仪内环 (200Hz)：计算 Yaw 速度指令并下发
  * @param  current_imu_yaw: 陀螺仪解析的当前 Yaw 角度
  */
void Gimbal_InnerLoop_Gyro_200Hz(void)
{
    // ================= 处理 Yaw 轴内环 (根据角度偏差控速) =================
    if (gyro_rx_done == 1)
    {
        // 1. 将 raw 数据转换为实际角度
        // (注：这里除以多少取决于你陀螺仪的数据手册，通常是 /100.0f 或 /32768.0f)
         float current_imu_yaw = (float)gyro_angle_raw / 100.0f;  //注意

        // 2. 处理 Yaw 轴内环 (根据角度偏差控速)
        float yaw_angle_error = Target_Angle_Yaw - current_imu_yaw;
        float yaw_target_rpm = PID_Calculate(&Gimbal_Gyro_PID_Yaw, yaw_angle_error);

        // 3. 执行 CAN 动作下发
        if (yaw_target_rpm == 0.0f)
        {
            Emm_V5_Stop_Now(GIMBAL_YAW_ID, false);
        }
        else
        {
            uint16_t yaw_abs_rpm = (uint16_t)fabs(yaw_target_rpm);
            uint8_t yaw_dir = (yaw_target_rpm > 0.0f) ? YAW_DIR_FORWARD : YAW_DIR_REVERSE;
            Emm_V5_Vel_Control(GIMBAL_YAW_ID, yaw_dir, yaw_abs_rpm, 0, false);
        }

        // 4. 清除解析完成标志位，等待串口接收下一帧[cite: 2]
        gyro_rx_done = 0;
    }
}