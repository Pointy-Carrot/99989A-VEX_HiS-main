#include "lemlib/chassis/chassis.hpp"
#include "config.h"
#include "lemlib/pid.hpp"
#include "pros/adi.hpp"
#include "pros/optical.hpp"
#include "pros/rotation.hpp"
#include "pros/misc.h"
#include "robodash/api.h"





// config
pros::Rotation vert_tracker(-6);
pros::Rotation horiz_tracker(15);
pros::Rotation arm_rot(10);
pros::Optical sorter(14);

pros::adi::DigitalOut mogo_mech('C');
pros::adi::DigitalOut doinker('A');
pros::adi::DigitalOut rush_arm('B');
pros::adi::DigitalOut ring_rush_mech('D');

pros::Motor arm_motor(-1, pros::MotorGears::red);
pros::Motor intake(-20, pros::v5::MotorGears::blue);
pros::Motor hooks(-2, pros::v5::MotorGears::blue);

// left motor group
pros::MotorGroup left_motor_group({-7, 8, -9}, pros::MotorGears::blue);
// right motor group
pros::MotorGroup right_motor_group({3, 4, -5}, pros::MotorGears::blue);

// drivetrain settings
lemlib::Drivetrain drivetrain(&left_motor_group, // left motor group
                              &right_motor_group, // right motor group
                              12.375, // 12.375 inch track width
                              lemlib::Omniwheel::NEW_325, // using new 3.25" omnis
                              450, // drivetrain rpm is 450
                              8 // horizontal drift is 2 (for now)
);

// imu
pros::Imu imu(19);

// tracking wheels
lemlib::TrackingWheel vertical_tracking_wheel(&vert_tracker, lemlib::Omniwheel::NEW_2, -1);
lemlib::TrackingWheel horizontal_tracking_wheel(&horiz_tracker, lemlib::Omniwheel::NEW_2, -1.5);

// odometry settings
lemlib::OdomSensors sensors(&vertical_tracking_wheel, // vertical tracking wheel 1, set to null
                            nullptr, // vertical tracking wheel 2, set to nullptr as we are using IMEs
                            &horizontal_tracking_wheel, // horizontal tracking wheel 1
                            nullptr, // horizontal tracking wheel 2, set to nullptr as we don't have a second one
                            &imu // inertial sensor
);

// lateral PID controller
lemlib::ControllerSettings lateral_controller(6,//16,  // proportional gain (kP)
                                              0,   // integral gain (kI)
                                              1,//74,  // derivative gain (kD)
                                              3,   // anti windup
                                              1,   // small error range, in inches
                                              100, // small error range timeout, in milliseconds
                                              3,   // large error range, in inches
                                              500, // large error range timeout, in milliseconds
                                              125   // maximum acceleration (slew)
);

// angular PID controller
lemlib::ControllerSettings angular_controller(4,   // proportional gain (kP)
                                              0,   // integral gain (kI)
                                              31,  // derivative gain (kD)
                                              3,   // anti windup
                                              1,   // small error range, in degrees
                                              100, // small error range timeout, in milliseconds
                                              3,   // large error range, in degrees
                                              500, // large error range timeout, in milliseconds
                                              0    // maximum acceleration (slew)
);

// create the chassis
lemlib::Chassis chassis(drivetrain, // drivetrain settings
                        lateral_controller, // lateral PID settings
                        angular_controller, // angular PID settings
                        sensors // odometry sensors
);

rd::Selector selector({
});

rd::Console console;

pros::Controller controller(pros::E_CONTROLLER_MASTER);

lemlib::PID arm_pid(.02, // kp
                    0, // ki
                    .0  // kd
);