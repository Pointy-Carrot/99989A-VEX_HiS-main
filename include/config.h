#pragma once

#include "lemlib/chassis/chassis.hpp"
#include "lemlib/chassis/trackingWheel.hpp"
#include "pros/adi.hpp"
#include "pros/optical.hpp"
#include "pros/rotation.hpp"
#include "robodash/views/selector.hpp"


// pros constructors
extern pros::Rotation vert_tracker;
extern pros::Rotation horiz_tracker;
extern pros::Rotation arm_rot;
extern pros::Optical sorter;

extern pros::adi::DigitalOut mogo_mech;
extern pros::adi::DigitalOut doinker;
extern pros::adi::DigitalOut rush_arm;
extern pros::adi::DigitalOut ring_rush_mech;

extern pros::Motor arm_motor;
extern pros::Motor intake;
extern pros::Motor hooks;

// left motor group
extern pros::MotorGroup left_motor_group;
// right motor group
extern pros::MotorGroup right_motor_group;

extern pros::Imu imu;



// lemlib constructors
extern lemlib::Chassis chassis;
extern lemlib::Drivetrain drivetrain;
extern lemlib::TrackingWheel vertical_tracking_wheel;
extern lemlib::TrackingWheel horizontal_tracking_wheel;
extern lemlib::OdomSensors sensors;
extern lemlib::ControllerSettings lateral_controller;
extern lemlib::ControllerSettings angular_controller;

extern rd::Selector selector;
extern rd::Console console;

extern pros::Controller controller;

extern lemlib::PID arm_pid;
