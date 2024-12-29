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
enum ArmState{
    UP,
    LOAD,
    DOWN,
    AUTONSCORE
};

void eject_ring();
int get_hooks_position();
int get_arm_position();
void sort_red();
void sort_blue();
void select_color_to_score();
void deactivate_sorter();
void activate_sorter();
bool detect_red();
bool detect_blue();
void alliance_red();
void alliance_blue();
void stop_hooks();
void check_arm_softstop();
void arm_state_switch();
void arm_state_tracker();
void arm_up();
void arm_load();
void arm_down();
void arm_autonscore();
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
extern int arm_softstop;
extern bool score_red;
extern bool score_blue;
extern bool intake_running;
extern float arm_pid_error;
extern bool loading_pos_activated;
extern float arm_pid_output;
extern int arm_state_num;