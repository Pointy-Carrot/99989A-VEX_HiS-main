#include "main.h"
#include "lemlib/api.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/chassis/trackingWheel.hpp"
#include "pros/abstract_motor.hpp"
#include "pros/adi.h"
#include "pros/adi.hpp"
#include "pros/misc.h"
#include "pros/motor_group.hpp"
#include "pros/optical.h"
#include "robodash/api.h"
#include <cstdio>
#include <string>

ASSET(path1_txt);

// config
pros::adi::DigitalIn selector_switch('H');
pros::Rotation vert_tracker(-2);
pros::Rotation horiz_tracker(13);
pros::adi::Encoder arm_rot('E', 'F');
pros::Rotation hooks_rot(3);
pros::Optical sorter(20);
// pros::Rotation hooks_rot(-20);

pros::adi::DigitalOut mogo_mech('B');
pros::adi::DigitalOut doinker('G');

pros::Motor arm_motor(-1, pros::MotorGears::red);
pros::Motor intake(6, pros::v5::MotorGears::blue);
pros::Motor hooks(5, pros::v5::MotorGears::blue);

// left motor group
pros::MotorGroup left_motor_group({8, -9, -10}, pros::MotorGears::blue);
// right motor group
pros::MotorGroup right_motor_group({-15, 19, 11}, pros::MotorGears::blue);

// drivetrain settings
lemlib::Drivetrain drivetrain(&left_motor_group, // left motor group
                              &right_motor_group, // right motor group
                              13, // 14.5 inch track width
                              lemlib::Omniwheel::NEW_325, // using new 3.25" omnis
                              450, // drivetrain rpm is 450
                              8 // horizontal drift is 2 (for now)
);

// imu
pros::Imu imu(16);

// horizontal tracking wheel encoder
// pros::Rotation horizontal_encoder(20);
// vertical tracking wheel encoder
// pros::adi::Encoder vertical_encoder('C', 'D', true);
// horizontal tracking wheel
// lemlib::TrackingWheel horizontal_tracking_wheel(&horizontal_encoder, lemlib::Omniwheel::NEW_275, -5.75);
// vertical tracking wheel
lemlib::TrackingWheel vertical_tracking_wheel(&vert_tracker, lemlib::Omniwheel::NEW_2, .75);
lemlib::TrackingWheel horizontal_tracking_wheel(&horiz_tracker, lemlib::Omniwheel::NEW_2, 2);

// odometry settings
lemlib::OdomSensors sensors(&vertical_tracking_wheel, // vertical tracking wheel 1, set to null
                            nullptr, // vertical tracking wheel 2, set to nullptr as we are using IMEs
                            &horizontal_tracking_wheel, // horizontal tracking wheel 1
                            nullptr, // horizontal tracking wheel 2, set to nullptr as we don't have a second one
                            &imu // inertial sensor
);

// lateral PID controller
lemlib::ControllerSettings lateral_controller(4, // proportional gain (kP)
                                              0, // integral gain (kI)
                                              2, // derivative gain (kD)
                                              3, // anti windup
                                              1, // small error range, in inches
                                              100, // small error range timeout, in milliseconds
                                              3, // large error range, in inches
                                              500, // large error range timeout, in milliseconds
                                              20 // maximum acceleration (slew)
);

// angular PID controller
lemlib::ControllerSettings angular_controller(4, // proportional gain (kP)
                                              0, // integral gain (kI)
                                              32, // derivative gain (kD)
                                              3, // anti windup
                                              1, // small error range, in degrees
                                              100, // small error range timeout, in milliseconds
                                              3, // large error range, in degrees
                                              500, // large error range timeout, in milliseconds
                                              0 // maximum acceleration (slew)
);

// create the chassis
lemlib::Chassis chassis(drivetrain, // drivetrain settings
                        lateral_controller, // lateral PID settings
                        angular_controller, // angular PID settings
                        sensors // odometry sensors
);







// COLOR SORTER CODE

bool ejecting = false;
bool red = true;
bool arm_moving = false;
double current_position;

void eject_ring(){
    pros::delay(40);
    intake.move(-127);
    pros::delay(150);
    intake.move(127);
}

enum Color {
    RED,
    BLUE
};

enum Arm_State {
    DOWN,
    LOAD,
    UP
};

Arm_State arm_state = DOWN;

int get_hooks_position(){
    return abs(hooks_rot.get_position() % 66000);
}

int get_arm_position(){
    return arm_rot.get_value();
}


void red_left_sawp(){
}
void red_right_aStake_bar(){
}
void red_right_rush(){}
void red_left_5ring_bar(){
}
void blue_right_sawp(){
}
void blue_right_5ring_bar(){}
void blue_left_rush(){}
void blue_right_6ring_nobar(){}
void blue_right_aStake_bar(){
}










void prog_skills(){
}





rd::Selector selector({
});





rd::Console console;

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

bool auto_started = false;
int selector_value;
int auton_selected = 0;
bool move_load_hooks = false;
bool hooks_reverse = true;
bool first_time_setting_hooks = true;
int delay_amount;


