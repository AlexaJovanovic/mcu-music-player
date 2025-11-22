/*
 * music.h
 *
 *  Created on: Sep 28, 2025
 *      Author: Aleksa
 */

#ifndef INC_MUSIC_H_
#define INC_MUSIC_H_

#include <stdint.h>

#define NOTE_C4 0
#define NOTE_CS4 1
#define NOTE_D4 2
#define NOTE_DS4 3
#define NOTE_E4 4
#define NOTE_F4 5
#define NOTE_FS4 6
#define NOTE_G4 7
#define NOTE_GS4 8
#define NOTE_A4 9
#define NOTE_AS4 10
#define NOTE_B4 11
#define NOTE_C5 11

#define NOTE_STOP ((uint8_t) 100)

#define F_NOTE_C4 261.63
#define F_NOTE_CS4 277.18
#define F_NOTE_D4 293.66
#define F_NOTE_DS4 311.13
#define F_NOTE_E4 329.63
#define F_NOTE_F4 349.23
#define F_NOTE_FS4 369.99
#define F_NOTE_G4 392.00
#define F_NOTE_GS4 415.23
#define F_NOTE_A4 440.00
#define F_NOTE_AS4 466.16
#define F_NOTE_B4 493.88
#define F_NOTE_C5 523.26
#define F_NOTE_STOP 0xFF

extern uint32_t melody[];

extern int melody_length;

typedef struct {
    char note_char;
    float frequency;
} NoteFrequencyMap;


// Initializes all FreeRTOS objects for the music system
// (tasks + queue). Call from main.c before osKernelStart().
void MusicPlayerTasks_Init(void);

// === FreeRTOS task prototypes ===
void ADCReaderTask(void *argument);
void MusicPlayerTask(void *argument);


#endif /* INC_MUSIC_H_ */
