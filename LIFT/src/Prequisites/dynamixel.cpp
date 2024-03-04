#include "dynamixel.h"

// Deklarasi ID dari masing-masing servo
const uint8_t XL320_ID[XL320_ID_CNT] = {1, 2, 3};  // Updated ID

uint8_t xl_user_buffer[BUFFER_SIZE];

// Deklarasi object untuk channel komunikasi dynamixel
Dynamixel2Arduino XL(Serial1, XL320_DIR_PIN);

// Informasi packet data XL320 yang akan di-READ
xl_read_data_t xl_read_data[XL320_ID_CNT];
DYNAMIXEL::InfoSyncReadInst_t xl_read_infos;
DYNAMIXEL::XELInfoSyncRead_t xl_xels_read[XL320_ID_CNT];

// Informasi packet data XL320 yang akan di-WRITE
xl_write_data_t xl_write_data[XL320_ID_CNT];
DYNAMIXEL::InfoSyncWriteInst_t xl_write_infos;
DYNAMIXEL::XELInfoSyncWrite_t xl_xels_write[XL320_ID_CNT];

TaskHandle_t xl_task_handle;

void initDynamixelChannel(Dynamixel2Arduino *dxl, HardwareSerial *serial, int baudrate, float protocol_version) {
    serial->begin(baudrate, SERIAL_8N1, UART_1_RX, UART_1_TX);

    dxl->setPortProtocolVersion(protocol_version);
    dxl->begin(baudrate);
}

void initDynamixel() {
    initDynamixelChannel(&XL, &Serial1, 1000000, 2.0);
    initSyncRead();
    initSyncWrite();
    ReadXL320_Joint();
}

void initSyncWrite() {
    uint8_t index;
    xl_write_infos.packet.p_buf = nullptr;
    xl_write_infos.packet.is_completed = false;
    xl_write_infos.addr = XL320_WRITE_START_ADDR;
    xl_write_infos.addr_length = XL320_WRITE_ADDR_LEN;
    xl_write_infos.p_xels = xl_xels_write;
    xl_write_infos.xel_count = 0;

    for (index = 0; index < XL320_ID_CNT; index++) {
        xl_xels_write[index].id = XL320_ID[index];
        xl_xels_write[index].p_data = (uint8_t *)&xl_write_data[index].goalPos;
        xl_write_infos.xel_count++;
    }
    xl_write_infos.is_info_changed = true;
}

void initSyncRead() {
    uint8_t index = 0;

    xl_read_infos.packet.p_buf = xl_user_buffer;
    xl_read_infos.packet.buf_capacity = BUFFER_SIZE;
    xl_read_infos.packet.is_completed = false;
    xl_read_infos.addr = XL320_READ_START_ADDR;
    xl_read_infos.addr_length = XL320_READ_ADDR_LEN;
    xl_read_infos.p_xels = xl_xels_read;
    xl_read_infos.xel_count = 0;

    for (index = 0; index < XL320_ID_CNT; index++) {
        xl_xels_read[index].id = XL320_ID[index];
        xl_xels_read[index].p_recv_buf = (uint8_t *)&xl_read_data[index];
        xl_read_infos.xel_count++;
    }
    xl_read_infos.is_info_changed = true;
}

void XL_readPresentPos() {
    XL.syncRead(&xl_read_infos);
}

void ReadXL320_Joint() {
    xTaskCreatePinnedToCore(
        XL_InitTask, /* Function to implement the task */
        "Init XL",   /* Name of the task */
        2000,        /* Stack size in words */
        NULL,        /* Task input parameter */
        2,           /* Priority of the task */
        NULL,        /* Task handle. */
        1);          /* Core where the task should run */
}