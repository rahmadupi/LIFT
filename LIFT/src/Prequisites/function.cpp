#include "prequisites.h"
#include "dynamixel.h"

#define FLOOR_COUNT 3

#define PERMITTED 1
#define UP 1
#define DOWN 2

#define OPEN 1
#define CLOSE 0

#define DORMANT 0
#define READY 1
#define MOVING 2

#define KALIBRASI 33.256


/*   ┌——————————————————————————————————————————————————————————————————————————————————————————————┐
     │                                            Functions                                         │
     └——————————————————————————————————————————————————————————————————————————————————————————————┘     */
// Fungsi fitur lift
int elev_pos_bottom_floor_check(){
    //int state=digitalRead(ir_cns_pin_floor);
    //if(state==HIGH) Serial.println("HIGH");
    //flagged tak walek dok var_obj
    if(digitalRead(ir_cns_pin_floor)==LOW) {
        //Serial.println("LOW");
        return 1;
    }
    else return 0;
}

int weight_check(){
    if (sensor_berat.is_ready()) {
        //const int kalibrasi=0;
        sensor_berat.set_scale(KALIBRASI);    
        Serial.println("Tare... remove any weights from the sensor_berat.");
        //delay(100);
        sensor_berat.tare();
        Serial.println("Tare done...");
        Serial.print("Place a known weight on the sensor_berat...");
        //delay(100);
        long reading = sensor_berat.get_units(10);
        //Serial.print("Result: ");
        //Serial.println(reading);
        // if (scale.wait_ready_timeout(200)) {
        // }
        //long reading = round(sensor_berat.get_units(20));
        Serial.print("Weight: ");
        Serial.println(reading); 
        //sensor_berat.power_down();
        //delay(10);
        //sensor_berat.power_up();
        if(reading > 300) return 1;
        else return 0;
    }
    else return 0;
}

int elev_door_ops_interuption(){ //sensor inframerah
    if(digitalRead(ir_cns_pin_door)==LOW){
        Serial.println("interupted");
        return 1;
    }
    else return 0;
}

void elev_door_ops(int open_close){ //0 close 1 Open , int permission
    if(open_close==1){
        playServo(3, 1023, 1500);
        Serial.println("door opened");
    }
    else if(open_close==0){
        playServo(3, 512, 1500);
        Serial.println("door closed");
    }
}

int elev_door_button(){
    //if(digitalRead(buttonopen_inner)==LOW || digitalRead(buttonclose_inner)==LOW || digitalRead(ir_cns_pin_door)==LOW){
    if(elev_door_ops_interuption()) return 2;
    if(digitalRead(buttonopen_inner)==LOW) return 1;
    if(digitalRead(buttonclose_inner)==LOW) return 0;
    else return 3;
    //}
    //else return 3;
}

//===============================================================================================================================
//lift
void sort_on_cycle(int floor_value, int direction, int current_cycle, int query[], int saved[]){
    //simplified
    if(direction != current_cycle){
        saved[floor_value-1]=1;
    }
    else if(direction == current_cycle){
        query[floor_value-1]=1;
    }
}

void on_floor_call(int current_floor, int current_cycle, int query[], int saved[]){
    int floor=0, dir=0;
    //if(digitalRead(button11_outer)==HIGH || digitalRead(button21_outer)==HGIH || digitalRead(button22_outer)==HIGH || digitalRead(button32_outer)==LOW){
    if(digitalRead(button11_outer)==LOW){
        if(current_floor!=1) floor=1; //call dari lantai berapa
        else return;
        //dir=1;   //1=Up, 2==Down
        String x="floor"+String(floor)+"dir"+String(dir);
        Serial.println(x);
    }
    if(digitalRead(button21_outer)==LOW){
        if(current_floor!=2) floor=2; //call dari lantai berapa
        else return;
        //dir=1;   //1=Up, 2==Down
        String x="floor"+String(floor)+"dir"+String(dir);
        Serial.println(x);
    }
    if(digitalRead(button22_outer)==LOW){
        if(current_floor!=2) floor=2; //call dari lantai berapa
        else return;
        //dir=2;   //1=Up, 2==Down
        String x="floor"+String(floor)+"dir"+String(dir);
        Serial.println(x);
    }
    if(digitalRead(button32_outer)==LOW){
        if(current_floor!=3) floor=3; //call dari lantai berapa
        else return;
        //dir=2;   //1=Up, 2==Down
        String x="floor"+String(floor)+"dir"+String(dir);
        Serial.println(x);
    }
    //}
    //flagged aman harus e
    if(current_floor>floor /*&& current_cycle==1*/) dir=2;
    if(current_floor<floor) dir=1;
    sort_on_cycle(floor, dir, current_cycle, query, saved);
}

