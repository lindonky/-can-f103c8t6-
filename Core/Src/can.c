/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
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
#include "can.h"

/* USER CODE BEGIN 0 */
__IO CAN_t can = {0};
/* USER CODE END 0 */

CAN_HandleTypeDef hcan;

/* CAN init function */
void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 6;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_9TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**CAN GPIO Configuration
    PB8     ------> CAN_RX
    PB9     ------> CAN_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    __HAL_AFIO_REMAP_CAN1_2();

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN GPIO Configuration
    PB8     ------> CAN_RX
    PB9     ------> CAN_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8|GPIO_PIN_9);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/**
	* @brief   ³õÊ¼»¯ÂË²¨Æ÷
	* @param   ÎÞ
	* @retval  ÎÞ
	*/
void USER_CAN1_Filter_Init(void)
{
	// ¹ýÂËÆ÷½á¹¹Ìå
	CAN_FilterTypeDef  sFilterConfig;

	// ÉèÖÃSTM32µÄÖ¡ID - À©Õ¹Ö¡¸ñÊ½ - ²»¹ýÂËÈÎºÎÊý¾ÝÖ¡
	__IO uint8_t id_o, im_o; __IO uint16_t id_l, id_h, im_l, im_h;
	id_o = (0x00);
	id_h = (uint16_t)((uint16_t)id_o >> 5);								// ¸ß3Î»
	id_l = (uint16_t)((uint16_t)id_o << 11) | CAN_ID_EXT; // µÍ5Î»
	im_o = (0x00);
	im_h = (uint16_t)((uint16_t)im_o >> 5);
	im_l = (uint16_t)((uint16_t)im_o << 11) | CAN_ID_EXT;

	// ¹ýÂËÆ÷²ÎÊý
	sFilterConfig.FilterBank = 0;                      		// ¹ýÂËÆ÷1
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;  		// ÑÚÂëÄ£Ê½
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT; 		// 32Î»¹ýÂËÆ÷Î»¿í
	sFilterConfig.FilterIdHigh = id_h;               			// ¹ýÂËÆ÷±êÊ¶·ûµÄ¸ß16Î»Öµ
	sFilterConfig.FilterIdLow = id_l;                			// ¹ýÂËÆ÷±êÊ¶·ûµÄµÍ16Î»Öµ
	sFilterConfig.FilterMaskIdHigh = im_h;           			// ¹ýÂËÆ÷ÆÁ±Î±êÊ¶·ûµÄ¸ß16Î»Öµ
	sFilterConfig.FilterMaskIdLow = im_l;            			// ¹ýÂËÆ÷ÆÁ±Î±êÊ¶·ûµÄµÍ16Î»Öµ
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0; 		// Ö¸Ïò¹ýÂËÆ÷µÄFIFOÎª0
	sFilterConfig.FilterActivation = ENABLE;           		// Ê¹ÄÜ¹ýÂËÆ÷
	sFilterConfig.SlaveStartFilterBank = 0;           		// ´Ó¹ýÂËÆ÷ÅäÖÃ£¬ÓÃÀ´Ñ¡Ôñ´Ó¹ýÂËÆ÷µÄ¼Ä´æÆ÷±àºÅ

	// ÅäÖÃ²¢×Ô¼ì
	while(HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK);
}

/**
	* @brief   CAN·¢ËÍ¶à¸ö×Ö½Ú
	* @param   ÎÞ
	* @retval  ÎÞ
	*/
void can_SendCmd(__IO uint8_t *cmd, uint8_t len)
{
	static uint32_t TxMailbox; __IO uint8_t i = 0, j = 0, k = 0, l = 0, packNum = 0;

	// ³ýÈ¥IDµØÖ·ºÍ¹¦ÄÜÂëºóµÄÊý¾Ý³¤¶È
	j = len - 2;

	// ·¢ËÍÊý¾Ý
	while(i < j)
	{
		// Êý¾Ý¸öÊý
		k = j - i;

		// Ìî³ä»º´æ
		can.CAN_TxMsg.StdId = 0x00;
		can.CAN_TxMsg.ExtId = ((uint32_t)cmd[0] << 8) | (uint32_t)packNum;
		can.txData[0] = cmd[1];
		can.CAN_TxMsg.IDE = CAN_ID_EXT;
		can.CAN_TxMsg.RTR = CAN_RTR_DATA;

		// Ð¡ÓÚ8×Ö½ÚÃüÁî
		if(k < 8)
		{
			for(l=0; l < k; l++,i++) { can.txData[l + 1] = cmd[i + 2]; } can.CAN_TxMsg.DLC = k + 1;
		}
		// ´óÓÚ8×Ö½ÚÃüÁî£¬·Ö°ü·¢ËÍ£¬Ã¿°üÊý¾Ý×î¶à·¢ËÍ8¸ö×Ö½Ú
		else
		{
			for(l=0; l < 7; l++,i++) { can.txData[l + 1] = cmd[i + 2]; } can.CAN_TxMsg.DLC = 8;
		}

		// ·¢ËÍÊý¾Ý
		HAL_CAN_AddTxMessage((&hcan), (CAN_TxHeaderTypeDef *)(&can.CAN_TxMsg), (uint8_t *)(&can.txData), (&TxMailbox));

		// ¼ÇÂ¼·¢ËÍµÄµÚ¼¸°üµÄÊý¾Ý
		++packNum;
	}
}
/* USER CODE END 1 */

