#include "uart_driver.h"
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "music.h"

extern UART_HandleTypeDef huart2;   // dedicated for PC connection

uint8_t rx_byte;

void UART_PC_Init(void) {
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    if (HAL_UART_Init(&huart2) != HAL_OK) {
    	// some Error_Handler();
    }

    // Start async note recieving
    HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
}


void UART_PC_SendChar(char c) {
    HAL_UART_Transmit(&huart2, (uint8_t *)&c, 1, HAL_MAX_DELAY);
}

void UART_PC_SendString(const char *str) {
    while (*str) {
        UART_PC_SendChar(*str++);
    }
}

void UART_PC_SendUInt(uint32_t value) {
    char buffer[13];
    sprintf(buffer, "%lu\n", (unsigned long)value);
    UART_PC_SendString(buffer);
}

char UART_PC_ReceiveChar(void) {
    char c;
    HAL_UART_Receive(&huart2, (uint8_t *)&c, 1, HAL_MAX_DELAY);
    return c;
}


#include "queue.h"

// Shared resource
extern QueueHandle_t noteQueue;

// Redefining interrupt callback
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        // Push received note code into queue
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(noteQueue, &rx_byte, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

        // Restart reception
        HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
    }
}