void on_elev_input(int current_floor, int current_cycle, int query[], int saved[]){
    int floor=0, dir=0;
    //if(digitalRead(button1_inner)==LOW || digitalRead(button2_inner)==LOW || digitalRead(button3_inner)==LOW){
    if(digitalRead(button1_inner)==LOW){
        if(current_floor!=1) floor=1; //input lift untuk ke lantai 1
        else return;
        String x="floor"+String(floor)+"dir"+String(dir);
        Serial.println(x);
    }
    else if(digitalRead(button2_inner)==LOW){
        if(current_floor!=2) floor=2; //input lift untuk ke lantai 2
        else return;
        String x="floor"+String(floor)+"dir"+String(dir);
        Serial.println(x);
    }
    else if(digitalRead(button3_inner)==LOW){
        if(current_floor!=3) floor=3; //input lift untuk ke lantai 3
        else return;
        String x="floor"+String(floor)+"dir"+String(dir);
        Serial.println(x);
    }
    //}
    /*
    1 munggah
    2 mudun  
    */
    if(current_floor>floor /*&& current_cycle==1*/) dir=2;
    if(current_floor<floor) dir=1;
    
    sort_on_cycle(floor, dir, current_cycle, query, saved);
}

int counter=0;
void sort_elev_move_query(int *main_query, int current_cycle, int elev_query[], int floor_query[]){
    //int counter=0;
    int init=0;
    if(current_cycle==UP){
        for(int i=0;i<FLOOR_COUNT;i++){
            if(elev_query[i]==1 || floor_query[i]==1){
                for(int j=0;j<FLOOR_COUNT+1;j++) if((i+1)==main_query[j]) init=1;
                if(init!=1) main_query[counter++]=i+1;
            }
        }
    }
    else if(current_cycle==DOWN){
        for(int i=FLOOR_COUNT-1;i>=0;i--){
            if(elev_query[i]==1 || floor_query[i]==1){
                for(int j=0;j<FLOOR_COUNT+1;j++) if((i+1)==main_query[j]) init=1;
                if(init!=1) main_query[counter++]=i+1;
            } 
        }
    }
    main_query[counter]=0;
    //Serial.println("atas "+String(main_query[0])+" "+String(main_query[1])+" "+String(main_query[2])+"    Cycle:"+String(current_cycle););

    //sort sesuai nilai
    if(current_cycle==UP){
        for(int i=0;i<FLOOR_COUNT-1;i++){ //only used for array more than 3 items long
            for(int i=0;i<FLOOR_COUNT-1;i++){
                if(main_query[i] > main_query[i+1] && main_query[i+1]!=0){
                    int temp=main_query[i];
                    main_query[i]=main_query[i+1];
                    main_query[i+1]=temp;
                }
            }
        }
    }
    else if(current_cycle==DOWN){ //tolol ultra
        for(int i=0;i<FLOOR_COUNT-1;i++){ //only used for array more than 3 items long
            for(int i=0;i<FLOOR_COUNT-1;i++){
                if(main_query[i] < main_query[i+1] && main_query[i+1]!=0){
                    int temp=main_query[i];
                    main_query[i]=main_query[i+1];
                    main_query[i+1]=temp;
                }
            }
        }
    }
    // Serial.println("bawah "+String(main_query[0])+" "+String(main_query[1])+" "+String(main_query[2])+"    Cycle:"+String(current_cycle););
}

int elev_begin_passed=0;
void elev_move_start(int current_floor, int current_cycle, int main_query[], int time_sheet[], int *lift_status, unsigned long long *elev_millis, unsigned long long *door_millis, int *door_open_permission, int *door_status, int existing_order){ // elev move start
    
    const int begin_elev_move=5000; //8 detik untuk menunggu input dari dalam lift
    const int door_permission_closed=2000; //3 detik untuk menutup pintu lift____ruang untuk interupsi

    //Serial.println( "Current "+String(main_query[0]));
    //flagged
    // if(weight_check()==1){
    //     //digitalWrite(buzzer_pin, LOW);
    //     Serial.println("Overload");
    //     // while(1){
    //     //     tone(buzzer_pin, 1000);
    //     //     Serial.println("Overload");
    //     //     if(weight_check()!=1) break;
    //     // }
    //     return;
    // }
    //else return;//digitalWrite(buzzer_pin, HIGH);
    //==========================================================
    unsigned long long current_millis=millis(); //current millis

    if(*lift_status==0) {
        *lift_status=1;
        *elev_millis=millis();
    }
    int just_begin=0; //value untuk bypass waktu tunggu lift
    
    //flagged masalah flow
    if(elev_door_button()==CLOSE) just_begin=1; //menutup langsung pintu lift
    else if(elev_door_button()==OPEN /*interupsi*/) *elev_millis=millis(); //interupsi perhitungan default lift

    if(existing_order){
        int check=current_millis-(*elev_millis);

        if((current_millis-(*elev_millis)>=begin_elev_move) || just_begin!=0 || elev_begin_passed==1){
            elev_begin_passed=1;
            if(elev_begin_passed!=1)*door_millis=millis();
            //close door functon
            if(*door_open_permission == PERMITTED){
                elev_door_ops(0); //menutup pintu lift
                *door_millis=millis();
                *door_open_permission=0; //OPEN NOT PERMITTED
            }
            
            if(*door_status==OPEN){
                if(elev_door_button()==1 || elev_door_button()==2){
                    elev_door_ops(1); //menutup pintu lift
                    *door_millis=millis();
                    *door_open_permission=1;
                    return;
                }
                int check1=current_millis-(*door_millis);
                Serial.println("Door Current "+String(check1)+" must stop at "+String(door_permission_closed));
                if(current_millis-(*door_millis)>= door_permission_closed){
                    *door_status=0; //CLOSE
                    *door_open_permission=0; // NOT PERMITTED
                }
                else return;
            }
            
            //begin elev move
            *lift_status=2; //MOVING
            elev_begin_passed=0;
            Serial.println("elevator begin move to "+String(main_query[0])+" Value might change due to continous input");
            *elev_millis=millis();
            if(current_cycle==UP){
                wheelMode(1,1340);
                wheelMode(2, 1000);
                setLED(1, 1);
                setLED(2, 1);
            }
            else if(current_cycle==DOWN){
                wheelMode(1,300);
                wheelMode(2, 2000);
                setLED(1, 2);
                setLED(2, 2);
            }
        }
        else Serial.println("Elev begin Current "+String(check)+" must stop at "+String(begin_elev_move));
    }
    else return;
}

