//=-----------------Enumeration-----------------=//
//=---------------------------------------------=//
enum robot_state
{
    //---General Cmd
    status_idle_0 = 0,
    status_idle = 83,   // S | 0x53
    status_idle_2 = 32, // Space | 0x20
    status_start = 115, // s | 0x73

    //---Home Cmd
    status_preparation_kickoff_home = 75,     // K | 0x4B
    status_preparation_freekick_home = 70,    // F | 0x46
    status_preparation_goalkick_home = 71,    // G | 0x47
    status_preparation_cornerkick_home = 67,  // C | 0x43
    status_preparation_penaltykick_home = 80, // P | 0x50
    status_preparation_throwin_home = 84,     // T | 0x54

    //---All Cmd
    status_preparation_dropball = 78, // N | 0x4E
    status_callibration = 35,         // # | 0x23
    status_park = 76,                 // L | 0x4C

    //---Away Cmd
    status_preparation_kickoff_away = 107,     // k | 0x6B
    status_preparation_freekick_away = 102,    // f | 0x66
    status_preparation_goalkick_away = 103,    // g | 0x67
    status_preparation_cornerkick_away = 99,   // c | 0x63
    status_preparation_penaltykick_away = 112, // p | 0x70
    status_preparation_throwin_away = 116,     // t | 0x74

    //---Keyboard Manual
    keyboard_forward = 106,       // j | 0x6A
    keyboard_left = 98,           // b | 0x62
    keyboard_backward = 110,      // n | 0x6E
    keyboard_right = 109,         // m | 0x6D
    keyboard_right_rotation = 48, // 0 | 0x30
    keyboard_left_rotation = 57,  // 9 | 0x39

    //---Start home Cmds
    game_kickoff_home = status_preparation_kickoff_home + 128,
    game_freekick_home = status_preparation_freekick_home + 128,
    game_goalkick_home = status_preparation_goalkick_home + 128,
    game_cornerkick_home = status_preparation_cornerkick_home + 128,
    game_penaltykick_home = status_preparation_penaltykick_home + 128,
    game_throwin_home = status_preparation_throwin_home + 128,
    game_dropball = status_preparation_dropball + 128,

    //---Start away Cmds
    game_kickoff_away = status_preparation_kickoff_away + 128,
    game_freekick_away = status_preparation_freekick_away + 128,
    game_goalkick_away = status_preparation_goalkick_away + 128,
    game_cornerkick_away = status_preparation_cornerkick_away + 128,
    game_penaltykick_away = status_preparation_penaltykick_away + 128,
    game_throwin_away = status_preparation_throwin_away + 128,

    //---Callibration use 'o'
    game_callibration = 111,

    //---Game Status
    reset_action = 0,
    is_passing = 10,
    is_passing_inside_state = 15,
    is_receiving = 20,
    is_receiving_inside_state = 25,
    is_chasing_ball = 29,
    is_catching_ball = 30,
    is_kicking_ball = 31,
    pre_passing = 35,

    //---Game Style
    style_a = 65,
    style_b = 66,
    style_c = 67,
    style_d = 68,
    style_e = 69,
};

enum obs_flags
{
    ignore_friends = 0b00001,
    near_ball = 0b00010,
    include_friends = 0b00100,
    nearest_pixel = 0b01000,
    include_ball = 0b100000,
};

enum motion_flags
{
    normal = 0b00,
    invert = 0b01,
    normal_obstacle = 0b10,
    normal_obstacle_ball = 0b100,
    custom_PID = 0b1000,
    invert_obs = 0b10000,
    normal_obstacle_friend = 0b100000
};