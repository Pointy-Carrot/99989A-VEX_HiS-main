#include "autons.h"
#include "config.h"
#include "userfunctions.h"
#include "lemlib/api.hpp"
#include "lemlib/asset.hpp"
#include "lemlib/chassis/chassis.hpp"

ASSET(red_mogorush1_txt);

void red_sawp(){};
void red_5ring_sawp(){};
void red_5ring_elim(){};
void red_mogorush_sawp(){};
void red_mogorush_elim(){
    alliance_red();
    activate_sorter();
    chassis.setPose(-49.5, -60.5, 270);
    
    chassis.follow(red_mogorush1_txt, 13, 1000, false); // rushing mogo
    chassis.waitUntil(5);
    rush_arm.set_value(true);
    rush_arm_clamp.set_value(true);
    chassis.waitUntilDone();
    pros::delay(250);
    rush_arm_clamp.set_value(false);
    chassis.moveToPose(-32, -56, 300, 1000);
    chassis.waitUntilDone();
    rush_arm_clamp.set_value(true);
    pros::delay(250);
    chassis.moveToPoint(-42, -51, 750);
    chassis.waitUntil(5);
    rush_arm.set_value(false);
    chassis.waitUntilDone();
    chassis.moveToPose(-12, -56, 270, 1000, {.forwards = false}); // picking up mogo
    rush_arm_clamp.set_value(false);
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    intake_running = true; // scoring on mogo
    pros::delay(250);
    hooks.move(127);
    chassis.moveToPose(-48, -48, 300, 1000); // placing mogo in corner
    intake.move(-127);
    chassis.turnToHeading(45, 750);
    chassis.waitUntilDone();
    mogo_mech.set_value(false);
    chassis.moveToPoint(-24, -44, 750); // picking up red ring
    intake.move(127);
    hooks.move(0);
    chassis.waitUntilDone();
    pros::delay(250);
    chassis.moveToPoint(-24, chassis.getPose().y, 750, {.forwards = false}); // picking up mogo
    chassis.turnToHeading(180, 500);
    chassis.moveToPoint(-24, -20, 750, {.forwards = false, .maxSpeed = 70});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    pros::delay(250);
    chassis.turnToPoint(-48, -32, 500);
    pros::delay(350);
    hooks.move(127);
    chassis.moveToPoint(-48, -32, 1000);
    chassis.turnToHeading(0, 500);
    mogo_mech.set_value(false);
    chassis.moveToPoint(-48, -28, 750);
    chassis.waitUntilDone();
    deactivate_sorter();
    chassis.moveToPoint(-48, 18, 2000, {.maxSpeed = 50});
    chassis.waitUntilDone();
    // might want to add variable here to check whether or not a blue ring has been sorted yet
    pros::delay(250);
    hooks.move(0);
    intake.move(0);
    mogo_mech.set_value(true);
    chassis.turnToHeading(45, 500, {.direction = lemlib::AngularDirection::CW_CLOCKWISE});
    chassis.moveToPose(-58, 0, 0, 1000, {.forwards = false});
    chassis.turnToHeading(90, 500);
    chassis.moveToPoint(-70, 0, 500, {.forwards = false});
    pros::delay(250);
    intake.move(127);
    hooks.move(127);
};
void red_sig_sawp(){};
void blue_sawp(){};
void blue_5ring_sawp(){};
void blue_5ring_elim(){};
void blue_mogorush_sawp(){};
void blue_mogorush_elim(){};
void blue_sig_sawp(){};
void prog_skills(){};