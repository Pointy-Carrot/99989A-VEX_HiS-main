#include "main.h"
#include "pros/misc.h"
#include <cstdio>
#include "autons.h"
#include "config.h"
#include "pros/motors.h"
#include "userfunctions.h"


ASSET(path1_txt);









// initialize function. Runs on program startup
void initialize() {
    chassis.calibrate(); // calibrate sensors
    console.focus();
    sorter.set_led_pwm(100);
    pros::Controller controller(CONTROLLER_MASTER);
    arm_motor.set_brake_mode(pros::MotorBrake::hold);
    hooks.set_brake_mode(pros::MotorBrake::hold);
    pros::Task arm_task(arm_state_switch);
    pros::Task arm_tracking_task(arm_state_tracker);
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

    // red_mogorush_sawp();
    blue_ringrush_wallstake();
    
}


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
    bool hooks_running = false;
    alliance_red();
    doinker.set_value(false);
    rush_arm.set_value(false);
    arm_motor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);


	while (true) {
		// get left y and right x positions
        int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int rightX = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
        
        console.println(std::to_string(arm_rot.get_position()));
        // move the robot
        chassis.arcade(leftY, rightX);
		// intake controls
		if(controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)){
            activate_sorter();
			intake.move(127);
            hooks.move(127);
            intake_running = true;
            hooks_running = true;
            first_time_setting_hooks = false;
		} else if(controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2)){
            deactivate_sorter();
            if(allow_score){
                if(intake_running){
                    if(hooks_running){
                        intake.move(0);
                        stop_hooks();
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

        if(controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)){
            if(arm_state_num < 3){
                arm_state_num ++;
            }
        } else if(controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)){
            if(arm_state_num > 1){
                arm_state_num --;
            }
            if(arm_state_num == 3){
                arm_state_num --;
            }
        }

        // delay to save resources
        pros::delay(25);
        console.clear();
	}
}