//flagged
void remove_query_order(int *query){
    counter--;
    for(int i=0;i<4;i++) query[i]=query[i+1];
    query[3]=0;
    Serial.println("query removed"+String(query[0]));
}


int on_move_completion(int main_query[], int time_sheet[], int *lift_status, int current_cycle, int *current_floor, unsigned long long *elev_millis, int *door_open_permission, int *door_status){ // elev move stop
    unsigned long long current_millis=millis();
    int floor_destination=main_query[0];
    int elev_stop_time=0;
    if(current_cycle==UP){
        for(int i=(*current_floor)-1;i<floor_destination-1;i++) elev_stop_time+=time_sheet[i];
    }
    else if(current_cycle==DOWN){
        for(int i=(*current_floor)-1;i>floor_destination-1;i--) elev_stop_time+=time_sheet[i-1];
    }
    //to be optimized -- leaving only logic and moving the process to another function

    int check=current_millis-(*elev_millis);
    Serial.println("Elevator Current "+String(check)+" must stop at "+String(elev_stop_time) + " Current cycle: "+String(current_cycle) +" Dest: "+ String(floor_destination));
    //stop lift
    if(current_millis-(*elev_millis)>=elev_stop_time){
        *elev_millis=0;
        *lift_status=0;
        *current_floor=main_query[0];
        wheelMode(1, 0);
        wheelMode(2, 0);
        //flagged
        remove_query_order(main_query);
        //Serial.println(main_query[0]);
        //for(int i=0;i<4;i++) main_query[i]=main_query[i+1];
        Serial.println(main_query[0]);
        Serial.println("elevator Stop Moving");

        //open door
        elev_door_ops(1);
        *door_status=1;
        *door_open_permission=1;
        return main_query[0];
    }
    else return 0;
    
} 


int check_on_query(int main_query[], int *existing_order){
    if(main_query[0]==0){
        *existing_order=0;
        return 0;
    }
    else{
        *existing_order=1;
        return 1;
    }
}

void on_cycle_end(int *current_cycle, int *main_query, int *elev_query, int *floor_query, int *elev_saved, int *floor_saved){
    //memindah nilai dari saved ke query
    int *query[]={elev_query, floor_query};
    int *saved[]={elev_saved, floor_saved};
    
    for(int i=0;i<2;i++){
        for(int j=0;j<3;j++){
            query[i][j]=saved[i][j];
            saved[i][j]=0;
        }
        //saved[i][3]=0;
        saved[i][2]=0;
    }
    sort_elev_move_query(main_query, *current_cycle, query[0], query[1]);

    //merubah current_cycle
    if(*current_cycle==UP) *current_cycle=2;
    else if(*current_cycle==DOWN) *current_cycle=1;
    //Serial.println("Cycle changed");
}

void reset_elev_pos(int *start){
    //drop elevator to ground floor
    Serial.println("Elevator dropping, Waiting for the censor input");
    //flagged
    //tone(buzzer_pin, 1000);
    wheelMode(1,300);
    wheelMode(2, 2000);
    setLED(1, 2);
    setLED(2, 2);
    while(1){ 
        //wait
        Serial.println("Elevator dropping");
        if(elev_pos_bottom_floor_check()){
            Serial.println("Censor triggered, Elevator at bottom floor");
            //noTone(buzzer_pin);
            wheelMode(1, 0);
            wheelMode(2, 0);
            *start=0;
            break;
        } 
    }
}


/*   ┌——————————————————————————————————————————————————————————————————————————————————————————————┐
     │                                  Unused/Removed Functions                                    │
     └——————————————————————————————————————————————————————————————————————————————————————————————┘     */

// void sort_list(int list[]){}
// int sort_floor_direction(){}
// int sort_elev_direction(){}
//int elev_floor_combined(){}
//int elev_move_command(){}