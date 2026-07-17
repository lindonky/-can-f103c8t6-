#include "pid.h"

/**
  * @brief  初始化 PID 控制器参数
  */
void PID_Init(PID_Controller_t *pid, float kp, float ki, float kd, float max_out, float i_max, float deadband)
{
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
    
    pid->max_out = max_out;
    pid->integral_max = i_max;
    pid->deadband = deadband;
    
    pid->error_last = 0.0f;
    pid->integral = 0.0f;
}

/**
  * @brief  PID 计算（位置式）
  * @param  pid: PID结构体指针
  * @param  current_error: 当前误差（如视觉像素差）
  * @retval 计算得出的控制量（带符号的 RPM 速度）
  */
float PID_Calculate(PID_Controller_t *pid, float current_error)
{
    float output = 0.0f;
    float P_out, I_out, D_out;

    // 1. 死区判断：如果误差很小，直接输出0，防止云台在中心点反复横跳
    if (current_error > -pid->deadband && current_error < pid->deadband) {
        pid->error_last = current_error;
        // 可选：在死区内清空积分，防止死区外突然爆发
        pid->integral = 0.0f; 
        return 0.0f;
    }

    // 2. 比例项
    P_out = pid->Kp * current_error;

    // 3. 积分项及积分抗饱和限幅
    pid->integral += current_error;
    if (pid->integral > pid->integral_max) pid->integral = pid->integral_max;
    if (pid->integral < -pid->integral_max) pid->integral = -pid->integral_max;
    I_out = pid->Ki * pid->integral;

    // 4. 微分项
    D_out = pid->Kd * (current_error - pid->error_last);
    pid->error_last = current_error;

    // 5. 总输出计算
    output = P_out + I_out + D_out;

    // 6. 输出限幅（限制电机最大转速）
    if (output > pid->max_out) output = pid->max_out;
    if (output < -pid->max_out) output = -pid->max_out;

    return output;
}

/**
  * @brief  重置 PID 历史状态（在丢失目标重新寻找时调用）
  */
void PID_Reset(PID_Controller_t *pid)
{
    pid->error_last = 0.0f;
    pid->integral = 0.0f;
}