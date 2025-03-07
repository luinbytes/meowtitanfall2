
enum ClientFrameStage_t {
    FRAME_UNDEFINED = -1, // (haven't run any frames yet)
    FRAME_START,

    // A network packet is being recieved
    FRAME_NET_UPDATE_START,
    // Data has been received and we're going to start calling PostDataUpdate
    FRAME_NET_UPDATE_POSTDATAUPDATE_START,
    // Data has been received and we've called PostDataUpdate on all data recipients
    FRAME_NET_UPDATE_POSTDATAUPDATE_END,
    // We've received all packets, we can now do interpolation, prediction, etc..
    FRAME_NET_UPDATE_END,

    // We're about to start rendering the scene
    FRAME_RENDER_START,
    // We've finished rendering the scene.
    FRAME_RENDER_END,

    FRAME_NET_FULL_FRAME_UPDATE_ON_REMOVE
};

#define IN_ATTACK (1 << 0)
#define IN_JUMP (1 << 1)
#define IN_DUCK (1 << 2)
#define IN_FORWARD (1 << 3)
#define IN_BACK (1 << 4)
#define IN_USE (1 << 5)
#define IN_PAUSEMENU (1 << 6)
#define IN_LEFT (1 << 7)
#define IN_RIGHT (1 << 8)
#define IN_MOVELEFT (1 << 9)
#define IN_MOVERIGHT (1 << 10)
#define IN_EXIT (1 << 11) //(Exiting a vehicle)
#define IN_RELOAD (1 << 12)
#define IN_WEAPON_PICKUP (1 << 13)
#define IN_WEAPON_DISCARD (1 << 14)
#define IN_SPEED (1 << 15) //(Sprint)
#define IN_WEAPON_PICKUP_AND_CYCLE (1 << 16)
#define IN_ZOOM (1 << 17) //(Aim down-sight)
#define IN_ZOOM_TOGGLE (1 << 18)
#define IN_MELEE (1 << 19)
#define IN_WEAPON_CYCLE (1 << 20)
#define IN_OFFHAND0 (1 << 21)
#define IN_OFFHAND1 (1 << 22)
#define IN_OFFHAND2 (1 << 23)
#define IN_OFFHAND3 (1 << 24)
#define IN_OFFHAND4 (1 << 25)
#define IN_OFFHAND_QUICK (1 << 26)
#define IN_DUCKTOGGLE (1 << 27)
#define IN_USE_AND_RELOAD (1 << 28)
#define IN_DODGE (1 << 29)
#define IN_VARIABLE_SCOPE_TOGGLE (1 << 30)
#define IN_PING (1 << 31)

#define CONTENTS_EMPTY 0x0
#define CONTENTS_SOLID 0x1
#define CONTENTS_WINDOW 0x2
#define CONTENTS_AUX 0x4
#define CONTENTS_GRATE 0x8
#define CONTENTS_SLIME 0x10
#define CONTENTS_WATER 0x20
#define CONTENTS_WINDOW_NOCOLLIDE 0x40
#define CONTENTS_OPAQUE 0x80
#define CONTENTS_TESTFOGVOLUME 0x100
#define CONTENTS_PHYSICSCLIP 0x200
#define CONTENTS_SOUNDTRIGGER 0x400
#define CONTENTS_NOGRAPPLE 0x800
#define CONTENTS_OCCLUDESOUND 0x1000
#define CONTENTS_IGNORE_NODRAW_OPAQUE 0x2000
#define CONTENTS_MOVEABLE 0x4000
#define CONTENTS_TEST_SOLID_BODY_SHOT 0x8000
#define CONTENTS_PLAYERCLIP 0x10000
#define CONTENTS_MONSTERCLIP 0x20000
#define CONTENTS_OPERATOR_FLOOR 0x40000
#define CONTENTS_BLOCKLOS 0x80000
#define CONTENTS_NOCLIMB 0x100000
#define CONTENTS_TITANCLIP 0x200000
#define CONTENTS_BULLETCLIP 0x400000
#define CONTENTS_OPERATORCLIP 0x800000
#define CONTENTS_NOAIRDROP 0x1000000
#define CONTENTS_MONSTER 0x2000000
#define CONTENTS_DEBRIS 0x4000000
#define CONTENTS_DETAIL 0x8000000
#define CONTENTS_TRANSLUCENT 0x10000000
#define CONTENTS_HITBOX 0x40000000

