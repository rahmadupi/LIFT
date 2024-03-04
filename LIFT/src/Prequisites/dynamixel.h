#pragma once
#include <Arduino.h>
#include <Dynamixel2Arduino.h>

/* Namespace ini diperlukan untuk menggunakan definisi nama item tabel kontrol DYNAMIXEL */
using namespace ControlTableItem;

/*   ┌——————————————————————————————————————————————————————————————————————————————————————————————┐
     │                                        Definitions                                           │
     └——————————————————————————————————————————————————————————————————————————————————————————————┘     */

/* Debugging */
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)

/* Jumlah Servo */
#define XL320_ID_CNT 3
#define XL320_TIME 3

/* Pin */
#define UART_1_RX 19
#define UART_1_TX 18
#define XL320_DIR_PIN 15

/* Configurasi Alamat Write dan Read */
#define XL320_READ_START_ADDR 37
#define XL320_READ_ADDR_LEN 2
#define XL320_WRITE_START_ADDR 30
#define XL320_WRITE_ADDR_LEN 4

/* Utils */
#define BUFFER_SIZE 256
#define BROADCAST_ID 0xFE

/*   ┌——————————————————————————————————————————————————————————————————————————————————————————————┐
     │                                          Structure                                           │
     └——————————————————————————————————————————————————————————————————————————————————————————————┘     */

typedef struct xl_read_data {
    uint16_t presentPos;                   // Present Position (2 bytes)
} __attribute__((packed)) xl_read_data_t;  // Attribute yang dibaca dari XL320 [ID]

typedef struct xl_write_data {
    uint16_t goalPos;                       // Goal Position (2 bytes)
    uint16_t goalVel;                       // Goal Velocity (2 bytes)
} __attribute__((packed)) xl_write_data_t;  // Attribute yang akan ditulis ke XL320 [ID]

extern const uint16_t XL_MOTION_DATA[][4];
extern const uint8_t XL_MOTION_FRAME[];

extern int idx;

/*   ┌——————————————————————————————————————————————————————————————————————————————————————————————┐
     │                                     External Variables                                       │
     └——————————————————————————————————————————————————————————————————————————————————————————————┘     */

/* Channel Dynamixel */
extern Dynamixel2Arduino XL;  // Channel untuk XL320

/* XL320 */
extern uint8_t xl_user_buffer[BUFFER_SIZE];                        // Buffer untuk paket yang dikirimkan ke XL320
extern const uint8_t XL320_ID[XL320_ID_CNT];                       // Daftar ID dari semua XL320 yang terhubung
extern xl_read_data_t xl_read_data[XL320_ID_CNT];                  // Data yang akan dibaca dari XL320 [ID]
extern DYNAMIXEL::InfoSyncReadInst_t xl_read_infos;                // Informasi alamat data yang akan dibaca
extern DYNAMIXEL::XELInfoSyncRead_t xl_xels_read[XL320_ID_CNT];    // Informasi ID dari XL320 yang akan dibaca
extern xl_write_data_t xl_write_data[XL320_ID_CNT];                // Data yang akan ditulis ke XL320 [ID]
extern DYNAMIXEL::InfoSyncWriteInst_t xl_write_infos;              // Informasi alamat data yang akan ditulis
extern DYNAMIXEL::XELInfoSyncWrite_t xl_xels_write[XL320_ID_CNT];  // Informasi ID dari XL320 yang akan ditulis

/* FreeRTOS */
extern TaskHandle_t xl_task_handle;  // Task handle untuk XL320

/*   ┌——————————————————————————————————————————————————————————————————————————————————————————————┐
     │                                      Functions Prototype                                     │
     └——————————————————————————————————————————————————————————————————————————————————————————————┘     */

/**
 * @brief Inisialisasi Channel Dynamixel
 * @param dxl Channel Dynamixel
 * @param serial Serial yang digunakan untuk komunikasi
 * @param baudrate Baudrate yang digunakan untuk komunikasi
 * @param protocol_version Versi protokol yang digunakan untuk komunikasi
 *
 */
void initDynamixelChannel(Dynamixel2Arduino &dxl, HardwareSerial &serial, int baudrate, float protocol_version);

void initDynamixel();

/**
 * @brief Inisialisasi SyncRead untuk buffer yang akan dibaca dari XL320
 * @see https://emanual.robotis.com/docs/en/dxl/protocol2/#sync-read-0x82
 */
void initSyncRead();

/**
 * @brief Inisialisasi SyncWrite untuk buffer yang akan ditulis ke XL320
 * @see https://emanual.robotis.com/docs/en/dxl/protocol2/#sync-write-0x83
 */
void initSyncWrite();

/**
 * @brief Membaca dan menyimpan data dari XL320 ke struct xl_read_data
 */
void XL_ReadPresentPos();

/*   ┌——————————————————————————————————————————————————————————————————————————————————————————————┐
     │                                         Task Prototype                                       │
     └——————————————————————————————————————————————————————————————————————————————————————————————┘     */

/**
 * @brief Task untuk inisialisasi XL320
 */
void XL_InitTask(void *parameter);

/**
 * @brief Task untuk menjalan motion XL320
 */
void XL_MotionTask(void *parameter);

/**
 * @brief Task untuk membaca data dari XL320
 */
void ReadXL320_Joint();


/*   ┌——————————————————————————————————————————————————————————————————————————————————————————————┐
     │                                      Inline Functions                                       │
     └——————————————————————————————————————————————————————————————————————————————————————————————┘     */

/**
 * @brief Memainkan servo dengan ID, degree, dan durasi yang ditentukan
 * @param servo_idx ID servo
 * @param degree Derajat yang diinginkan
 * @param duration Durasi pergerakan servo
 */
void playServo(uint8_t servo_idx, int degree, int duration);

/**
 * @brief Menghidupkan atau mematikan torque pada servo
 * @param servo_idx ID servo
 * @param onoff 1 untuk menyalakan, 0 untuk mematikan
 */
void onOffTorque(uint8_t servo_idx, uint8_t onoff);

/**
 * @brief Mengatur LED pada servo
 * @param servo_idx ID servo
 * @param led 1-7 untuk mengatur LED
 */
void setLED(uint8_t servo_idx, uint8_t led);

/**
 * @brief Mengatur mode wheel pada servo
 * @param servo_idx ID servo
 * @param speed Kecepatan putaran servo (0-1023 untuk gerak CCW, 1024-2047 untuk gerak CW)
 */
void wheelMode(uint8_t servo_idx, int speed);