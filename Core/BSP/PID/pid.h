#ifndef __PID_H
#define __PID_H

#include <stdint.h>

/* PID 结构体定义 */
typedef struct {
    float Kp;             // 比例系数
    float Ki;             // 积分系数
    float Kd;             // 微分系数

    float error_last;     // 上一次误差
    float integral;       // 积分累加值
    float integral_max;   // 积分限幅（防止积分饱和）

    float max_out;        // 输出限幅（最大速度限制 RPM）
    float deadband;       // 误差死区（在这个范围内输出为0，防抖）
} PID_Controller_t;

/* PID 函数声明 */
void PID_Init(PID_Controller_t *pid, float kp, float ki, float kd, float max_out, float i_max, float deadband);
float PID_Calculate(PID_Controller_t *pid, float current_error);
void PID_Reset(PID_Controller_t *pid);

#endif /* __PID_H *///
// Created by 林~ on 2026/7/17.
//

#ifndef F103_YUNTAI_PID_H
#define F103_YUNTAI_PID_H

#endif //F103_YUNTAI_PID_H
