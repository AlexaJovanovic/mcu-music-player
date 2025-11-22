import serial
import threading
import time

ser_com = serial.Serial()
ser_com.baudrate = 115200
ser_com.port = 'COM7'
ser_com.open()

# Define note codes
NOTE_MAP = {
    "C4": 0,
    "CS4": 1,
    "D4": 2,
    "DS4": 3,
    "E4": 4,
    "F4": 5,
    "FS4": 6,
    "G4": 7,
    "GS4": 8,
    "A4": 9,
    "AS4": 10,
    "B4": 11,
    "C5": 12,
    "CS5": 13,
    "D5": 14,
    "DS5": 15,
    "E5": 16,
    "F5": 17,
    
    "STOP": 100
}

NOTE_STOP = 100

# Melody as (note, duration_in_beats)
# duration = multiplier of base_duration
MELODY_1 = [
    ("A4", 1), ("STOP", 0.5), ("B4", 1), ("A4", 1), ("G4", 1),
    ("FS4", 1), ("G4", 1), ("A4", 1), ("STOP", 0.5), ("E4", 1),
    ("FS4", 1), ("G4", 1), ("FS4", 1), ("G4", 1),
    ("A4", 1), ("STOP", 0.5), ("A4", 1),
    ("B4", 1), ("A4", 1), ("G4", 1), ("FS4", 1),
    ("G4", 1), ("A4", 1),
    ("E4", 1), ("A4", 1), ("FS4", 1), ("D4", 1),
    ("STOP", 1)
]

MELODY_2 = [
    ("E4", 1), ("E4", 1), ("F4", 1), ("G4", 1),
    ("G4", 1), ("F4", 1), ("E4", 1), ("D4", 1),
    ("C4", 1), ("C4", 1), ("D4", 1), ("E4", 1),
    ("E4", 1.5), ("D4", 0.5), ("D4", 2),
    ("E4", 1), ("E4", 1), ("F4", 1), ("G4", 1),
    ("G4", 1), ("F4", 1), ("E4", 1), ("D4", 1),
    ("C4", 1), ("C4", 1), ("D4", 1), ("E4", 1),
    ("E4", 1.5), ("D4", 0.5), ("D4", 2),
    ("D4", 1), ("D4", 1), ("E4", 1), ("C4", 1),
    ("D4", 1), ("E4", 0.5), ("F4", 0.5), ("E4", 1),
    ("C4", 1), ("D4", 1), ("E4", 0.5), ("F4", 0.5), ("E4", 1),
    ("D4", 1), ("G3", 1),
    ("E4", 1), ("E4", 1), ("F4", 1), ("G4", 1),
    ("G4", 1), ("F4", 1), ("E4", 1), ("D4", 1),
    ("C4", 1), ("C4", 1), ("D4", 1), ("E4", 1),
    ("D4", 1.5), ("C4", 0.5), ("C4", 2),
    
    ("STOP", 1)
]

MELODY_3 = [
    ("D4", 1),
    ("G4", 1), ("G4", 0.5), ("A4", 0.5), ("G4", 0.5), ("FS4", 0.5),
    ("E4", 1), ("E4", 1), ("E4", 1),
    ("A4", 1), ("A4", 0.5), ("B4", 0.5), ("A4", 0.5), ("G4", 0.5),
    ("FS4", 1), ("D4", 1), ("D4", 1),
    ("B4", 1), ("B4", 0.5), ("C5", 0.5), ("B4", 0.5), ("A4", 0.5),
    ("G4", 1), ("E4", 1), ("D4", 0.5), ("D4", 0.5),
    ("E4", 1), ("A4", 1), ("FS4", 1),
    
    ("G4", 1.5),

    ("STOP", 1)
]

MELODY_4 = [
    ("A4", 1),
    ("E5", 0.5), ("E5", 0.5), ("E5", 1), ("E5", 0.5), ("D5", 0.3), ("E5", 0.5),
    ("F5", 0.5), ("E5", 0.5), ("D5", 0.5), ("E5", 0.5),("D5", 0.5), ("E5", 0.5),
    ("D5", 0.5), ("C5", 0.5), ("D5", 0.5),

    ("STOP", 1)
]

