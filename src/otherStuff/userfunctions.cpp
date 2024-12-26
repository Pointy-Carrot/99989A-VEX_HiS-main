#include "userfunctions.h"
#include "pros/rtos.hpp"
#include "config.h"





bool ejecting = false;
bool arm_moving = false;
double current_position;
bool auto_started = false;
int selector_value;
int auton_selected = 0;
bool move_load_hooks = false;
bool hooks_reverse = true;
bool first_time_setting_hooks = true;
int delay_amount;
bool allow_score = true;
bool y_pressed = false;
bool b_pressed = false;
bool right_pressed = false;
bool down_pressed = false;
int arm_velocity = 127;
int arm_softstop = -3000;
bool score_red = false;
bool score_blue = false;
bool intake_running = false;
bool descore_mech_activated = false;
float arm_pid_error = 0;
bool loading_pos_activated = false;
float arm_pid_output = 0;
int arm_state_num = 1;


ColorSort colorsort = NOSORT;
Color alliance = BLUE;
ArmState arm_state = DOWN;

void alliance_red(){
    alliance = RED;
}

void alliance_blue(){
    alliance = BLUE;
}

void eject_ring(){
    hooks.move(-127);
    pros::delay(150);
    hooks.move(127);
}

void deactivate_sorter(){
    colorsort = NOSORT;
}
void activate_sorter(){
    if(alliance == RED){
        colorsort = SCORERED;
    } else if(alliance == BLUE){
        colorsort = SCOREBLUE;
    }
}

bool detect_red(){
    if(sorter.get_proximity() > 100){
        if((sorter.get_hue() > 340 || sorter.get_hue() < 30)){
            return true;
        }
    }
}

bool detect_blue(){
    if(sorter.get_proximity() > 100){
        if((sorter.get_hue() > 180 && sorter.get_hue() < 300)){
            return true;
        }
    }
}

void sort_red(){
    if(sorter.get_proximity() > 100){
        
    }
}

void sort_blue(){
    if(sorter.get_proximity() > 100){
        
    }
}

void select_color_to_score(){
    while(1){
        if(colorsort == SCORERED){
            sort_blue();
        } else if(colorsort == SCOREBLUE){
            sort_red();
        }
        pros::delay(20);
    }
}

void stop_hooks(){
    hooks.move(-127);
    pros::delay(40);
    hooks.move(0);
}

void arm_up(){
    arm_state = UP;
}

void arm_down(){
    arm_state = DOWN;
}

void arm_load(){
    arm_state = LOAD;
}

void arm_descore(){
    arm_state = DESCORE;
}

void arm_state_switch(){
        while(1){
            if(arm_state == UP){
                arm_pid_error = 16000 - arm_rot.get_position();
            } else if(arm_state == LOAD){
                arm_pid_error = 3350 - arm_rot.get_position();
            } else if(arm_state == DOWN){
                arm_pid_error = 0 - arm_rot.get_position();
            } else if(arm_state == DESCORE){
                arm_pid_error = 16000 - arm_rot.get_position();
            }

            arm_pid_output = arm_pid.update(arm_pid_error);
            arm_motor.move(arm_pid_output);
            pros::delay(20);
        }
}

void arm_state_tracker(){
    while(1){
        if(arm_state_num == 1){
            arm_state = DOWN;
        } else if(arm_state_num == 2){
            arm_state = LOAD;
        } else if(arm_state_num == 3){
            arm_state = UP;
        } else if(arm_state_num == 4){
            arm_state = DESCORE;
        }
        pros::delay(10);
    }
}