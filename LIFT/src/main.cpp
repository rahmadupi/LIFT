#include "Prequisites/dynamixel.h"
/*Fungsi dan deklarasi objek*/
#include "Prequisites/prequisites.h"

#define ull unsigned long long
#define DORMANT 0
#define READY 1
#define MOVING 2

#define SCREEN_WIDTH 128                                        // OLED width,  in pixels
#define SCREEN_HEIGHT 64                                        // 0OLED height, in pixels
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);  // create an OLED display object connected to I2C

/*   ┌——————————————————————————————————————————————————————————————————————————————————————————————┐
     │                             Variable Declaration and Setup                                   │
     └——————————————————————————————————————————————————————————————————————————————————————————————┘     */


int start=1;

/* variable global untuk algoritma lift*/ //nilai 0 default
int floor_call_query[5]={0,0,0}; //wadah untuk input call dari lantai yang searah dengan cycle lift
int floor_call_saved[5]={0,0,0}; //wadah untuk input call dari lantai yang tidak searah dengan cycle lift
int elev_input_query[5]={0,0,0}; //wadah untuk input call dari lift yang searah dengan cycle lift
int elev_input_saved[5]={0,0,0}; //wadah untuk input call dari lift yang tidak searah dengan cycle lift
int elev_move_query[5]={0,0,0,0,0}; // wadah akhir value untuk diproses lift

//flagged
int time_sheet[3]={3000, 3000}; //waktu perpindahan lift dari setiap lantai
//int time_sheet[3]={7000, 7000};

int current_cycle=1; //arah lift 1 munggah 2 mudun
int current_floor=1; //penanda posisi lift

int lift_status=0; //kondisi lift apakah sedang bergerak atau tidak 1 bergerak 0 tidak 

int existing_order=0; //check adakah call atau perintah untuk lift

int door_open_permission=1; //izin untuk membuka pintu lift 0 terkunci 1 diizinkan
int door_status=1; //kondisi pintu lift 1 untuk terbuka 0 untuk tertutup

ull elev_door_millis=millis();
ull elev_move_millis=millis();


void setup(){
    Serial.begin(115200);
    //rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);
    if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {          // inisialisasi OLED display with I2C address 0x3C
        Serial.println(F("failed to start SSD1306 OLED"));  // Handler
    }
    oled.clearDisplay();       // clear display
    oled.setTextSize(4);       // set text size
    oled.setTextColor(WHITE);  // set text color
    oled.setCursor(0, 0);      // set position to display (x,y)

    initDynamixel(); //inisialisasi Servo 
    //DEBUG_PRINTF("[SUB-Micro] Ready for Serial\n");

    sensor_berat.begin(weight_cns_pin_dout, weight_cns_pin_sck);

    //flagged
    //pin setup ir sensor
    pinMode(ir_cns_pin_floor, INPUT_PULLUP);
    pinMode(ir_cns_pin_door, INPUT_PULLUP);

    //button pin input setup
    pinMode(buttonopen_inner, INPUT_PULLUP);
    pinMode(buttonclose_inner, INPUT_PULLUP);
    pinMode(button1_inner, INPUT_PULLUP);
    pinMode(button2_inner, INPUT_PULLUP);
    pinMode(button3_inner, INPUT_PULLUP);
    
    pinMode(button11_outer, INPUT_PULLUP);
    pinMode(button21_outer, INPUT_PULLUP);
    pinMode(button22_outer, INPUT_PULLUP);
    pinMode(button32_outer, INPUT_PULLUP);

    // pinMode(buzzer_pin, OUTPUT);
    // noTone(buzzer_pin);

    //test
    //onOffTorque(1, 1);
    delay(500);
}

/*   ┌——————————————————————————————————————————————————————————————————————————————————————————————┐
     │                                      Main Process                                            │
     └——————————————————————————————————————————————————————————————————————————————————————————————┘     */

void loop() {
    //flagged while troubleshooting
    //if (start==1 || (digitalRead(button21_outer)==LOW && digitalRead(button22_outer)==LOW)) reset_elev_pos(&start); // mereset posisi lift ke lantai 1
    //if(elev_pos_bottom_floor_check()) Serial.println("elev at bottom floor");
    
    // //check for keyword //flagged in  each file
    //Serial.println("finished");
    //oled
    oled.clearDisplay();
    oled.setCursor(54, 20);
    oled.println(current_floor);
    oled.display();
    //Serial.println(current_floor);
    //oled


    //masalah pas update query - uwes
    //main process
    on_floor_call(current_floor , current_cycle, floor_call_query, floor_call_saved);
    on_elev_input(current_floor, current_cycle, elev_input_query, elev_input_saved);
    sort_elev_move_query(elev_move_query, current_cycle, elev_input_query, floor_call_query);


    if(lift_status!=MOVING) elev_move_start(current_floor, current_cycle, elev_move_query, time_sheet, &lift_status, &elev_move_millis, &elev_door_millis, &door_open_permission, &door_status, existing_order);
    
    //else if(lift_status==MOVING) on_move_completion(elev_move_query, time_sheet, &lift_status, current_cycle, &current_floor, &elev_move_millis, &door_open_permission, &door_status);
    else if(lift_status==MOVING){
        int temp=on_move_completion(elev_move_query, time_sheet, &lift_status, current_cycle, &current_floor, &elev_move_millis, &door_open_permission, &door_status);
        if(temp!=0) elev_move_query[0]=temp;
    }
    //Serial.println("Current Order "+String(elev_move_query[0]));
    
    if(check_on_query(elev_move_query, &existing_order)==0) on_cycle_end(&current_cycle, elev_move_query, elev_input_query, floor_call_query, elev_input_saved, floor_call_saved);

    //test
    // if(start==1 && digitalRead(buttonclose_inner)==LOW){
    //     start=0;
    //     on_cycle_end(&current_cycle, elev_move_query, elev_input_query, floor_call_query, elev_input_saved, floor_call_saved);
    // }
    // if(digitalRead(button21_outer)==LOW){
    //     //playServo(3, 1023, 3000);
    //     wheelMode(1,1340);
    //     wheelMode(2, 1000);
    // }
    // else if(digitalRead(button22_outer)==LOW){
    //     //playServo(3, 512, 3000);
    //     wheelMode(1,300);
    //     wheelMode(2, 2000);
        
    // }
    // else{
    //     wheelMode(2, 0);
    //     wheelMode(1, 0);
    // }
    // wheelMode(2, 0);
    // wheelMode(1, 0);


    //if(digitalRead(ir_cns_pin_floor)==LOW) Serial.println("LOW");
    //else if(digitalRead(ir_cns_pin_floor)==HIGH) noTone(buzzer_pin);
    // if(elev_door_button()==0) Serial.println("close");
    // else if(elev_door_button()==1) Serial.println("open12");
    // else if(elev_door_button()==2) Serial.println("interuption");
    
    //wheelMode(1, 256);
    //wheelMode(2, 1280);
    //playServo(3, random(1, 1023), 1500);
    //current_floor++;
    //delay(1000);
    //if(digitalRead(buttonopen_inner)==LOW) elev_door_ops(0);
    //else if(digitalRead(buttonopen_inner)==LOW) elev_door_ops(1);
}


// Set LED (ID, LED)
//setLED(BROADCAST_ID, 1);

// On Off Torque (ID, On/Off)
//onOffTorque(1, 1);

// Wheel Mode (ID, Speed)
//wheelMode(0, 2000);

// Play Servo (ID, degree, duration)
//playServo(1, random(1, 1023), 1000);