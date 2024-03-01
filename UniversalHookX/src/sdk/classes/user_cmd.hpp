
class c_user_cmd {
public:
    int command_number;
    int tick_count;
    float command_time;
    vector3f world_view_angles;
    char gap18[4];
    vector3f local_view_angles;
    vector3f attack_angles;
    vector3f move;
    int buttons;
    char impulse;
    short weapon_select;
    int melee_target;
    char gap4C[24];
    char head_offset;
    char gap65[11];
    vector3f camera_position;
    vector3f camera_angles;
    char gap88[4];
    int tick_smth;
    int dword90;
    int predicted_server_event_ack;
    int dword98;
    float frame_time;
};
