#include "main.h"
#include "lemlib/api.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/chassis/trackingWheel.hpp"
#include "pros/adi.h"
#include "pros/adi.hpp"
#include "pros/misc.h"
#include "robodash/api.h"
#include <cstdio>
#include <string>

ASSET(path1_txt);

// config
pros::adi::AnalogIn selector_knob('A');
pros::adi::DigitalIn selector_switch('H');
pros::Rotation vert_tracker(11);
pros::Rotation horiz_tracker(20);
pros::Rotation arm_rot(19);
pros::Optical sorter(21);
// pros::Rotation hooks_rot(-20);

pros::adi::DigitalOut mogo_mech('B');
pros::adi::DigitalOut doinker('G');
pros::adi::DigitalOut intake_lift('D');

pros::Motor arm_motor(-1, pros::MotorGears::red);
pros::Motor intake(-5, pros::MotorGears::blue);

// left motor group
pros::MotorGroup left_motor_group({-12, -14, -15}, pros::MotorGears::blue);
// right motor group
pros::MotorGroup right_motor_group({2, 9, 10}, pros::MotorGears::blue);

// drivetrain settings
lemlib::Drivetrain drivetrain(&left_motor_group, // left motor group
                              &right_motor_group, // right motor group
                              13, // 14.5 inch track width
                              lemlib::Omniwheel::NEW_325, // using new 3.25" omnis
                              450, // drivetrain rpm is 450
                              8 // horizontal drift is 2 (for now)
);

// imu
pros::Imu imu(13);

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

// double get_hook_position(){
//     return (hooks_rot.get_position() % 54000);
// }







// ARM MACROS

void arm_move_load(){
    arm_moving = true;
    while(arm_moving){
        arm_motor.move(60);
        while(arm_rot.get_position()>11500){
            pros::delay(20);
        }
        if(arm_rot.get_position()<11500){
            arm_moving = false;
        }
    }
    arm_motor.set_brake_mode(pros::MotorBrake::hold);
    arm_motor.move(0);
}


