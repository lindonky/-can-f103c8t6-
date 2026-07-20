#ifndef __PERF_MONITOR_H
#define __PERF_MONITOR_H

#include "stm32f1xx_hal.h"  // 如果移植到其他系列，只需改为对应的库 (如 stm32f4xx_hal.h)
#include <stdint.h>
#include <stdbool.h>

/* DWT (Data Watchpoint and Trace) 寄存器地址定义 (Cortex-M3/M4/M7通用) */
#define PERF_DWT_CR      *(volatile uint32_t *)0xE0001000
#define PERF_DWT_CYCCNT  *(volatile uint32_t *)0xE0001004
#define PERF_DEM_CR      *(volatile uint32_t *)0xE000EDFC

#define PERF_DEM_CR_TRCENA       (1 << 24)
#define PERF_DWT_CR_CYCCNTENA    (1 << 0)

/* 帧率/频率 统计结构体 */
typedef struct {
    uint32_t frame_count;  // 内部累加器：当前秒内的运行次数
    uint32_t fps_result;   // 对外输出：计算出的真实频率 (Hz)
    uint32_t last_tick;    // 内部时间戳：记录上次结算的时间
} Perf_FPS_t;

/* ======= 函数声明 ======= */

/* 1. DWT 极速测时功能 (精确到微秒级) */
void Perf_Init(void);
void Perf_Start_Measure(void);
float Perf_End_Measure_us(void);

/* 2. 频率(Hz) 统计功能 */
void Perf_FPS_Tick(Perf_FPS_t *fps_obj);
bool Perf_FPS_Calc_1Hz(Perf_FPS_t *fps_obj);

#endif /* __PERF_MONITOR_H */