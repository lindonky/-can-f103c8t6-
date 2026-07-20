#include "perf_monitor.h"

// 内部变量，用于记录掐表时的起始周期数
static uint32_t start_cycle = 0;

/**
  * @brief  初始化 DWT 周期计数器 (必须在 HAL_Init 和 时钟配置之后调用)
  */
void Perf_Init(void)
{
    PERF_DEM_CR |= PERF_DEM_CR_TRCENA;      // 开启内核跟踪模块
    PERF_DWT_CR |= PERF_DWT_CR_CYCCNTENA;   // 开启周期计数器
}

/**
  * @brief  开始测时（掐表）
  */
void Perf_Start_Measure(void)
{
    start_cycle = PERF_DWT_CYCCNT;
}

/**
  * @brief  结束测时并计算耗时
  * @retval 代码执行所消耗的微秒数 (us)
  * @note   使用 SystemCoreClock 自动适配 MCU 主频，无需手动写死 72MHz
  */
float Perf_End_Measure_us(void)
{
    uint32_t end_cycle = PERF_DWT_CYCCNT;
    uint32_t cost_cycles = end_cycle - start_cycle;
    
    // SystemCoreClock 默认是 72000000 (72MHz)
    // 每微秒的周期数 = 72000000 / 1000000 = 72
    return (float)cost_cycles / (SystemCoreClock / 1000000.0f);
}

/**
  * @brief  记录一次函数的执行（打卡）
  * @param  fps_obj: 目标帧率统计对象指针
  */
void Perf_FPS_Tick(Perf_FPS_t *fps_obj)
{
    fps_obj->frame_count++;
}

/**
  * @brief  每秒结算一次真实运行频率
  * @param  fps_obj: 目标帧率统计对象指针
  * @retval true: 刚好满 1 秒，数值已更新; false: 还未到 1 秒
  */
bool Perf_FPS_Calc_1Hz(Perf_FPS_t *fps_obj)
{
    uint32_t current_tick = HAL_GetTick();
    
    if (current_tick - fps_obj->last_tick >= 1000)
    {
        fps_obj->last_tick = current_tick;
        fps_obj->fps_result = fps_obj->frame_count; // 保存结算结果
        fps_obj->frame_count = 0;                   // 清零重新开始下一秒
        return true;
    }
    return false;
}