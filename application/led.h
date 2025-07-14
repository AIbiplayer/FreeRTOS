#ifndef LED_H
#define LED_H

#include "main.h"

void FreeRTOS_Start(void);
void Task_Init(void *pvParaMeters);
void Task_1(void *pvParaMeters);
void Task_2(void *pvParaMeters);
void Task_Static(void *pvParaMeters);
void Uart_printf(UART_HandleTypeDef *huart, char *format, ...);

#endif
