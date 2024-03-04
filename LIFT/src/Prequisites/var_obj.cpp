#include "prequisites.h"

/*   ┌——————————————————————————————————————————————————————————————————————————————————————————————┐
     │                                      Object and Pins                                         │
     └——————————————————————————————————————————————————————————————————————————————————————————————┘     */

//flagged
/*objek*/
HX711 sensor_berat;

// int counter=0;

/* buzzer, led*/
const int buzzer_pin=5; //pin buzzer
const int led1_pin=0; //pin led
const int led2_pin=0; //pin led
const int led3_pin=0; //pin led

/* IR, Weight*/
const int weight_cns_pin_dout=16;
const int weight_cns_pin_sck=4;
const int ir_cns_pin_door=23;
const int ir_cns_pin_floor=34;

/* PushButton */
/*dalam lift*/
const int buttonopen_inner=32;  //pin button membuka pintu lift
const int buttonclose_inner=35; // pin button menutup pintu lift
const int button1_inner=26;     //pin button ke lantai 1
const int button2_inner=25;     //pin button ke lantai 2
const int button3_inner=33;     //pin button ke lantai 3

/*setiap lantai*/
const int button11_outer=13;    //pin button lantai 1 - ke atas
//const int button12_outer=0;   //pin button lantai 1 - bawah

const int button21_outer=14;    //pin button lantai 2 - ke atas
const int button22_outer=12;    //pin button lantai 2 - ke bawah

//const int button31_outer=0;   //pin button lantai 3 - ke atas
const int button32_outer=27;    //pin button lantai 3 - ke bawah



