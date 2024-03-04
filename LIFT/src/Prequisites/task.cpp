#include "dynamixel.h"

uint16_t xl_previous_goal_position[XL320_ID_CNT + 1] = {0};

void playServo(uint8_t servo_idx, int degree, int duration) {
    servo_idx--;
    xl_write_data[servo_idx].goalPos = degree;
    xl_write_data[servo_idx].goalVel = degree * 500 / duration;
    xl_write_infos.is_info_changed = true;
    XL.syncWrite(&xl_write_infos);
}

void onOffTorque(uint8_t servo_idx, uint8_t onoff) {
    XL.writeControlTableItemXL(TORQUE_ENABLE, servo_idx, onoff);
}

void setLED(uint8_t servo_idx, uint8_t led) {
    XL.writeControlTableItemXL(LED, servo_idx, led);
}

void wheelMode(uint8_t servo_idx, int speed) {
    XL.writeControlTableItemXL(MOVING_SPEED, servo_idx, speed);
}

void XL_InitTask(void *parameter) {
    int index = 0;
    int total_joint = 0;

    for (index = 0; index < XL320_ID_CNT; index++) {
        XL.writeControlTableItemXL(TORQUE_ENABLE, XL320_ID[index], 1);
        if (XL.writeControlTableItemXL(LED, XL320_ID[index], random(1, 8)))
            total_joint++;
        else
            DEBUG_PRINTF("[XL320] ID %d is not connected\n", XL320_ID[index]);
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }

    DEBUG_PRINTF("[XL320] Joint: %d\n", total_joint);
    vTaskDelete(NULL);
}