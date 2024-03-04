#pragma once
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "soc/rtc.h"
#include "HX711.h"

/* deklarasi objek, pin */
//extern Adafruit_SSD1306 oled();  // create an OLED display object connected to I2C
extern HX711 sensor_berat;

/* buzzer, led*/
extern const int buzzer_pin; //pin buzzer
extern const int led1_pin;   //pin led
extern const int led2_pin;   //pin led
extern const int led3_pin;   //pin led

/* IR, Weight*/
extern const int weight_cns_pin_dout;
extern const int weight_cns_pin_sck;
extern const int ir_cns_pin_door;
extern const int ir_cns_pin_floor;

/* PushButton */
extern const int buttonopen_inner;  //pin button membuka pintu lift
extern const int buttonclose_inner; // pin button menutup pintu lift
extern const int button1_inner;     //pin button ke lantai 1
extern const int button2_inner;     //pin button ke lantai 2
extern const int button3_inner;     //pin button ke lantai 3

extern const int button11_outer; //pin button lantai 1 - ke atas
//const int button12_outer=0; //pin button lantai 1 - bawah

extern const int button21_outer; //pin button lantai 2 - ke atas
extern const int button22_outer; //pin button lantai 2 - ke bawah

//const int button31_outer=0; //pin button lantai 3 - ke atas
extern const int button32_outer; //pin button lantai 3 - ke bawah

// /*struct untuk call setiap lantai*/
// typedef struct floor_call_st{
//     int floor_id; //nomer lantai
//     int direction; // 1 untuk keatas 2 untuk kebawah
// }floor_call_st;

// typedef struct elev_input_st{
//     int elev_begin; //nomer lantai
//     int elev_destination; // 1 untuk keatas 2 untuk kebawah
// }elev_input_st;

// /*struct untuk call setiap lantai untuk history*/
// extern floor_call_st floor_call[10000]; //unused
// /*struct untuk input dalam lift untuk history*/ 
// extern elev_input_st elev_input[10000]; //unused

/* variable global untuk algoritma lift*/
// extern int floor_call_query[5];
// extern int floor_call_saved[5];
// extern int elev_input_query[5];
// extern int elev_input_saved[5];

// Fungsi fitur lift
/// @brief mengkonfirmasi bahwa lift sekarang berada di lantai 1
/// @return 1 untuk ya dan 0 untuk tidak
int elev_pos_bottom_floor_check();

/// @brief mengecek berat dari dalam lift
/// @return 1 untuk menahan lift 0 untuk continue
int weight_check();

/// @brief mendeteksi interupsi penutupan pintu lift dengan sensor inframerah
/// @return 1 untuk ya 0 untuk tidak
int elev_door_ops_interuption();

/// @brief mebuka tutup pintu lift
/// @param open_close 0 untuk tutup 1 untuk buka
void elev_door_ops(int open_close);

/// @brief mendeteksi input dari tombol buka tutup pintu dari dalam lift
/// @return mereturn nilai 1 jika tombol buka ditekan dan 2 untuk tombol menutup
int elev_door_button();

//===============================================================================================================================
//lift
/// @brief mensortir nilai input sesuai arah tujuannya
/// @param floor_value nilai untuk lantai tujuan
/// @param direction arah tujuan dari panggilan
/// @param current_cycle cycle dari lift atau arah gerak dari lift saat ini
/// @param query array dari variable query
/// @param saved array dari variable saved
void sort_on_cycle(int floor_value, int direction, int current_cycle, int *query, int *saved);

/// @brief mengambil input panggilan lift dari luar lantai
/// @param current_cycle cycle dari lift atau arah gerak dari lift saat ini
/// @param query array dari variable query
/// @param saved array dari variable saved
void on_floor_call(int current_floor, int current_cycle, int query[], int saved[]);

/// @brief mengambil input dari dalam lift
/// @param current_floor 
/// @param current_cycle cycle dari lift atau arah gerak dari lift saat ini
/// @param query array dari variable query
/// @param saved array dari variable saved
void on_elev_input(int current_floor, int current_cycle, int query[], int saved[]);

