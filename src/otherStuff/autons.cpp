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
void red_mogorush_sawp(){
    alliance_red();
    activate_sorter();
    chassis.setPose(-50, -59, 90);
    // mogo rush
    chassis.moveToPoint(-18, -59, 1000, {.minSpeed = 127});
    rush_arm.set_value(true);
    chassis.waitUntilDone();
    chassis.turnToHeading(70, 500);
    chassis.waitUntilDone();
    rush_arm.set_value(false);
    pros::delay(125);
    // pulling back mogo
    chassis.moveToPoint(-38, -62, 1000, {.forwards = false});
    chassis.waitUntilDone();
    rush_arm.set_value(true);
    pros::delay(100);
    chassis.moveToPoint(-36, -62, 500);
    chassis.moveToPoint(-42, -60, 1000, {.forwards = false});
    chassis.waitUntilDone();
    rush_arm.set_value(false);
    pros::delay(250);
    // picking up mogo
    chassis.turnToHeading(270, 750, {.direction = lemlib::AngularDirection::CCW_COUNTERCLOCKWISE});
    chassis.moveToPoint(-12, -61, 2000, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    intake_running = true;
    pros::delay(125);
    // scoring on mogo
    chassis.moveToPoint(-40, -60, 1000);
    chassis.waitUntil(10);
    intake.move(127);
    hooks.move(127);
    // dropping mogo
    chassis.turnToHeading(90, 750);
    chassis.waitUntilDone();
    mogo_mech.set_value(false);
    hooks.move(0);
    intake_running = false;
    // picking up new mogo
    chassis.moveToPoint(-22, -48, 1000);
    chassis.turnToHeading(180, 500);
    chassis.moveToPoint(-22, -22, 1000, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    // scoring on mogo
    intake_running = true;
    chassis.turnToPoint(-52, 8, 750);
    chassis.moveToPoint(-70, 24, 2000, {.maxSpeed = 40});
    hooks.move(127);
    chassis.waitUntilDone();
    // touching evation bar
    chassis.turnToPoint(-38, 12, 500);
    chassis.moveToPoint(-38, 12, 1000);
    doinker.set_value(true);
};
void red_mogorush_elim(){};
void red_sig_sawp(){};
void blue_sawp(){};
void blue_5ring_sawp(){};
void blue_5ring_elim(){};
void blue_mogorush_sawp(){
    alliance_blue();
    activate_sorter();
    chassis.setPose(50, -37, 270);
    // mogo rush
    chassis.moveToPoint(17.25, -37, 1000, {.minSpeed = 127});
    rush_arm.set_value(true);
    chassis.waitUntilDone();
    chassis.turnToHeading(250, 500);
    chassis.waitUntilDone();
    rush_arm.set_value(false);
    pros::delay(125);
    // pulling back mogo
    chassis.moveToPoint(38, -34, 1000, {.forwards = false});
    chassis.waitUntilDone();
    rush_arm.set_value(true);
    pros::delay(100);
    chassis.moveToPoint(36, -34, 500);
    chassis.moveToPoint(42, -36, 1000, {.forwards = false});
    chassis.waitUntilDone();
    rush_arm.set_value(false);
    pros::delay(250);
    // picking up mogo
    chassis.turnToHeading(90, 750, {.direction = lemlib::AngularDirection::CW_CLOCKWISE});
    chassis.moveToPoint(12, -39, 2000, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    intake_running = true;
    pros::delay(125);
    // scoring on mogo
    chassis.moveToPoint(40, -36, 1000);
    chassis.waitUntil(10);
    intake.move(127);
    hooks.move(127);
    // dropping mogo
    chassis.turnToHeading(270, 750);
    chassis.waitUntilDone();
    mogo_mech.set_value(false);
    stop_hooks();
    intake_running = false;
    // picking up new mogo
    chassis.moveToPoint(22, -48, 1000);
    chassis.turnToHeading(180, 500);
    chassis.moveToPoint(14, -18, 1000, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntilDone();
    mogo_mech.set_value(true);
    // scoring on mogo
    intake_running = true;
    chassis.turnToPoint(52, 8, 750);
    chassis.moveToPoint(75, 24, 2000, {.maxSpeed = 40});
    hooks.move(127);
    chassis.waitUntilDone();
    // touching evation bar
    chassis.turnToPoint(36, chassis.getPose().y+3, 500);
    chassis.moveToPoint(36, chassis.getPose().y+3, 1000);
    chassis.waitUntilDone();
    rush_arm.set_value(true);
    chassis.turnToHeading(270, 500);
};
void blue_mogorush_elim(){};
void blue_sig_sawp(){};
void prog_skills(){};