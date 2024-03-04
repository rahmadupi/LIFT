# Template Dynamixel XL-330
## Mode
### Set LED
```cpp
void setLED(int id, int led);
```
Keterangan :
- id : ID Servo yang ingin diset LED 
  - BROADCAST_ID : Semua Servo
  - ID Servo : ID Servo yang ingin diset LED
- led : 0 (Off) atau 1-7 (On dengan warna tertentu)

### On Off Torque
```cpp
void onOffTorque(int id, int onOff);
```
Keterangan :
- id : ID Servo yang ingin diaktifkan atau dinonaktifkan
  - BROADCAST_ID : Semua Servo
  - ID Servo : ID Servo yang ingin diaktifkan atau dinonaktifkan
- onOff : 0 (Off) atau 1 (On)

### Joint Mode (0-150 derajat positif dan negatif)
```cpp
void playServo(int id, int degree, int duration);
```
Keterangan :
- id : ID Servo yang ingin diputar
  - BROADCAST_ID : Semua Servo
  - ID Servo : ID Servo yang ingin diputar
- degree : 0-1023 (mapping ke 0-300 derajat)
- duration : dalam milidetik

### Wheel Mode (Putar 360 derajat)
```cpp
void wheelMode(int id, int speed);
```
Keterangan :
- id : ID Servo yang ingin diputar
  - BROADCAST_ID : Semua Servo
  - ID Servo : ID Servo yang ingin diputar
- speed : 
  - 0-1023 (Gerak searah jarum jam) 
  - 1024-2047 (Gerak berlawanan arah jarum jam)