/// @brief menggabungkan kedua array query kedalam wadah array akhir yang akan digunakan untuk menjalankan lift
/// @param main_query array wadah untuk gabungan dari elev_input_query dan floor_call_query
/// @param current_cycle cycle dari lift atau arah gerak dari lift saat ini
/// @param elev_query array dari variable elev_input_query
/// @param floor_query array dari variable floor_call_query
void sort_elev_move_query(int *main_query, int current_cycle, int elev_query[], int floor_query[]);

/// @brief menginisiasi penjalanan lift
/// @param current_floor posisi lift saat ini
/// @param current_cycle cycle dari lift atau arah gerak dari lift saat ini
/// @param main_query array akhir gabungan dari elev_input_query dan floor_call_query
/// @param time_sheet sheet lama waktu dari perpindahan lift ke setiap lantai //digunakan karena tidak ada sensor untuk mengetahui posisi lift//tidak digunakan
/// @param lift_status status dari lift apakah diam 0, siap 1 atau bergerak 2
/// @param elev_millis nilai waktu eksekusi program, untuk lift
/// @param door_millis nilai waktu eksekusi program, untuk pintu lift
/// @param door_open_permission izin pembukaan pntu lift
/// @param door_status status dari pintu lift terbuka 1 atau tertutup 0
/// @param existing_order check apakah terdapat perintah untuk lift
void elev_move_start(int current_floor, int current_cycle, int main_query[], int time_sheet[], int *lift_status, unsigned long long *elev_millis, unsigned long long *door_millis, int *door_open_permission, int *door_status, int existing_order);

//flagged
/// @brief menghapus nilai di array perintah lift
/// @param query array dari query akhir perintah lift (elev_move_query)
void remove_query_order(int *query);

/// @brief memberhentikan lift setelah lift bergerak beberapa waktu sesuai array time sheet
/// @param main_query array akhir gabungan dari elev_input_query dan floor_call_query
/// @param time_sheet sheet lama waktu dari perpindahan lift ke setiap lantai //digunakan karena tidak ada sensor untuk mengetahui posisi lift//tidak digunakan
/// @param lift_status status dari lift apakah diam 0, siap 1 atau bergerak 2
/// @param current_cycle cycle dari lift atau arah gerak dari lift saat ini
/// @param current_floor posisi lift saat ini
/// @param elev_millis nilai waktu eksekusi program, untuk lift
/// @param door_open_permission izin pembukaan pntu lift
/// @param door_status status dari pintu lift terbuka 1 atau tertutup 0
void on_move_completion(int *main_query, int time_sheet[], int *lift_status, int current_cycle, int *current_floor, unsigned long long *elev_millis, int *door_open_permission, int *door_status);

/// @brief mengecek apakah masih terdapat perintah untuk lift yang belum dijalankan
/// @param main_query array akhir gabungan dari elev_input_query dan floor_call_query
/// @param existing_order variable untuk konfirmasi apakah terdapat perintah untuk saat ini
/// @return 1 untuk terdapat perintah 0 untuk tidak ada
int check_on_query(int main_query[], int *existing_order);

/// @brief dijalankan saat lift berada dipuncak atau dasar lantai untuk merubah arah lift dan mereset nilai nilai array
/// @param current_cycle cycle dari lift atau arah gerak dari lift saat ini
/// @param main_query array akhir gabungan dari elev_input_query dan floor_call_query
/// @param elev_query array dari variable elev_input_query
/// @param floor_query array dari variable floor_call_query
/// @param elev_saved array dari variable elev_input_saved
/// @param floor_saved array dari variable floor_call_saved
void on_cycle_end(int *current_cycle, int *main_query, int *elev_query, int *floor_query, int *elev_saved, int *floor_saved);

/// @brief mereset posisi dari lift ke lantai satu, dijalankan untuk mensinkronisasi posisi lift
/// @param current_floor posisi lift saat ini
void reset_elev_pos(int *current_floor);