#define TRACE_CONTENTS_OPERATOR_FLOOR (CONTENTS_OPERATOR_FLOOR)
#define TRACE_MASK_SOLID (CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_GRATE | CONTENTS_PHYSICSCLIP | CONTENTS_MOVEABLE | CONTENTS_MONSTER)
#define TRACE_MASK_PLAYERSOLID (CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_GRATE | CONTENTS_MOVEABLE | CONTENTS_PLAYERCLIP | CONTENTS_MONSTER)
#define TRACE_MASK_TITANSOLID (CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_GRATE | CONTENTS_MOVEABLE | CONTENTS_TITANCLIP | CONTENTS_MONSTER)
#define TRACE_MASK_NPCSOLID (CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_GRATE | CONTENTS_PHYSICSCLIP | CONTENTS_MOVEABLE | CONTENTS_MONSTERCLIP | CONTENTS_MONSTER)
#define TRACE_MASK_NPCFLUID (CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_MOVEABLE | CONTENTS_MONSTERCLIP | CONTENTS_MONSTER)
#define TRACE_MASK_SOLID_BRUSHONLY (CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_GRATE | CONTENTS_PHYSICSCLIP | CONTENTS_MOVEABLE)
#define TRACE_MASK_PLAYERSOLID_BRUSHONLY (CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_GRATE | CONTENTS_MOVEABLE | CONTENTS_PLAYERCLIP)
#define TRACE_MASK_NPCSOLID_BRUSHONLY (CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_GRATE | CONTENTS_MOVEABLE | CONTENTS_MONSTERCLIP)
#define TRACE_MASK_WATER (CONTENTS_SLIME | CONTENTS_WATER)
#define TRACE_MASK_OPAQUE (CONTENTS_SOLID | CONTENTS_OPAQUE | CONTENTS_MOVEABLE)
#define TRACE_MASK_OPAQUE_AND_NPCS (CONTENTS_SOLID | CONTENTS_OPAQUE | CONTENTS_MOVEABLE | CONTENTS_MONSTER)
#define TRACE_MASK_BLOCKLOS (CONTENTS_SOLID | CONTENTS_OPAQUE | CONTENTS_MOVEABLE | CONTENTS_BLOCKLOS)
#define TRACE_MASK_BLOCKLOS_AND_NPCS (CONTENTS_SOLID | CONTENTS_OPAQUE | CONTENTS_MOVEABLE | CONTENTS_BLOCKLOS | CONTENTS_MONSTER)
#define TRACE_MASK_VISIBLE (CONTENTS_SOLID | CONTENTS_OPAQUE | CONTENTS_IGNORE_NODRAW_OPAQUE | CONTENTS_MOVEABLE)
#define TRACE_MASK_VISIBLE_AND_NPCS (CONTENTS_SOLID | CONTENTS_OPAQUE | CONTENTS_IGNORE_NODRAW_OPAQUE | CONTENTS_MOVEABLE | CONTENTS_MONSTER)
#define TRACE_MASK_SHOT (CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_SLIME | CONTENTS_WATER | CONTENTS_MOVEABLE | CONTENTS_BULLETCLIP | CONTENTS_MONSTER | CONTENTS_DEBRIS | CONTENTS_HITBOX)
#define TRACE_MASK_SHOT_BRUSHONLY (CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_SLIME | CONTENTS_WATER | CONTENTS_MOVEABLE | CONTENTS_BULLETCLIP | CONTENTS_DEBRIS)
#define TRACE_MASK_GRENADE (CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_GRATE | CONTENTS_MOVEABLE | CONTENTS_BULLETCLIP | CONTENTS_MONSTER | CONTENTS_DEBRIS | CONTENTS_HITBOX)
#define TRACE_MASK_SHOT_HULL (CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_GRATE | CONTENTS_MOVEABLE | CONTENTS_BULLETCLIP | CONTENTS_MONSTER | CONTENTS_DEBRIS)
#define TRACE_MASK_NPCWORLDSTATIC (CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_GRATE | CONTENTS_MONSTERCLIP)

enum SIGNONSTATE : int {
    SIGNONSTATE_NONE = 0,
    SIGNONSTATE_CHALLENGE = 1,
    SIGNONSTATE_CONNECTED = 2,
    SIGNONSTATE_NEW = 3,
    SIGNONSTATE_PRESPAWN = 4,
    SIGNONSTATE_GETTINGDATA = 5,
    SIGNONSTATE_SPAWN = 6,
    SIGNONSTATE_FIRSTSNAP = 7,
    SIGNONSTATE_FULL = 8,
    SIGNONSTATE_MAYRECONNECT = 9,
    SIGNONSTATE_CHANGELEVEL = 10,
};

// m_lifeState values
#define LIFE_ALIVE 0 // alive
#define LIFE_DYING 1 // playing death animation or still falling off of a ledge waiting to hit ground
#define LIFE_DEAD 2  // dead. lying still.
#define LIFE_RESPAWNABLE 3
#define LIFE_DISCARDBODY 4