MELODY_5 = [
    ("E4", 0.5), ("FS4", 0.5), ("G4", 1), ("A4", 1),
    ("B4", 0.5), ("C5", 0.5), ("B4", 1), ("A4", 1),
    ("G4", 1), ("FS4", 1), ("E4", 1), ("D4", 1),
    ("E4", 1), ("FS4", 0.5), ("G4", 0.5), ("A4", 1),
    ("B4", 1), ("A4", 0.5), ("G4", 0.5), ("FS4", 1),
    ("E4", 2), ("G4", 1), ("FS4", 1), ("E4", 1),
    ("D4", 1), ("E4", 2), ("G4", 1), ("A4", 1),
    ("B4", 0.5), ("C5", 0.5), ("B4", 1), ("A4", 1),
    ("G4", 1), ("FS4", 1), ("E4", 2), ("G4", 1),
    ("A4", 1), ("B4", 0.5), ("C5", 0.5), ("B4", 1),
    ("A4", 1), ("G4", 2), ("FS4", 1), ("E4", 2),
    ("D4", 1), ("E4", 1), ("FS4", 1), ("E4", 2)
]

MELODY_6 = [
    ("C4", 1), 
    ("D4", 1),
    ("E4", 1),
    ("F4", 1),
    ("G4", 1), 
    ("A4", 1),
    ("B4", 1),
    ("C5", 1),
    ("C5", 1), 
    ("H4", 1),
    ("A4", 1),
    ("G4", 1),
    ("F4", 1), 
    ("E4", 1),
    ("D4", 1),
    ("C4", 1)
]


# Shared variable
duration_factor = 1.0
lock = threading.Lock()
stop_event = threading.Event()

NOTE_PAUSE = 0.08

# Task that recieves ADC values from MCU and adjusts the playing speed
def read_from_port(ser):
    global duration_factor
    while not stop_event.is_set():
        if ser.in_waiting > 0:
            line = ser.readline().decode(errors="ignore").strip()
            if line:
                try:
                    value = int(line)  # integer 0–200
                    factor = 1.0 + (value / 200.0)  # map 0→1.0, 200→2.0
                    with lock:
                        duration_factor = factor
                    print(f"[RX] {value} → duration factor {factor:.2f}")
                except ValueError:
                    print(f"[RX] Invalid data: {line}")
        time.sleep(0.05)

def play_note(ser, note, beats, base_duration=0.5):
    """Play a single note for given beats and adjust it based on ADC values."""
    with lock:
        factor = duration_factor
    duration = base_duration * beats * factor

    # Send note code
    code = NOTE_MAP.get(note, NOTE_MAP["STOP"])
    ser.write(bytes([code]))

    # Logging
    print(f"[TX] Sent: {note} (code {code}), duration {duration:.2f}s")
    time.sleep(duration - NOTE_PAUSE)

    # Send stop note - pause before playing next note
    ser.write(bytes([NOTE_STOP]))
    time.sleep(NOTE_PAUSE)

# Worker task that recives a melody and trasmits it to the MCU
def melody_worker(ser, melody, base_duration):
    for note, beats in melody:
        if stop_event.is_set():
            break
        play_note(ser, note, beats, base_duration)
    ser.write(bytes([NOTE_STOP]))  # ensure stopped

# Start a worker task that sends the melody notes and wait for it to finish
def play_melody(ser, melody, base_duration=0.5):
    """Play a melody and wait until finished."""
    t = threading.Thread(target=melody_worker, args=(ser, melody, base_duration))
    t.start()
    t.join()

# Create thread that recieves ADC values 
read_thread = threading.Thread(target=read_from_port, args=(ser_com,), daemon=True)
read_thread.start()


print("Playing melody 3...")

play_melody(ser_com, MELODY_3)

# Stop any running threads
stop_event.set()

read_thread.join()