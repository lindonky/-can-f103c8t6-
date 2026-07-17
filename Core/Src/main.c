/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"
#include "Emm_V5.h"
#include "gimbal.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t rx_buffer[64];        // DMA 接收缓冲区，足够装下整句话
uint32_t last_track_time = 0; // 用于裸机非阻塞定时的系统滴答时间

uint32_t debug_rx_count = 0;  // 记录到底触发了多少次串口中断
char debug_last_char = ' ';   // 记录最后一个收到的字符
char oled_buf[32];            // OLED 字符串格式化缓冲
uint32_t debug_frame_count = 0;
uint32_t count = 0;
uint32_t debug_can_rx_count = 0;  // 记录 CAN 总共收到了多少帧数据
uint32_t debug_can_last_id = 0;   // 记录最新收到的一帧数据的 ID

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_CAN_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init();

  // CAN_FilterTypeDef sFilterConfig;
  // sFilterConfig.FilterBank = 0;
  // sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  // sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  // sFilterConfig.FilterIdHigh = 0x0000;
  // sFilterConfig.FilterIdLow = 0x0000;
  // // 掩码全部设为 0，意味着“不检查任何位”，所有设备的报文全部放行！
  // sFilterConfig.FilterMaskIdHigh = 0x0000;
  // sFilterConfig.FilterMaskIdLow = 0x0000;
  // sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  // sFilterConfig.FilterActivation = ENABLE;
  // sFilterConfig.SlaveStartFilterBank = 14;

  USER_CAN1_Filter_Init();



  if(HAL_CAN_Start(&hcan) != HAL_OK) { Error_Handler(); }
  if(HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) { Error_Handler(); }
  HAL_Delay(500);
 // Emm_V5_Pos_Control(1, 0, 1000, 0, 256000, 0, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  K230_Parser_Init();
  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, sizeof(rx_buffer));

  // OLED_ShowString(0, 0, "Inited", OLED_8X16);
  // OLED_Update();
  // 初始化云台 PID 参数
  Gimbal_Init();
  // 上电后等待稳定，使能锁死电机
  HAL_Delay(100);
  //Gimbal_Enable(true);
  OLED_ShowString(0, 0, "Inited", OLED_8X16);
  OLED_Update();
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if (HAL_GetTick() - last_track_time >= 30)  //33Hz
    {
      last_track_time = HAL_GetTick();
      //  OLED_Clear();
      // OLED_Printf(0,0,OLED_6X8,"%d,%d",count++);
      // // OLED_Printf(0,0,OLED_6X8,"%d,%d",K230_Target.err_x, K230_Target.err_y);
      // // OLED_Printf(0,12,OLED_6X8,"RX:%lu CH:%d  ",debug_rx_count, debug_frame_count);
      // // OLED_Printf(0,24,OLED_6X8,"UPD:%d ERR:%lu  ",K230_Target.is_updated, hcan.ErrorCode);
      // // OLED_Printf(0,36,OLED_6X8,"B:%s", rx_buffer);
      //  OLED_Update();

      // 只有当状态机完整拼接出一帧校验通过的数据时，才去控制电机
      if (K230_Target.is_updated == true)
      {
        // 第一时间清除标志位，等待下一帧
        K230_Target.is_updated = false;
        OLED_Clear();
        OLED_Printf(0,0,OLED_6X8,"%d,%d",K230_Target.err_x, K230_Target.err_y);
        // OLED_Printf(0,12,OLED_6X8,"RX:%lu CH:%d  ",debug_rx_count, debug_frame_count);
        // OLED_Printf(0,24,OLED_6X8,"UPD:%d ERR:%lu  ",K230_Target.is_updated, hcan.ErrorCode);
        // OLED_Printf(0,36,OLED_6X8,"B:%s", rx_buffer);
        OLED_Printf(0, 48, OLED_6X8, "CRX:%lu,ID:%X", debug_can_rx_count,debug_can_last_id);
        OLED_Update();
         // OLED_Printf(0,0,OLED_6X8,"%d,%d",K230_Target.err_x, K230_Target.err_y);
        // OLED_Printf(30,0,OLED_6X8,"RX:%lu CH:%c  ",debug_rx_count, debug_last_char);
        // OLED_Printf(60,0,OLED_6X8,"UPD:%d ERR:%lu  ",K230_Target.is_updated, hcan.ErrorCode);
         // OLED_Update();

        // 直接读取全局结构体中的像素误差，送入云台 PID 驱动
        Gimbal_Vision_Track(K230_Target.err_x, K230_Target.err_y);
        Emm_V5_Read_Sys_Params(2,1);
      }
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
// 在 stm32f1xx_it.c 或者你统一写回调的地方：
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//   if (huart->Instance == USART1) // 假设 K230 接在串口1
//   {
//     // rx_buffer[0] 是你每次 HAL_UART_Receive_IT 接收存放的那个字节
//     debug_rx_count++;
//     debug_last_char = (char)rx_buffer[0];
//
//     K230_Parse_Byte(rx_buffer[0]);
//
//     // 重新开启下一次单字节接收
//     HAL_UART_Receive_IT(&huart1, rx_buffer, 1);
//   }
// }
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if (huart->Instance == USART1)
  {
    debug_rx_count++; // 只要进中断，屏幕上的 RX 必定 +1

    // 1. 强制补上结束符
    rx_buffer[Size] = '\0';

    // 2. 解析坐标数据
    int temp_x = 0, temp_y = 0;
    if (sscanf((char *)rx_buffer, "X:%d,Y:%d", &temp_x, &temp_y) == 2 ||
        sscanf((char *)rx_buffer, "X:%d, Y:%d", &temp_x, &temp_y) == 2)
    {
      K230_Target.err_x = (int16_t)temp_x;
      K230_Target.err_y = (int16_t)temp_y;
      K230_Target.is_updated = true;

      debug_frame_count++; // 解析成功，屏幕上的 CH +1
    }

    // 3. 重启 DMA 接收
    //    HAL 底层在调用本回调之前，已经完成了：
    //      - HAL_DMA_Abort → DMA 硬件已停止，状态已复位
    //      - RxState = READY，ReceptionType = STANDARD
    //    所以这里只需要重新调用即可，千万不要手动操作 DMA 寄存器！
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, sizeof(rx_buffer));
    __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);        // 关闭过半中断防干扰
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    // 遇到静电或拔插串口线的噪声，清除错误后重启
    __HAL_UART_CLEAR_OREFLAG(huart);
    __HAL_UART_CLEAR_NEFLAG(huart);
    __HAL_UART_CLEAR_FEFLAG(huart);

    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, sizeof(rx_buffer));
    __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
  }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  CAN_RxHeaderTypeDef RxHeader;
  uint8_t RxData[8];

  // 读取接收到的数据（这一步非常关键！它会自动清除 FIFO 里的待处理标志，防止死机）
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
  {
    debug_can_rx_count++;               // CAN 接收计数器 +1
    debug_can_last_id = RxHeader.StdId; // 抓取是谁发来的 ID

    // 如果你在使用张大头的驱动库，通常需要在这里把数据“喂”给它的解析函数
    // 取决于你用的版本，可能叫这个名字：
    // Emm_V5_Receive_Data(&RxHeader, RxData);
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