void red_solo_awp(){
    red = true;
    chassis.setPose(-55, 32, 0);
    // pickup alliance partner matchload
    chassis.moveToPoint(-55, 40, 500, {.minSpeed = 100});
    chassis.swingToHeading(30, lemlib::DriveSide::RIGHT, 500);
    intake.move(127);
    pros::delay(500);
    intake.move(0);
    // pick up first mogo
    chassis.turnToPoint(-18, 22, 500, {.forwards = false});
    chassis.moveToPoint(-16, 21, 1500, {.forwards = false, .maxSpeed = 70});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    pros::delay(350);
    // intake rings
    intake.move(127);
    pros::delay(500);
    chassis.turnToPoint(-24, 48, 500);
    chassis.moveToPoint(-24, 48, 1000);
    chassis.waitUntilDone();
    pros::delay(500);
    chassis.turnToPoint(-40, 0, 750);
    chassis.moveToPoint(-40, 0, 1000);
    chassis.waitUntil(30);
    // drop first mogo
    mogo_mech.set_value(false);
    intake.move(0);
    chassis.waitUntilDone();
    // pick up second mogo
    chassis.turnToHeading(0, 750);
    chassis.moveToPoint(-18, -23, 2000, {.forwards = false, .maxSpeed = 65});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    pros::delay(250);
    // picking up rings
    chassis.turnToPoint(-16, -46, 750);
    chassis.moveToPoint(-16, -46, 1000);
    intake.move(127);
    // touching elevation structure
    chassis.turnToPoint(-24, 0, 1000);
    chassis.moveToPoint(-24, 0, 1500);

}
void red_left_6ring_nobar(){
    chassis.setPose(-55, 16, 180);
    // grabbing doinker ring
    chassis.moveToPoint(-55, 8, 500);
    chassis.waitUntilDone();
    doinker.set_value(true);
    pros::delay(250);
    // picking up mogo
    chassis.turnToPoint(-24, 20, 750, {.forwards = false, .maxSpeed = 45});
    chassis.moveToPoint(-24, 20, 1000, {.forwards = false, .maxSpeed = 70});
    chassis.waitUntilDone();
    doinker.set_value(false);
    mogo_mech.set_value(true);
    pros::delay(400);
    // picking up doinker ring
    chassis.moveToPoint(-40, 10, 1000);
    intake.move(127);
    chassis.waitUntilDone();
    pros::delay(500);
    // picking up ground ring 1
    chassis.turnToPoint(-28, 42, 750);
    chassis.moveToPoint(-28, 42, 1000);
    chassis.waitUntilDone();
    pros::delay(500);
    // picking up ground ring 2
    chassis.turnToPoint(-6, 44, 750);
    chassis.moveToPoint(-6, 44, 1000);
    chassis.waitUntilDone();
    pros::delay(250);
    // returning
    chassis.moveToPoint(-24, 48, 1000, {.forwards = false});
    // picking up ground ring 3
    chassis.turnToPoint(-10, 54, 500);
    chassis.moveToPoint(-10, 54, 1000);
    chassis.waitUntilDone();
    pros::delay(250);
    // clearing corner
    chassis.moveToPoint(-60, 40, 2000, {.forwards = false});
    chassis.turnToPoint(-64, 74, 500);
    chassis.waitUntilDone();
    doinker.set_value(true);
    chassis.moveToPoint(-64, 74, 1000);
    chassis.turnToHeading(90, 500);
    chassis.waitUntilDone();
    doinker.set_value(false);
}
void red_qual_right(){
    red = true;
    chassis.setPose(-55, -16, 0);
    // scoring preload on alliance stake
    chassis.swingToPoint(-62.75, -2, lemlib::DriveSide::LEFT, 500, {.minSpeed = 80});
    chassis.moveToPoint(-62.75, -2, 500);
    pros::Task alliance_stake_task([]{
        arm_motor.move(127);
        while(arm_rot.get_position() > -8000){
            pros::delay(20);
        }
        arm_motor.move(-127);
        while(arm_rot.get_position() < 13000){
            pros::delay(20);
        }
        arm_motor.move(0);
    });
    chassis.waitUntilDone();
    pros::delay(250);
    chassis.turnToHeading(310, 500);
    chassis.waitUntilDone();
    pros::delay(500);
    // grabbing ring with doinker
    chassis.turnToHeading(0, 500);
    chassis.moveToPoint(-60, -16, 500, {.forwards = false});
    chassis.turnToPoint(-48, -4, 500);
    chassis.waitUntilDone();
    doinker.set_value(true);
    pros::delay(250);
    // picking up mogo
    chassis.turnToHeading(0, 500, {.maxSpeed = 60});
    chassis.swingToPoint(-48, -38, lemlib::DriveSide::RIGHT, 500, {.forwards = false});
    chassis.moveToPoint(-48, -38, 1000, {.forwards = false});
    chassis.turnToPoint(-22, -24, 500, {.forwards = false});
    chassis.moveToPoint(-22, -24, 1000, {.forwards = false});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    doinker.set_value(false);
    pros::delay(250);
    //scoring doinker ring
    chassis.moveToPoint(-42, -38, 1000);
    intake.move(127);
    chassis.waitUntilDone();
    pros::delay(750);
    // picking up ring
    chassis.turnToPoint(-24, -40, 750);
    chassis.moveToPoint(-24, -40, 1000);
    chassis.waitUntilDone();
    pros::delay(1000);
    // touching elevation structure
    chassis.turnToPoint(-24, 2, 750);
    chassis.moveToPoint(-24, 2, 1000);
}
void red_elim_left(){}
void red_elim_right(){}
void blue_solo_awp(){
    red = true;
    chassis.setPose(55, 32, 0);
    // pickup alliance partner matchload
    chassis.moveToPoint(55, 40, 500, {.minSpeed = 100});
    chassis.swingToHeading(-30, lemlib::DriveSide::LEFT, 500);
    intake.move(127);
    pros::delay(300);
    intake.move(0);
    // pick up first mogo
    chassis.turnToPoint(24, 26, 500, {.forwards = false});
    chassis.moveToPoint(24, 26, 1500, {.forwards = false, .maxSpeed = 70});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    pros::delay(250);
    // intake rings
    intake.move(127);
    pros::delay(500);
    chassis.turnToPoint(24, 44, 500);
    chassis.moveToPoint(24, 44, 1000);
    chassis.waitUntilDone();
    pros::delay(500);
    chassis.turnToPoint(40, 0, 750);
    chassis.moveToPoint(46, 0, 1250);
    chassis.waitUntil(25);
    // drop first mogo
    mogo_mech.set_value(false);
    intake.move(0);
    chassis.waitUntilDone();
    // pick up second mogo
    chassis.turnToHeading(0, 750);
    chassis.moveToPoint(18, -26, 2000, {.forwards = false, .maxSpeed = 65});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    pros::delay(250);
    // picking up rings
    chassis.turnToPoint(30, -46, 750);
    chassis.moveToPoint(30, -46, 1000);
    intake.move(127);
    // touching elevation structure
    chassis.turnToPoint(20, 0, 1000);
    chassis.moveToPoint(20, 0, 1500);
}
void blue_qual_left(){
    red = true;
    chassis.setPose(-55, 16, 180);
    // scoring preload on alliance stake
    chassis.swingToPoint(-61.5, -.5, lemlib::DriveSide::RIGHT, 500, {.minSpeed = 80});
    chassis.moveToPoint(-61.5, -.5, 500);
    pros::Task alliance_stake_task([]{
        arm_motor.move(127);
        while(arm_rot.get_position() > -8000){
            pros::delay(20);
        }
        arm_motor.move(-127);
        while(arm_rot.get_position() < 13000){
            pros::delay(20);
        }
        arm_motor.move(0);
    });
    chassis.waitUntilDone();
    pros::delay(250);
    chassis.turnToHeading(230, 500);
    chassis.waitUntilDone();
    pros::delay(500);
    // grabbing ring with doinker
    chassis.moveToPoint(-55, 15, 1000, {.forwards = false});
    chassis.turnToHeading(165, 500);
    chassis.waitUntilDone();
    doinker.set_value(true);
    pros::delay(250);
}
void blue_qual_right(){
    red = true;
    chassis.setPose(-55, -16, 0);
    // scoring preload on alliance stake
    chassis.swingToPoint(-62.75, -2, lemlib::DriveSide::LEFT, 500, {.minSpeed = 80});
    chassis.moveToPoint(-62.75, -2, 500);
    pros::Task alliance_stake_task([]{
        arm_motor.move(127);
        while(arm_rot.get_position() > -8000){
            pros::delay(20);
        }
        arm_motor.move(-127);
        while(arm_rot.get_position() < 13000){
            pros::delay(20);
        }
        arm_motor.move(0);
    });
    chassis.waitUntilDone();
    pros::delay(250);
    chassis.turnToHeading(310, 500);
    chassis.waitUntilDone();
    pros::delay(500);
    // grabbing ring with doinker
    chassis.turnToHeading(0, 500);
    chassis.moveToPoint(-60, -16, 500, {.forwards = false});
    chassis.turnToPoint(-48, -4, 500);
    chassis.waitUntilDone();
    doinker.set_value(true);
    pros::delay(250);
    // picking up mogo
    chassis.turnToHeading(0, 500, {.maxSpeed = 60});
    chassis.swingToPoint(-48, -38, lemlib::DriveSide::RIGHT, 500, {.forwards = false});
    chassis.moveToPoint(-48, -38, 1000, {.forwards = false});
    chassis.turnToPoint(-22, -24, 500, {.forwards = false});
    chassis.moveToPoint(-22, -24, 1000, {.forwards = false});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    doinker.set_value(false);
    pros::delay(250);
    //scoring doinker ring
    chassis.moveToPoint(-42, -38, 1000);
    intake.move(127);
    chassis.waitUntilDone();
    pros::delay(750);
    // picking up ring
    chassis.turnToPoint(-24, -40, 750);
    chassis.moveToPoint(-24, -40, 1000);
    chassis.waitUntilDone();
    pros::delay(1000);
    // touching elevation structure
    chassis.turnToPoint(-24, 2, 750);
    chassis.moveToPoint(-24, 2, 1000);
}
void blue_elim_left(){}
void blue_elim_right(){}
void prog_skills(){
    chassis.setPose(-62, 0, 90);
    // scoring on alliance stake
    intake.move(127);
    pros::delay(500);
    intake.move(0);
    // picking up mogo
    chassis.moveToPoint(-48, 0, 750);
    chassis.turnToHeading(0, 500);
    chassis.moveToPoint(-48, -24, 1000, {.forwards = false});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    pros::delay(250);
    // picking up ring 1
    chassis.turnToPoint(-28, -24, 500);
    chassis.moveToPoint(-28, -24, 1000, {.minSpeed = 90});
    intake.move(127);
    // picking up ring 2
    chassis.swingToPoint(0, -48, lemlib::DriveSide::RIGHT, 500, {.minSpeed = 90});
    chassis.moveToPoint(6, -62, 1000);
    chassis.waitUntilDone();
    pros::delay(250);
    chassis.moveToPoint(chassis.getPose().x-6, chassis.getPose().y+6, 1000, {.forwards = false});
    chassis.turnToHeading(270, 750);
    // picking up ring 3
    chassis.moveToPoint(-24, -52, 1000);
    chassis.waitUntilDone();
    pros::delay(250);
    // picking up ring 4
    chassis.moveToPoint(-44, -52, 1000);
    chassis.waitUntilDone();
    pros::delay(750);
    // picking up ring 5
    chassis.moveToPoint(-56, -52, 1000);
    chassis.waitUntilDone();
    pros::delay(250);
    // picking up ring 6
    chassis.turnToPoint(-42, -60, 750);
    chassis.moveToPoint(-42, -60, 1000);
    chassis.waitUntilDone();
    pros::delay(500);
    // placing mogo 1 in corner
    chassis.turnToPoint(-68, -68, 500, {.forwards = false});
    chassis.moveToPoint(-68, -68, 1000, {.forwards = false});
    chassis.waitUntilDone();
    mogo_mech.set_value(false);
    intake.move(-127);
    pros::delay(250);
    intake.move(0);
    // moving into position for next side of field
    chassis.moveToPoint(-54, 0, 2000);
    chassis.waitUntilDone();
    // picking up mogo 2
    chassis.turnToPoint(chassis.getPose().x+4, 28, 750, {.forwards = false});
    chassis.moveToPoint(chassis.getPose().x+4, 28, 1000, {.forwards = false});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    pros::delay(250);
    // picking up ring 1
    chassis.turnToPoint(-24, 24, 500);
    chassis.moveToPoint(-24, 24, 1000);
    intake.move(127);
    chassis.waitUntilDone();
    pros::delay(250);
    // picking up ring 2
    chassis.turnToPoint(0, 0, 500);
    chassis.moveToPoint(0, 0, 1000);
    chassis.waitUntilDone();
    pros::delay(250);
    // picking up ring 3
    chassis.turnToPoint(-24, 24, 750);
    chassis.moveToPoint(-24, 24, 1000);
    chassis.moveToPoint(-30, 48, 1000);
    chassis.waitUntilDone();
    pros::delay(250);
    // picking up ring 4
    chassis.turnToPoint(-44, 46, 500);
    chassis.moveToPoint(-44, 46, 1000);
    chassis.waitUntilDone();
    pros::delay(500);
    // picking up ring 5
    chassis.moveToPoint(-60, 46, 500);
    chassis.waitUntilDone();
    pros::delay(500);
    // picking up ring 6
    chassis.turnToPoint(-50, 62, 500);
    chassis.moveToPoint(-50, 62, 1000);
    chassis.waitUntilDone();
    pros::delay(500);
    // placing mogo 2 in corner
    chassis.turnToPoint(-64, 64, 500, {.forwards = false});
    chassis.moveToPoint(-64, 64, 1000, {.forwards = false});
    chassis.waitUntilDone();
    mogo_mech.set_value(false);
    intake.move(-127);
    pros::delay(250);
    intake.move(0);
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

// initialize function. Runs on program startup
void initialize() {
    chassis.calibrate(); // calibrate sensors
    console.focus();
    pros::Controller controller(CONTROLLER_MASTER);
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
    pros::Task selector_task([]{
        while(!auto_started){
            while(!(selector_switch.get_value() == 1)){
                pros::delay(20);            
            }
            console.clear();
            selector_value = selector_knob.get_value();
            if(selector_value < 90){
                auton_selected = 1;
                console.println("RED QUAL LEFT");
            } else if(selector_value > 90 && selector_value < 700) {
                auton_selected = 2;
                console.println("RED QUAL RIGHT");
            } else if(selector_value > 700 && selector_value < 1350){
                auton_selected = 3;
                console.println("RED ELIM LEFT");
            } else if(selector_value > 1350 && selector_value < 1900){
                auton_selected = 4;
                console.println("RED ELIM RIGHT");
            } else if(selector_value > 1900 && selector_value < 2500){
                auton_selected = 5;
                console.println("BLUE QUAL LEFT");
            } else if(selector_value > 2500 && selector_value < 3200){
                auton_selected = 6;
                console.println("BLUE QUAL RIGHT");
            } else if(selector_value > 3200 && selector_value < 4095){
                auton_selected = 7;
                console.println("BLUE ELIM LEFT");
            } else {
                auton_selected = 8;
                console.println("BLUE ELIM RIGHT");
            }
        }
    });
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
    red_left_6ring_nobar();
    // switch(auton_selected) {
    //     case 1:
    //         red_qual_left();
    //         break;
    //     case 2:
    //         red_qual_right();
    //         break;
    //     case 3:
    //         red_elim_left();
    //         break;
    //     case 4:
    //         red_elim_right();
    //         break;
    //     case 5:
    //         blue_qual_left();
    //         break;
    //     case 6:
    //         blue_qual_right();
    //         break;
    //     case 7:
    //         blue_elim_left();
    //         break;
    //     case 8:
    //         blue_elim_right();
    //         break;
    // }
    
}

bool y_pressed = false;
bool b_pressed = false;
bool right_pressed = false;
bool down_pressed = false;
int arm_velocity = 127;
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
	bool ring_mech_on = false;
    arm_motor.set_brake_mode(pros::MotorBrake::hold);
    

	while (true) {
		// get left y and right x positions
        int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int rightX = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
        console.println(std::to_string(chassis.getPose().x));
        console.println(std::to_string(chassis.getPose().y));
        
        // move the robot
        chassis.arcade(leftY, rightX);
        
		// intake controls
		if(controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)){
			intake.move(127);
            ring_mech_on = true;
		} else if(controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2)){
            if(ring_mech_on){
                intake.move(0);
                ring_mech_on = false;
            } else {
                intake.move(-127);
                ring_mech_on = true;
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
            // arm_velocity = 127;
            pros::Task task{[] {
                arm_move_load();
            }};
        }

        if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1)){
            arm_motor.move(arm_velocity);
        } else if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2)){
            arm_motor.move(-arm_velocity);
            
        } else if(!arm_moving){
            arm_motor.move(0);
        }
        
        // manual color sort override
        if(controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)){
            red = true;
        } else if(controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_LEFT)){
            red = false;
        }

        // delay to save resources
        pros::delay(25);
        console.clear();
	}
}