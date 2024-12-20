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
int hook_deg;
bool y_pressed = false;
bool b_pressed = false;
bool right_pressed = false;
bool down_pressed = false;
int arm_velocity = 127;
bool hang_up = false;
int arm_softstop = -3000;
bool score_red = false;
bool score_blue = false;
bool intake_running = false;
bool descore_mech_activated = false;


ColorSort colorsort = NOSORT;
Color alliance = BLUE;
Arm_State arm_state = DOWN;

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

int get_hooks_position(){
    return abs(hooks_rot.get_position() % 66000);
}

int get_arm_position(){
    return arm_rot.get_value();
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
        if((sorter.get_hue() > 340 || sorter.get_hue() < 30) && !ejecting){
            ejecting = true;
            while(get_hooks_position() < 46000){
                pros::delay(10);
            }
            eject_ring();
            ejecting = false;
        }
    }
}

void sort_blue(){
    if(sorter.get_proximity() > 100){
        if((sorter.get_hue() > 180 && sorter.get_hue() < 300) && !ejecting){
            ejecting = true;
            intake.set_brake_mode(pros::MotorBrake::hold);
            while(get_hooks_position() < 46000){
                pros::delay(10);
            }
            eject_ring();
            ejecting = false;
        }
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

void arm_tracking_function(){
    while(1){
        console.print(std::to_string(get_arm_position()));
        pros::delay(10);
        console.clear();
    }
}

void hooks_tracking_function(){
    while(1){
        console.print(std::to_string(get_hooks_position()));
        pros::delay(10);
        console.clear();
    }
}

void hooks_state_switch(){
    while(1){
        // console.print(std::to_string(move_load_hooks));
        if(first_time_setting_hooks){
            delay_amount = 75;
        } else{
            delay_amount = 30;
        }
        if(move_load_hooks){
            if(hooks_reverse){
                hooks.move(-70);
                pros::delay(delay_amount);
            } else{
                hooks.move(70);
                pros::delay(delay_amount);
            }
            while(get_hooks_position() > 1750 && get_hooks_position() < 64250){ // range of error for the hook rot
                pros::delay(5);
            }
            hooks.move(0);
            move_load_hooks = false;
            first_time_setting_hooks = false;
        }
        pros::delay(20);
    }
}

void arm_state_function(){
    while(1){
        if(arm_state == DOWN){ // make arm go down
            while(get_arm_position() > 1600){
                arm_motor.move(-127);
                pros::delay(10);
            }
            if(intake_running){
                arm_motor.move(-40);
            } else{
                arm_motor.move(0);
            }
        } else if(arm_state == LOAD){ // make arm go to loading position
            intake.move(127);
            if(get_arm_position() > 3000){
                while(get_arm_position() > 2200){
                    arm_motor.move(-127);
                    pros::delay(10);
                }

                arm_motor.move(0);
            } else if(get_arm_position() < 1800){
                while(get_arm_position() < 1900){
                    arm_motor.move(127);
                    pros::delay(10);
                }

                arm_motor.move(0);
            }
        } else if(arm_state == UP){ // make arm go up
            if(get_arm_position() < 3000){
                while(get_arm_position() < 3550){
                    arm_motor.move(127);
                    pros::delay(20);
                    allow_score = false;
                }

                intake.move(-127);
                arm_motor.move(0);
                allow_score = true;
            }
        }
        pros::delay(10);
    }
}

void arm_up_control(){
    if(arm_state == DOWN){
        arm_state = LOAD;
    } else if(arm_state == LOAD){
        arm_state = UP;
    }
}

void arm_down_control(){
    if(arm_state == UP){
        arm_state = LOAD;
    } else if(arm_state == LOAD){
        arm_state = DOWN;
    }
}

void stop_hooks(){
    hooks.move(-127);
    pros::delay(100);
    hooks.move(0);
}


