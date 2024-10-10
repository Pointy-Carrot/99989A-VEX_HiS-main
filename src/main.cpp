#include "main.h"
#include "lemlib/api.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/chassis/trackingWheel.hpp"
#include "pros/adi.h"
#include "pros/adi.hpp"
#include "robodash/api.h"
#include <cstdio>
#include <string>

ASSET(path1_txt);

// config
pros::adi::AnalogIn selector_knob('B');
pros::adi::DigitalIn selector_switch('D');
pros::Rotation vert_tracker(16);
pros::Rotation arm_rot(10);
pros::Optical sorter(11);
pros::Rotation hooks_rot(-12);

pros::adi::DigitalOut mogo_mech('A');
pros::adi::DigitalOut doinker('E');
pros::adi::DigitalOut intake_lift('H');

pros::Motor arm_motor(-7, pros::MotorGears::red);
pros::Motor intake(-15, pros::MotorGears::blue);

// left motor group
pros::MotorGroup left_motor_group({-3, -4, -5}, pros::MotorGears::blue);
// right motor group
pros::MotorGroup right_motor_group({1, 6, 9}, pros::MotorGears::blue);

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

// odometry settings
lemlib::OdomSensors sensors(&vertical_tracking_wheel, // vertical tracking wheel 1, set to null
                            nullptr, // vertical tracking wheel 2, set to nullptr as we are using IMEs
                            nullptr, // horizontal tracking wheel 1
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
                                              28, // derivative gain (kD)
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

void eject_ring(){
    intake.move(-127);
    pros::delay(100);
    intake.move(127);
}

enum Color {
    RED,
    BLUE
};

double get_hook_position(){
    return (hooks_rot.get_position() % 54000);
}







// ARM MACROS

void arm_move_load(){
    arm_moving = true;
    while(arm_moving){
        arm_motor.move(60);
        while(arm_rot.get_position()>16000){
            pros::delay(20);
        }
        if(arm_rot.get_position()<16000){
            arm_moving = false;
        }
    }
    arm_motor.set_brake_mode(pros::MotorBrake::hold);
    arm_motor.move(0);
}

void red_qual_left(){
    // score preload on alliance stake
    red = true;
    chassis.setPose(-56, 12, 0);
    chassis.moveToPoint(-56, -6, 750, {.forwards = false});
    chassis.moveToPoint(-56, -1, 500);
    chassis.turnToHeading(90, 500);
    chassis.moveToPoint(-62, -1, 500, {.forwards = false});
    chassis.turnToHeading(92, 500);
    chassis.waitUntilDone();
    intake.move(127);
    pros::delay(500);
    intake.move(0);
    // pick up mogo
    chassis.swingToHeading(0, lemlib::DriveSide::LEFT, 750);
    chassis.turnToPoint(-24, 21, 750, {.forwards = false});
    chassis.moveToPose(-20, 24, 240, 1500, {.forwards = false, .minSpeed = 70});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    pros::delay(125);
    // score ring 1
    chassis.turnToPoint(-26, 42, 500);
    chassis.moveToPoint(-26, 42, 500);
    intake.move(127);
    chassis.waitUntilDone();
    pros::delay(250);
    // score ring 2
    chassis.turnToPoint(-10, 42, 500);
    chassis.moveToPoint(-10, 42, 1000);
    chassis.waitUntilDone();
    pros::delay(250);
    // scoring ring 3
    chassis.moveToPoint(-24, 48, 1000, {.forwards = false});
    chassis.turnToPoint(-8, 50, 500);
    chassis.moveToPoint(-8, 50, 1000);
    chassis.waitUntilDone();
    pros::delay(250);
    // touching elevation structure
    chassis.moveToPoint(-24, 48, 1000, {.forwards = false});
    chassis.turnToHeading(180, 500);
    chassis.moveToPoint(-24, 4, 2000);
}
void red_qual_right(){
chassis.setPose(-56, 12, 0);
    chassis.moveToPoint(-56, 6, 750, {.forwards = false});
    chassis.moveToPoint(-56, 1, 500);
    chassis.turnToHeading(90, 500);
    chassis.moveToPoint(-62, 1, 500, {.forwards = false});
    chassis.turnToHeading(88, 500);
    chassis.waitUntilDone();
    intake.move(127);
    pros::delay(500);
    intake.move(0);
    // pick up mogo
    chassis.swingToHeading(180, lemlib::DriveSide::RIGHT, 750);
    chassis.turnToPoint(-24, -24, 750, {.forwards = false});
    chassis.moveToPoint(-24, -24, 1000, {.forwards = false});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    pros::delay(250);
    // score ring 1
    chassis.turnToPoint(-26, -40, 500);
    chassis.moveToPoint(-26, -40, 500);
    intake.move(127);
    chassis.waitUntilDone();
    pros::delay(250);
    chassis.moveToPoint(chassis.getPose().x, -30, 500, {.forwards = false});
    // clear corner
    chassis.moveToPose(-64, -64, 270, 2000, {.minSpeed = 90});
    chassis.waitUntil(20);
    doinker.set_value(true);
    chassis.waitUntilDone();
    chassis.turnToHeading(315, 750);
    chassis.waitUntilDone();
    doinker.set_value(false);
    pros::delay(250);
    // score ring 2
    chassis.moveToPoint(-60, -60, 1000, {.maxSpeed = 60});
    chassis.waitUntilDone();
    pros::delay(500);
    // touch elevation structure
    chassis.moveToPoint(-38, -26, 1000, {.forwards = false});
    chassis.turnToPoint(-36, -6, 750);
    chassis.moveToPoint(-36, -6, 2000);
}
void red_elim_left(){
    // score preload on alliance stake
    red = true;
    chassis.setPose(-56, 12, 0);
    chassis.moveToPoint(-56, -6, 750, {.forwards = false});
    chassis.moveToPoint(-56, -1, 500);
    chassis.turnToHeading(90, 500);
    chassis.moveToPoint(-62, -1, 500, {.forwards = false});
    chassis.turnToHeading(92, 500);
    chassis.waitUntilDone();
    intake.move(127);
    pros::delay(500);
    intake.move(0);
    // pick up mogo
    chassis.swingToHeading(0, lemlib::DriveSide::LEFT, 750);
    chassis.turnToPoint(-24, 21, 750, {.forwards = false});
    chassis.moveToPose(-20, 24, 240, 1500, {.forwards = false, .minSpeed = 70});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    pros::delay(125);
    // score ring 1
    chassis.turnToPoint(-26, 42, 500);
    chassis.moveToPoint(-26, 42, 500);
    intake.move(127);
    chassis.waitUntilDone();
    pros::delay(250);
    // score ring 2
    chassis.turnToPoint(-10, 42, 500);
    chassis.moveToPoint(-10, 42, 1000);
    chassis.waitUntilDone();
    pros::delay(250);
    // scoring ring 3
    chassis.moveToPoint(-24, 48, 1000, {.forwards = false});
    chassis.turnToPoint(-8, 50, 500);
    chassis.moveToPoint(-8, 50, 1000);
    chassis.waitUntilDone();
    pros::delay(250);
    // touching elevation structure
    chassis.moveToPoint(-24, 48, 1000, {.forwards = false});
    chassis.turnToHeading(180, 500);
    chassis.moveToPoint(-24, 4, 2000);
}
void red_elim_right(){}
void blue_qual_left(){}
void blue_qual_right(){}
void blue_elim_left(){}
void blue_elim_right(){}
void prog_skills(){}





rd::Selector selector({
    {"PROG SKILLS", &prog_skills},
    {"RED QUAL LEFT", &red_qual_left},
    {"RED QUAL RIGHT", &red_qual_right},
    {"RED ELIM LEFT", &red_elim_left},
    {"RED ELIM RIGHT", &red_elim_right},
    {"BLUE QUAL LEFT", &blue_qual_left},
    {"BLUE QUAL RIGHT", &blue_qual_right},
    {"BLUE ELIM LEFT", &blue_elim_left},
    {"BLUE ELIM RIGHT", &blue_elim_right}
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
            while(!(get_hook_position() > 9000)){
                pros::delay(10);
            }
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
            pros::delay(100);
            while(!(get_hook_position() > 9000)){
                pros::delay(10);
            }
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
    selector.focus();
    pros::Task sort_task([]{
        while(true){
            if(!red){
                color_sort(RED);
                pros::delay(20);
            } else if(red){
                color_sort(BLUE);
                pros::delay(20);
            }
        }
    });
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
    red_qual_right();
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
	pros::Controller controller(pros::E_CONTROLLER_MASTER);
	bool ring_mech_on = false;
    arm_motor.set_brake_mode(pros::MotorBrake::hold);
    red = true;
    

	while (true) {
		// get left y and right x positions
        int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int rightX = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
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

        // arm controls
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
            // red = true;
            intake.move(127);
            pros::delay(100);
            while(!(get_hook_position() < 5000)){
                pros::delay(10);
            }
            intake.move(0);
        } else if(controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_LEFT)){
            red = false;
        }

        // delay to save resources
        pros::delay(25);
	}
}