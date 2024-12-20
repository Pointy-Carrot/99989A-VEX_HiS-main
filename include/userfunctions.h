#pragma once



enum ColorSort{
    SCORERED,
    SCOREBLUE,
    NOSORT
};
enum Color{
    RED,
    BLUE
};
enum Arm_State{
    UP,
    LOAD,
    DOWN
};

void eject_ring();
int get_hooks_position();
int get_arm_position();
void sort_red();
void sort_blue();
void select_color_to_score();
void arm_tracking_function();
void hooks_tracking_function();
void hooks_state_switch();
void arm_state_function();
void arm_up_control();
void arm_down_control();
void deactivate_sorter();
void activate_sorter();
bool detect_red();
bool detect_blue();
void alliance_red();
void alliance_blue();
void stop_hooks();
extern bool ejecting;
extern bool red;
extern bool arm_moving ;
extern double current_position;
extern bool auto_started;
extern int selector_value;
extern int auton_selected;
extern bool move_load_hooks;
extern bool hooks_reverse;
extern bool first_time_setting_hooks;
extern int delay_amount;
extern bool allow_score;
extern int hook_deg;
extern bool y_pressed;
extern bool b_pressed;
extern bool right_pressed;
extern bool down_pressed;
extern int arm_velocity;
extern bool hang_up;
extern int arm_softstop;
extern bool score_red;
extern bool score_blue;
extern bool intake_running;
extern bool descore_mech_activated;