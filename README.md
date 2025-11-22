# STM32 NUCLEO-L476RG – UART-Controlled Music Player

A simple embedded **music player** running on the **STM32 NUCLEO-L476RG** board.
The project plays melodies on a passive buzzer by generating a **PWM signal**, with note frequencies and timing controlled externally through UART and ADC.

---

## 🎶 Overview

* Notes are **sent from a PC via UART**
* A **PWM signal drives a buzzer**, and its frequency determines the musical note
* A **potentiometer connected to the ADC** sets the playback speed
* The **PC receives ADC values** and dynamically adjusts when each note is sent

---

## 🧠 How It Works

### MCU Responsibilities

* Sample potentiometer value using **ADC**
* Send ADC readings to PC over **UART**
* Receive note values from PC via UART
* Convert note into PWM frequency

### PC Client

* Written in Python
* Reads ADC feedback from UART
* Adjusts transmission timing of notes based on playback speed
* Sends next note to the MCU in sync

---

## 🧰 Peripherals Used

| Peripheral      | Usage                                         |
| --------------- | --------------------------------------------- |
| **ADC**         | Reads potentiometer to compute playback speed |
| **UART**        | Bi-directional MCU ↔ PC communication         |
| **Timer (PWM)** | Generates buzzer audio output                 |

---

## 📂 Project Structure

```
/l476rg_mcu_code      → Most important files of the MCU firmware
/pc_py_serial_com     → Python UART script and sound recording for analysis
```

