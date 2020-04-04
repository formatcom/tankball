#ifndef _HACK_SDL_H
#define _HACK_SDL_H

#include <linux/input.h>


// REF: ttps://github.com/emscripten-ports/SDL2/blob/master/src/joystick/linux/SDL_sysjoystick_c.h
// REF: https://github.com/emscripten-ports/SDL2/blob/master/src/joystick/SDL_sysjoystick.h
struct joystick_hwdata
{
    int fd;
    struct SDL_joylist_item *item;
    SDL_JoystickGUID guid;
    char *fname;                /* Used in haptic subsystem */

    SDL_bool ff_rumble;
    SDL_bool ff_sine;
    struct ff_effect effect;

    /* The current Linux joystick driver maps hats to two axes */
    struct hwdata_hat
    {
        int axis[2];
    } *hats;
    /* The current Linux joystick driver maps balls to two axes */
    struct hwdata_ball
    {
        int axis[2];
    } *balls;

    /* Support for the Linux 2.4 unified input interface */
    Uint8 key_map[KEY_MAX];
    Uint8 abs_map[ABS_MAX];
    struct axis_correct
    {
        int used;
        int coef[3];
    } abs_correct[ABS_MAX];

    int fresh;

    /* Steam Controller support */
    SDL_bool m_bSteamController;
    /* 4 = (ABS_HAT3X-ABS_HAT0X)/2 (see input-event-codes.h in kernel) */
    int hats_indices[4];

    /* Set when gamepad is pending removal due to ENODEV read error */
    SDL_bool gone;
};

typedef struct _SDL_JoystickAxisInfo
{
    Sint16 initial_value;       /* Initial axis state */
    Sint16 value;               /* Current axis state */
    Sint16 zero;                /* Zero point on the axis (-32768 for triggers) */
    SDL_bool has_initial_value; /* Whether we've seen a value on the axis yet */
    SDL_bool sent_initial_value; /* Whether we've sent the initial axis value */
} SDL_JoystickAxisInfo;

struct _SDL_Joystick
{
    SDL_JoystickID instance_id; /* Device instance, monotonically increasing from 0 */
    char *name;                 /* Joystick name - system dependent */
    int player_index;           /* Joystick player index, or -1 if unavailable */
    SDL_JoystickGUID guid;      /* Joystick guid */

    int naxes;                  /* Number of axis controls on the joystick */
    SDL_JoystickAxisInfo *axes;

    int nhats;                  /* Number of hats on the joystick */
    Uint8 *hats;                /* Current hat states */

    int nballs;                 /* Number of trackballs on the joystick */
    struct balldelta {
        int dx;
        int dy;
    } *balls;                   /* Current ball motion deltas */

    int nbuttons;               /* Number of buttons on the joystick */
    Uint8 *buttons;             /* Current button states */

    SDL_bool attached;
    SDL_bool is_game_controller;
    SDL_bool delayed_guide_button; /* SDL_TRUE if this device has the guide button event delayed */
    SDL_bool force_recentering; /* SDL_TRUE if this device needs to have its state reset to 0 */
    SDL_JoystickPowerLevel epowerlevel; /* power level of this joystick, SDL_JOYSTICK_POWER_UNKNOWN if not supported */
    struct _SDL_JoystickDriver *driver;

    struct joystick_hwdata *hwdata;     /* Driver dependent information */

    int ref_count;              /* Reference count for multiple opens */

    struct _SDL_Joystick *next; /* pointer to next joystick we have allocated */
};


#endif
