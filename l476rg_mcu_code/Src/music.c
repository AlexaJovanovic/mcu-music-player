/*
 * music.c
 *
 *  Created on: Sep 28, 2025
 *      Author: Aleksa
 */

#include "music.h"

uint32_t melody[] = {
  NOTE_A4, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_E4,
  NOTE_FS4, NOTE_G4, NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_A4, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_FS4, NOTE_G4, NOTE_A4,
  NOTE_E4, NOTE_A4, NOTE_FS4, NOTE_D4
};

int melody_length = 26;

static const NoteFrequencyMap note_frequencies[] = {
    {'C', 261.2},
    {'D', 293.66f},
    {'E', 329.63f},
    {'F', 349.23f},
    {'G', 392.00f},
    {'A', 440.00f},
    // Add more notes as needed, for example:
    // {'B', 493.88f},
};

static const int NUM_NOTES = sizeof(note_frequencies) / sizeof(NoteFrequencyMap);

float get_frequency(char note_char) {
    for (int i = 0; i < NUM_NOTES; i++) {
        if (note_frequencies[i].note_char == note_char) {
            return note_frequencies[i].frequency;
        }
    }
    return 0.0f; // Return 0.0f or some other error value if note is not found
}

#include "main.h"
#include "cmsis_os.h"

#include "uart_driver.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#include <stdio.h>

extern UART_HandleTypeDef huart2;
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim1;

// Note lookup table (must match Python NOTE_MAP)
const float notes[] = {
    261.63, // C4
    277.18, // CS4
    293.66, // D4
    311.13, // DS4
    329.63, // E4
    349.23, // F4
    369.99, // FS4
    392.00, // G4
    415.23, // GS4
    440.00, // A4
    466.16, // AS4
    493.88, // B4
	523.26, // C5
    554.36, // CS5
    587.32, // D5
    622.26, // DS5
    659.26, // E5
    698.46 // F5
};

// Queues for UART RX → Melody task
QueueHandle_t noteQueue;

void ADCReaderTask(void *argument);
void MusicPlayerTask(void *argument);
void MusicSetterTask(void *argument);


void MusicPlayerTasks_Init(void) {
    noteQueue = xQueueCreate(8, sizeof(uint8_t));

    UART_PC_Init();
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);


    xTaskCreate(ADCReaderTask, "ADC", 128, NULL, 2, NULL);
    xTaskCreate(MusicPlayerTask, "Melody", 128, NULL, 2, NULL);

    // Only for testing:
    // xTaskCreate(MusicSetterTask, "MelodySetter", 128, NULL, 2, NULL);
}

// --- ADC task: read ADC and send to PC ---
void ADCReaderTask(void *argument) {
    for (;;) {
        HAL_ADC_Start(&hadc1);
        osDelay(300); // sample every 200 ms

        if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
            uint32_t adc_val = HAL_ADC_GetValue(&hadc1);

            UART_PC_SendUInt(adc_val);
        }
        HAL_ADC_Stop(&hadc1);
    }
}

// Helper adjusting PWM period to play the sound of the right frequency
void Buzzer_SetTone(uint32_t freqHz) {
    uint32_t timer_clk = 1000000; // after prescaler
    uint32_t arr = (timer_clk / freqHz) - 1;
    __HAL_TIM_SET_AUTORELOAD(&htim1, arr);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, (arr+1)/3); // 33% duty
}

// --- Music player task: wait for note codes and play ---
void MusicPlayerTask(void *argument) {
    // Set pulse width to zero and start PWM channel
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 0);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

	UART_PC_SendString("PLAYER STARTED\n");
    osDelay(500);

    uint8_t code = 0;
    for (;;) {
        // Wait for next note code from UART ISR
        if (xQueueReceive(noteQueue, &code, portMAX_DELAY) == pdPASS) {
        	if (code < sizeof(notes)/sizeof(notes[0])) {
				float freq = notes[code];
              	Buzzer_SetTone((uint32_t)freq);
        	}
        	if (code == NOTE_STOP){
				// Special STOP code: silence buzzer
				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 0);
			}
        }
        else UART_PC_SendString("note not received\n");
    }
}

// --- Melody setter task for testing: puts notes in the playing queue ---
void MusicSetterTask(void *argument) {
	uint8_t code;
	int i = 0;
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

	for(;;)
	{
		  HAL_ADC_Start(&hadc1);
		  vTaskDelay(pdMS_TO_TICKS(500));
		  code = melody[i++];
		  if (i == melody_length) i = 0;

		  xQueueSend(noteQueue, &code, portMAX_DELAY);
	}
}

