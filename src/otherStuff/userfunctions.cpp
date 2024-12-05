#include "userfunctions.h"
#include "pros/rtos.hpp"
#include "config.h"





bool ejecting = false;
bool red = true;
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




Arm_State arm_state = DOWN;

void eject_ring(){
    pros::delay(40);
    intake.move(-127);
    pros::delay(150);
    intake.move(127);
}

int get_hooks_position(){
    return abs(hooks_rot.get_position() % 66000);
}

int get_arm_position(){
    return arm_rot.get_value();
}

void sort_red(){
    sorter.set_led_pwm(35);
    if(sorter.get_proximity() > 150){
        pros::delay(50);
        if((sorter.get_hue() > 350 || sorter.get_hue() < 10) && !ejecting){
            ejecting = true;
            intake.set_brake_mode(pros::MotorBrake::hold);
            pros::delay(100);
            // while(!(get_hook_position() > 9000)){
            //     pros::delay(10);/
            // }
            eject_ring();
            intake.set_brake_mode(pros::MotorBrake::coast);
            ejecting = false;
        }
    }
}

void sort_blue(){
    sorter.set_led_pwm(35);
    if(sorter.get_proximity() > 150){
        pros::delay(50);
        if((sorter.get_hue() > 180 && sorter.get_hue() < 300) && !ejecting){
            ejecting = true;
            intake.set_brake_mode(pros::MotorBrake::hold);
            // while(!(get_hook_position() > 9000)){
            //     pros::delay(10);
            // }
            eject_ring();
            intake.set_brake_mode(pros::MotorBrake::coast);
            ejecting = false;
        }
    }
}

void color_sort(Color color){
    if(color == RED){
        sort_red();
    } else if(color == BLUE){
        sort_blue();
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
            while(!(arm_down_switch.get_value() == 1)){
                arm_motor.move(-127);
                pros::delay(10);
            }
            
            arm_motor.move(0);
            arm_rot.reset();
        } else if(arm_state == LOAD){ // make arm go to loading position
            if(get_arm_position() > 200){
                while(get_arm_position() > 170){
                    arm_motor.move(-127);
                    pros::delay(10);
                }

                arm_motor.move(0);
            } else if(get_arm_position() < 75){
                while(get_arm_position() < 110){
                    arm_motor.move(127);
                    pros::delay(10);
                }

                arm_motor.move(0);
            }
        } else if(arm_state == UP){ // make arm go up
            if(get_arm_position() < 455){
                while(get_arm_position() < 450){
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



