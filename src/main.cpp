#include "main.h"
#include "lemlib/api.hpp"
#include "robodash/api.h"
#include <cstdio>
#include <string>

ASSET(path1_txt);

// config

pros::Rotation vert_tracker(16);
pros::Rotation arm_rot(10);
pros::Optical sorter(14);
pros::Rotation hooks_rot(-12);

pros::adi::DigitalOut mogo_mech('A');
pros::adi::DigitalOut arm('E');
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
lemlib::ControllerSettings lateral_controller(10, // proportional gain (kP)
                                              0, // integral gain (kI)
                                              3, // derivative gain (kD)
                                              3, // anti windup
                                              1, // small error range, in inches
                                              100, // small error range timeout, in milliseconds
                                              3, // large error range, in inches
                                              500, // large error range timeout, in milliseconds
                                              20 // maximum acceleration (slew)
);

// angular PID controller
lemlib::ControllerSettings angular_controller(2, // proportional gain (kP)
                                              0, // integral gain (kI)
                                              10, // derivative gain (kD)
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




void red_5_ring(){
    red = true;
    chassis.setPose(-48, 30, 270);

    chassis.moveToPoint(-36, 30, 500, {.forwards = false});
    chassis.turnToHeading(300, 500);
    chassis.waitUntilDone();
    chassis.moveToPoint(-24, 24, 1000, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    pros::delay(250);
    intake.move(127);
    pros::delay(100);
    chassis.turnToPoint(-24, 52, 500);
    chassis.moveToPoint(-24, 40, 500, {.minSpeed = 60});
    intake.move(127);
    chassis.waitUntilDone();
    chassis.turnToHeading(90, 500);
    chassis.waitUntilDone();
    chassis.moveToPoint(-12, chassis.getPose().y-6, 1000, {.maxSpeed = 40});
    chassis.waitUntilDone();
    pros::delay(250);
    chassis.moveToPoint(-24, chassis.getPose().y+4, 1000, {.forwards = false});
    chassis.turnToPoint(-12, chassis.getPose().y, 500);
    chassis.moveToPoint(-12, chassis.getPose().y, 2000, {.maxSpeed = 40});
    chassis.waitUntilDone();
    pros::delay(250);
    chassis.moveToPoint(-24, chassis.getPose().y-2, 1000, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntilDone();
    pros::delay(500);
    chassis.turnToHeading(270, 750);
    chassis.moveToPose(-53, 8, 180, 2000, {.maxSpeed = 80});
    intake_lift.set_value(true);
    chassis.waitUntilDone();
    intake_lift.set_value(false);
    pros::delay(250);
    chassis.moveToPoint(-50, 20, 1000, {.forwards = false, .maxSpeed = 60});
    chassis.turnToPoint(-28, 12, 500);
    intake.move(0);
    chassis.moveToPoint(-28, 12, 1000, {.maxSpeed = 60});
}

void red_solo_AWP(){
    red = true;
    chassis.setPose(-48, -18, 270);
    chassis.moveToPoint(-32, -18, 1000, {.forwards = false});
    chassis.turnToPoint(-18, -26, 500, {.forwards = false});
    chassis.moveToPoint(-18, -26, 1000, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    pros::delay(250);
    intake.move(127);
    pros::delay(250);
    chassis.moveToPoint(-48, -8, 2000, {.maxSpeed = 70});
    intake_lift.set_value(true);
    chassis.waitUntilDone();
    intake_lift.set_value(false);
    pros::delay(250);
    chassis.moveToPoint(chassis.getPose().x, chassis.getPose().y-6, 1000, {.forwards = false});
    pros::delay(1500);
    chassis.turnToHeading(0, 500);
    chassis.moveToPoint(-48, 8, 1000);
    chassis.waitUntilDone();
    mogo_mech.set_value(false);
    chassis.turnToPoint(-32, 12, 500, {.forwards = false});
    intake.move(0);
    chassis.moveToPoint(-32, 12, 1500, {.forwards = false});
    chassis.turnToPoint(-20, 24, 500, {.forwards = false});
    chassis.moveToPoint(-20, 24, 1000, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    pros::delay(250);
    chassis.turnToHeading(0, 750);
    chassis.moveToPoint(-32, 42, 1000);
    intake.move(127);
    chassis.waitUntilDone();
    pros::delay(500);
    chassis.turnToPoint(-16, 6, 750);
    chassis.moveToPoint(-16, 6, 2000, {.maxSpeed = 90});
    chassis.waitUntilDone();
    intake.move(0);
}

void blue_5_ring(){
    red = false;
    chassis.setPose(48, 30, 90);

    chassis.moveToPoint(36, 30, 500, {.forwards = false});
    chassis.turnToHeading(60, 500);
    chassis.waitUntilDone();
    chassis.moveToPoint(24, 24, 1000, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    pros::delay(250);
    intake.move(127);
    pros::delay(100);
    chassis.turnToPoint(24, 52, 500);
    chassis.moveToPoint(24, 40, 500, {.minSpeed = 60});
    intake.move(127);
    chassis.waitUntilDone();
    chassis.turnToHeading(270, 500);
    chassis.waitUntilDone();
    chassis.moveToPoint(12, chassis.getPose().y-4, 1000, {.maxSpeed = 40});
    chassis.waitUntilDone();
    pros::delay(250);
    chassis.moveToPoint(24, chassis.getPose().y+6, 1000, {.forwards = false});
    chassis.turnToPoint(12, chassis.getPose().y, 500);
    chassis.moveToPoint(12, chassis.getPose().y, 2000, {.maxSpeed = 40});
    chassis.waitUntilDone();
    pros::delay(250);
    chassis.moveToPoint(24, chassis.getPose().y-2, 1000, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntilDone();
    pros::delay(500);
    chassis.turnToHeading(90, 750);
    chassis.moveToPose(42, 0, 180, 2000, {.maxSpeed = 80});
    intake_lift.set_value(true);
    chassis.waitUntilDone();
    intake_lift.set_value(false);
    pros::delay(250);
    chassis.moveToPoint(chassis.getPose().x, chassis.getPose().y-8, 1000, {.forwards = false});
    chassis.turnToPoint(8, 6, 500);
    chassis.moveToPoint(8, 6, 2000, {.maxSpeed = 60});
}

void blue_solo_AWP(){
    red = true;
    chassis.setPose(48, -18, 90);
    chassis.moveToPoint(32, -18, 1000, {.forwards = false});
    chassis.turnToPoint(18, -26, 500, {.forwards = false});
    chassis.moveToPoint(18, -26, 1000, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    pros::delay(250);
    intake.move(127);
    pros::delay(250);
    chassis.moveToPoint(48, -8, 2000, {.maxSpeed = 70});
    intake_lift.set_value(true);
    chassis.waitUntilDone();
    intake_lift.set_value(false);
    pros::delay(250);
    chassis.moveToPoint(chassis.getPose().x, chassis.getPose().y-6, 1000, {.forwards = false});
    pros::delay(1500);
    chassis.turnToHeading(0, 500);
    chassis.moveToPoint(48, 8, 1000);
    chassis.waitUntilDone();
    mogo_mech.set_value(false);
    chassis.turnToPoint(32, 12, 500, {.forwards = false});
    intake.move(0);
    chassis.moveToPoint(32, 12, 1500, {.forwards = false});
    chassis.turnToPoint(20, 24, 500, {.forwards = false});
    chassis.moveToPoint(20, 24, 1000, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    pros::delay(250);
    chassis.turnToHeading(0, 750);
    chassis.moveToPoint(32, 24, 1000);
    intake.move(127);
    chassis.waitUntilDone();
    pros::delay(500);
    chassis.turnToPoint(16, 6, 750);
    chassis.moveToPoint(16, 6, 2000, {.maxSpeed = 90});
    chassis.waitUntilDone();
    intake.move(0);
}

void red_goal_rush(){
    red = true;
    chassis.setPose(-56, -58, 90);
    
    chassis.moveToPoint(-18, -58, 750);
    chassis.swingToHeading(60, lemlib::DriveSide::LEFT, 500);
    chassis.waitUntilDone();
    arm.set_value(true);
    pros::delay(250);
    chassis.moveToPoint(chassis.getPose().x-4, chassis.getPose().y-4, 500, {.forwards = false});
    chassis.swingToHeading(90, lemlib::DriveSide::LEFT, 500);
    chassis.moveToPoint(-50, -65, 2000, {.forwards = false});
    chassis.turnToHeading(135, 500, {.minSpeed = 90});
    chassis.waitUntilDone();
    arm.set_value(false);
    pros::delay(250);
    chassis.turnToHeading(270, 750, {.direction = lemlib::AngularDirection::CCW_COUNTERCLOCKWISE});
    chassis.moveToPose(-20, -64, 270, 1000, {.forwards = false});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    pros::delay(250);
    intake.move(127);
    chassis.turnToPoint(chassis.getPose().x-6, -56, 750);
    chassis.moveToPoint(chassis.getPose().x-6, -56, 1000);
    chassis.waitUntilDone();
    pros::delay(1500);
    chassis.turnToHeading(0, 500, {.direction = lemlib::AngularDirection::CW_CLOCKWISE});
    chassis.waitUntilDone();
    mogo_mech.set_value(false);
    intake.move(-127);
    chassis.turnToHeading(180, 750, {.direction = lemlib::AngularDirection::CCW_COUNTERCLOCKWISE});
    chassis.moveToPoint(chassis.getPose().x-2, -32, 1000, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    intake.move(127);
    pros::delay(250);
    chassis.turnToPoint(-48, -18, 500);
    intake_lift.set_value(true);
    chassis.moveToPoint(-48, -19, 2000, {.minSpeed = 80});
    chassis.waitUntilDone();
    intake_lift.set_value(false);
}

void blue_goal_rush(){
    red = false;
    chassis.setPose(54, -30, 270);
    
    chassis.moveToPoint(20, -38, 1000); 
    chassis.turnToHeading(270, 500);
    chassis.waitUntilDone();
    arm.set_value(true);
    pros::delay(250);
    chassis.swingToHeading(15, lemlib::DriveSide::LEFT, 750);
    chassis.waitUntilDone();
    arm.set_value(false);
    pros::delay(250);
    chassis.turnToHeading(135, 750, {.direction = lemlib::AngularDirection::CCW_COUNTERCLOCKWISE});
    chassis.moveToPoint(chassis.getPose().x-12, chassis.getPose().y+16, 1000, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    pros::delay(250);
    chassis.turnToHeading(180, 500);
    intake.move(127);
    chassis.moveToPoint(chassis.getPose().x+6, -58, 1000);
    pros::delay(250);
    chassis.turnToHeading(270, 500);
    chassis.moveToPoint(24, chassis.getPose().y, 1000, {.forwards = false});
    chassis.waitUntilDone();
    pros::delay(750);
    chassis.turnToHeading(0, 500, {.direction = lemlib::AngularDirection::CW_CLOCKWISE});
    chassis.waitUntilDone();
    mogo_mech.set_value(false);
    intake.move(-127);
    pros::delay(250);
    chassis.turnToHeading(180, 750, {.direction = lemlib::AngularDirection::CW_CLOCKWISE});
    chassis.moveToPoint(24, -38, 750, {.forwards = false});
    chassis.moveToPoint(24, -28, 1000, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    pros::delay(250);
    chassis.moveToPoint(chassis.getPose().x, chassis.getPose().y-16, 1000);
    chassis.turnToHeading(90, 500);
    chassis.moveToPose(46, -8, 0, 2000, {.minSpeed = 90});
    intake_lift.set_value(true);
    intake.move(127);
    chassis.waitUntilDone();
    intake_lift.set_value(false);
}


void prog_skills(){
    red = true;
    chassis.setPose(-59, 24, 270);

    // grabbing mogo
    chassis.moveToPoint(-48, 24, 1000, {.forwards = false});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    pros::delay(250);

    // scoring rings
    chassis.turnToHeading(90, 750);
    chassis.waitUntilDone();
    intake.move(127);
    chassis.moveToPoint(-24, 24, 1500, {.maxSpeed = 80});
    chassis.waitUntilDone();
    pros::delay(250);
    chassis.turnToHeading(0, 500);
    chassis.moveToPoint(-24, 48, 1500, {.maxSpeed = 80});
    chassis.waitUntilDone();
    pros::delay(250);
    chassis.turnToPoint(0, 60, 500);
    chassis.moveToPoint(-4, 56, 2000);
    chassis.waitUntilDone();
    pros::delay(500);
    chassis.turnToHeading(270, 750);
    chassis.moveToPoint(-48, 60, 2000);
    chassis.waitUntilDone();
    pros::delay(250);
    chassis.turnToHeading(180, 500);
    chassis.moveToPoint(-48, 48, 1000, {.maxSpeed = 60});
    chassis.waitUntilDone();
    pros::delay(250);
    chassis.turnToHeading(270, 500);
    chassis.moveToPoint(-60, 48, 1000, {.maxSpeed = 60});
    chassis.waitUntilDone();
    pros::delay(250);
    chassis.turnToHeading(180, 500);
    chassis.moveToPoint(-60, 54, 1000, {.forwards = false});
    chassis.waitUntilDone();
    mogo_mech.set_value(false);

    // grabbing second mogo
    chassis.moveToPoint(-54, -8, 2000);
    intake.move(0);
    chassis.turnToPoint(-48, -24, 750, {.forwards = false});
    chassis.moveToPoint(-48, -24, 1000, {.maxSpeed = 60});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    pros::delay(250);

    // scoring rings
    chassis.turnToHeading(90, 750);
    chassis.moveToPoint(-24, -24, 1000);
    intake.move(127);
    chassis.waitUntilDone();
    pros::delay(250);
    chassis.turnToHeading(180, 500);
    chassis.moveToPoint(-24, -48, 1000);
    chassis.waitUntilDone();
    pros::delay(250);
    chassis.turnToPoint(0, -60, 500);
    chassis.moveToPoint(-4, -54, 1000);
    chassis.waitUntilDone();
    pros::delay(500);
    chassis.turnToHeading(270, 750);
    chassis.moveToPoint(-48, -60, 2000);
    chassis.waitUntilDone();
    pros::delay(250);
    chassis.turnToHeading(0, 500);
    chassis.moveToPoint(-48, -48, 1000);
    chassis.waitUntilDone();
    pros::delay(250);
    chassis.turnToHeading(270, 500);
    chassis.moveToPoint(-60, -48, 1000);
    chassis.waitUntilDone();
    pros::delay(250);
    chassis.turnToPoint(-60, -54, 1000, {.forwards = false});
    chassis.waitUntilDone();
    mogo_mech.set_value(false);

    // grabbing third mogo
    chassis.moveToPoint(24, -48, 2000);
    chassis.waitUntilDone();
    pros::delay(600);
    intake.move(0);
    chassis.turnToPoint(40, -30, 500);
    chassis.moveToPoint(40, -30, 1000);
    chassis.turnToHeading(180, 500);
    chassis.moveToPoint(40, -9, 1000, {.forwards = false});
    chassis.turnToPoint(48, 0, 500, {.forwards = false});
    chassis.moveToPoint(48, 0, 1000, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    pros::delay(250);

    // scoring rings
    intake.move(127);
    chassis.moveToPoint(24, -24, 1000);
    chassis.waitUntilDone();
    chassis.turnToPoint(40, 0, 750);
    chassis.moveToPoint(40, 0, 2000);
    chassis.turnToPoint(24, 24, 500);
    chassis.moveToPoint(24, 24, 2000);
    chassis.waitUntilDone();
    pros::delay(250);
    chassis.turnToHeading(0, 500);
    chassis.moveToPoint(24, 48, 1000);
    chassis.waitUntilDone();
    pros::delay(250);
    chassis.turnToHeading(90, 500);
    chassis.moveToPoint(48, 48, 2000, {.maxSpeed = 60});
    chassis.waitUntilDone();
    pros::delay(500);
    chassis.moveToPoint(60, 48, 2500, {.maxSpeed = 45});
    chassis.waitUntilDone();
    pros::delay(750);
    chassis.turnToHeading(180, 500);
    chassis.moveToPoint(60, 54, 1000, {.forwards = false});
    chassis.waitUntilDone();
    mogo_mech.set_value(false);

    // grabbing mogo
    chassis.moveToPoint(40, 16, 2000);
    chassis.turnToPoint(60, -24, 500, {.forwards = false});
    chassis.moveToPoint(60, -24, 2000, {.forwards = false, .maxSpeed = 80});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    pros::delay(250);

    // scoring mogo
    chassis.moveToPoint(60, -54, 2000, {.forwards = false});
    chassis.waitUntilDone();
    mogo_mech.set_value(false);
    chassis.moveToPoint(48, -42, 1000);
}



rd::Selector selector({
    {"Red Solo AWP", &red_solo_AWP},
    {"Red 5 Ring", &red_5_ring},
    {"Red Goal Rush", &red_goal_rush},
    {"Blue Solo AWP", &blue_solo_AWP},
    {"Blue 5 Ring", &blue_5_ring},
    {"Blue Goal Rush", &blue_goal_rush},
    {"Prog Skills", &prog_skills}
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


// initialize function. Runs on program startup
void initialize() {
    chassis.calibrate(); // calibrate sensors
    selector.focus();
    pros::Task sort_task([]{
        while(true){
            if(red){
                color_sort(RED);
                pros::delay(20);
            } else if(!red){
                color_sort(BLUE);
                pros::delay(20);
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
    selector.run_auton();
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
        console.println(std::to_string(sorter.get_proximity()));
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
                arm.set_value(false);
                b_pressed = false;
            } else{
                arm.set_value(true);
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
        console.clear();
	}
}