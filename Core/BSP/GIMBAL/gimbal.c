#include "gimbal.h"
#include <math.h>

/* 实例化双轴 PID 控制器 */
PID_Controller_t Gimbal_PID_Yaw;
PID_Controller_t Gimbal_PID_Pitch;

/**
  * @brief  云台初始化（配置 PID 参数）
  */
void Gimbal_Init(void)
{
    /* 
     * 参数含义: Kp, Ki, Kd, 速度限幅(RPM), 积分限幅, 像素死区
     * 注意：这里的 Kp 和 Kd 需要根据你的电机细分和机械重量实际微调
     * 初始调试建议只给 Kp，设 Ki=0, Kd=0。
     */
    
    // 初始化 Yaw 轴 PID: 最大速度限制 500 RPM，死区 10 个像素
    PID_Init(&Gimbal_PID_Yaw, 0.5f, 0.0f, 0.1f, 500.0f, 1000.0f, 10.0f);
    
    // 初始化 Pitch 轴 PID: 最大速度限制 400 RPM，死区 10 个像素
    PID_Init(&Gimbal_PID_Pitch, 0.5f, 0.0f, 0.1f, 400.0f, 1000.0f, 10.0f);
}

/**
  * @brief  云台整体使能控制
  * @param  state: true 使能电机(锁轴)， false 关闭电机(脱机)
  */
void Gimbal_Enable(bool state)
{
    // 调用底层 CAN 驱动协议
    Emm_V5_En_Control(GIMBAL_YAW_ID, state, false);
    Emm_V5_En_Control(GIMBAL_PITCH_ID, state, false);
}

/**
  * @brief  执行一次视觉追踪速度下发
  * @param  pixel_error_x: K230 X轴目标像素偏差 (中心为0)
  * @param  pixel_error_y: K230 Y轴目标像素偏差 (中心为0)
  * @note   需在 FreeRTOS 任务中周期性调用此函数 (例如 30Hz)
  */
void Gimbal_Vision_Track(int16_t pixel_error_x, int16_t pixel_error_y)
{
    // 1. 通过 PID 计算期望转速 (带符号)
    float yaw_target_rpm = PID_Calculate(&Gimbal_PID_Yaw, (float)pixel_error_x);
    float pitch_target_rpm = PID_Calculate(&Gimbal_PID_Pitch, (float)pixel_error_y);

    // =========================================================
    // 2. 处理 Yaw 轴 (X轴)
    // =========================================================
    if (yaw_target_rpm == 0.0f) 
    {
        // 如果速度算出来为 0 (进入死区)，立刻急刹车
        Emm_V5_Stop_Now(GIMBAL_YAW_ID, false);
    } 
    else 
    {
        // 提取绝对速度
        uint16_t yaw_abs_rpm = (uint16_t)fabs(yaw_target_rpm);
        // 判断方向 (如果追逐方向反了，把这里的 YAW_DIR_FORWARD 和 REVERSE 对调)
        uint8_t yaw_dir = (yaw_target_rpm > 0.0f) ? YAW_DIR_FORWARD : YAW_DIR_REVERSE;
        
        // 下发速度控制指令 (acc参数给0，代表急加速，最适合视觉实时跟踪)
        Emm_V5_Vel_Control(GIMBAL_YAW_ID, yaw_dir, yaw_abs_rpm, 0, false);
    }

    // =========================================================
    // 3. 处理 Pitch 轴 (Y轴)
    // =========================================================
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