#define FL_ONGROUND (1 << 0)   // At rest / on the ground
#define FL_DUCKING (1 << 1)    // Player flag -- Player is fully crouched
#define FL_WATERJUMP (1 << 2)  // player jumping out of water
#define FL_ONTRAIN (1 << 3)    // Player is _controlling_ a train, so movement commands should be ignored on client during prediction.
#define FL_INRAIN (1 << 4)     // Indicates the entity is standing in rain
#define FL_FROZEN (1 << 5)     // Player is frozen for 3rd person camera
#define FL_ATCONTROLS (1 << 6) // Player can't move, but keeps key inputs for controlling another entity
#define FL_CLIENT (1 << 7)     // Is a player
#define FL_FAKECLIENT (1 << 8) // Fake client, simulated server side; don't send network messages to them
// NON-PLAYER SPECIFIC (i.e., not used by GameMovement or the client .dll ) -- Can still be applied to players, though
#define FL_INWATER (1 << 9)    // In water#define	FL_ONGROUND				(1<<0)	// At rest / on the ground
#define FL_DUCKING (1 << 1)    // Player flag -- Player is fully crouched
#define FL_WATERJUMP (1 << 2)  // player jumping out of water
#define FL_ONTRAIN (1 << 3)    // Player is _controlling_ a train, so movement commands should be ignored on client during prediction.
#define FL_INRAIN (1 << 4)     // Indicates the entity is standing in rain
#define FL_FROZEN (1 << 5)     // Player is frozen for 3rd person camera
#define FL_ATCONTROLS (1 << 6) // Player can't move, but keeps key inputs for controlling another entity
#define FL_CLIENT (1 << 7)     // Is a player
#define FL_FAKECLIENT (1 << 8) // Fake client, simulated server side; don't send network messages to them
// NON-PLAYER SPECIFIC (i.e., not used by GameMovement or the client .dll ) -- Can still be applied to players, though
#define FL_INWATER (1 << 9) // In water

#define MAX_SPLITSCREEN_CLIENT_BITS 2
#define MAX_SPLITSCREEN_CLIENTS (1 << MAX_SPLITSCREEN_CLIENT_BITS) // 4

enum {
    MAX_JOYSTICKS = MAX_SPLITSCREEN_CLIENTS,
    MOUSE_BUTTON_COUNT = 5,
};

enum JoystickAxis_t {
    JOY_AXIS_X = 0,
    JOY_AXIS_Y,
    JOY_AXIS_Z,
    JOY_AXIS_R,
    JOY_AXIS_U,
    JOY_AXIS_V,
    MAX_JOYSTICK_AXES,
};

enum {
    JOYSTICK_MAX_BUTTON_COUNT = 32,
    JOYSTICK_POV_BUTTON_COUNT = 4,
    JOYSTICK_AXIS_BUTTON_COUNT = MAX_JOYSTICK_AXES * 2,
};

#define JOYSTICK_BUTTON_INTERNAL(_joystick, _button) (JOYSTICK_FIRST_BUTTON + ((_joystick)*JOYSTICK_MAX_BUTTON_COUNT) + (_button))
#define JOYSTICK_POV_BUTTON_INTERNAL(_joystick, _button) (JOYSTICK_FIRST_POV_BUTTON + ((_joystick)*JOYSTICK_POV_BUTTON_COUNT) + (_button))
#define JOYSTICK_AXIS_BUTTON_INTERNAL(_joystick, _button) (JOYSTICK_FIRST_AXIS_BUTTON + ((_joystick)*JOYSTICK_AXIS_BUTTON_COUNT) + (_button))

#define JOYSTICK_BUTTON(_joystick, _button) ((ButtonCode_t)JOYSTICK_BUTTON_INTERNAL(_joystick, _button))
#define JOYSTICK_POV_BUTTON(_joystick, _button) ((ButtonCode_t)JOYSTICK_POV_BUTTON_INTERNAL(_joystick, _button))
#define JOYSTICK_AXIS_BUTTON(_joystick, _button) ((ButtonCode_t)JOYSTICK_AXIS_BUTTON_INTERNAL(_joystick, _button))

#define JOYSTICK_AXIS_INTERNAL(_joystick, _axis) (JOYSTICK_FIRST_AXIS + ((_joystick)*MAX_JOYSTICK_AXES) + (_axis))
#define JOYSTICK_AXIS(_joystick, _axis) ((AnalogCode_t)JOYSTICK_AXIS_INTERNAL(_joystick, _axis))