int hook_deg;

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
            while(get_arm_position() > 0){
                arm_motor.move(-127);
                pros::delay(10);
            }
            
            arm_motor.move(0);
        } else if(arm_state == LOAD){ // make arm go to loading position
            if(get_arm_position() > 150){
                while(get_arm_position() > 145){
                    arm_motor.move(-127);
                    pros::delay(10);
                }

                arm_motor.move(0);
            } else if(get_arm_position() < 75){
                while(get_arm_position() < 105){
                    arm_motor.move(127);
                    pros::delay(10);
                }

                arm_motor.move(0);
            }
        } else if(arm_state == UP){ // make arm go up
            if(get_arm_position() < 465){
                while(get_arm_position() < 465){
                    arm_motor.move(127);
                    pros::delay(20);
                }

                arm_motor.move(0);
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




// initialize function. Runs on program startup
void initialize() {
    chassis.calibrate(); // calibrate sensors
    console.focus();
    hooks_rot.set_position(0);
    pros::Controller controller(CONTROLLER_MASTER);
    pros::Task hooks_state_switch_task(hooks_state_switch);
    pros::Task arm_state_switch_task(arm_state_function);
    pros::Task show_arm_task(arm_tracking_function);
    hooks_rot.set_data_rate(5);
    arm_motor.set_brake_mode(pros::MotorBrake::hold);
    hooks.set_brake_mode(pros::MotorBrake::hold);
    arm_rot.reset();
    // pros::Task sort_task([]{
    //     while(true){
    //         if(!red){
    //             color_sort(RED);
    //             pros::delay(20);
    //         } else if(red){
    //             color_sort(BLUE);
    //             pros::delay(20);
    //         }
    //     }
    // });
}



/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}
/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
    auto_started = true;
    // red_left_5ring_cornerclear(); //-> scores 6 rings on a single mogo. Start bordering two tiles, facing 180 deg.
    // red_left_5ring_bar(); //-> scores 5 rings on a single mogo and touches bar. Start bordering two tiles, facing 180 deg.
    // red_left_sawp(); //-> Solo AWP. Start bordering two tiles, facing 0 deg.
    //red_right_aStake_bar(); //-> Partial AWP except for moving partner off line. Start Bordering two tiles, facing 0 deg.
    //red_right_rush(); //-> Rush mid mogo.

    //blue_right_5ring_cornerclear(); //-> scores 6 rings on a single mogo. Start bordering two tiles, facing 180 deg.
    //blue_right_5ring_bar(); //-> scores 5 rings on a single mogo and touches bar. Start bordering two tiles, facing 180 deg.
    //blue_right_sawp(); //-> Solo AWP. Start bordering two tiles, facing 0 deg.
    //blue_right_aStake_bar(); //-> Partial AWP except for moving partner off line. Start Bordering two tiles, facing 0 deg.
    //blue_left_rush(); //-> Rush mid mogo.

    
    // prog_skills();
    
}

bool y_pressed = false;
bool b_pressed = false;
bool right_pressed = false;
bool down_pressed = false;
int arm_velocity = 127;
bool hang_up = false;
int arm_softstop = -3000;
/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	console.focus();
    pros::Controller controller(CONTROLLER_MASTER);
	bool intake_running = false;
    bool hooks_running = false;
    

	while (true) {
		// get left y and right x positions
        int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int rightX = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
        // console.println(std::to_string(chassis.getPose().x));
        // console.println(std::to_string(chassis.getPose().y));
        // move the robot
        chassis.arcade(leftY, rightX);
        
		// intake controls
		if(controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)){
			intake.move(127);
            hooks.move(127);
            intake_running = true;
            hooks_running = true;
            first_time_setting_hooks = false;
		} else if(controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2)){
            if(intake_running){
                if(hooks_running){
                    intake.move(-127);
                    hooks.move(-127);
                    pros::delay(75);
                    intake.move(0);
                    hooks.move(0);
                    intake_running = false;
                    hooks_running = false;
                } else{
                    hooks.move(-127);
                    hooks_running = true;
                    first_time_setting_hooks = false;
                }
            } else {
                intake.move(-127);
                hooks.move(-127);
                intake_running = true;
                hooks_running = true;
                first_time_setting_hooks = false;
            }
		}
		
		// mogo mech controls
		if(controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_Y)){
			if(y_pressed){
                mogo_mech.set_value(false);
                y_pressed = false;
            } else{
                mogo_mech.set_value(true);
                y_pressed = true;
            }
		}

        // doinker controls
        if(controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)){
            if(b_pressed){
                doinker.set_value(false);
                b_pressed = false;
            } else{
                doinker.set_value(true);
                b_pressed = true;
            }
        }

        if(controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_RIGHT)){
            if(intake_running){
                move_load_hooks = true;
            } else{
                intake.move(127);
                move_load_hooks = true;
                intake_running = true;
            }
        }

        if(controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)){
            arm_up_control();
        } else if(controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)){
            arm_down_control();
        }

        

        // delay to save resources
        pros::delay(25);
        // console.clear();
	}
}