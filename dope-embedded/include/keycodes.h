/*
 * \brief   Keycodes used by DOpE
 *
 * This file defines the keycodes used by DOpE to deliver events to its
 * client applications. The definitions are derrived from Linux-2.4.25.
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/*
 * This file is part of the DOpE package, which is distributed under
 * the  terms  of the  GNU General Public Licence 2.  Please see the
 * COPYING file for details.
 */

#ifndef _DOPE_KEYCODES_H_
#define _DOPE_KEYCODES_H_


#define DOPE_KEY_RESERVED           0
#define DOPE_KEY_ESC                1
#define DOPE_KEY_1                  2
#define DOPE_KEY_2                  3
#define DOPE_KEY_3                  4
#define DOPE_KEY_4                  5
#define DOPE_KEY_5                  6
#define DOPE_KEY_6                  7
#define DOPE_KEY_7                  8
#define DOPE_KEY_8                  9
#define DOPE_KEY_9                 10
#define DOPE_KEY_0                 11
#define DOPE_KEY_MINUS             12
#define DOPE_KEY_EQUAL             13
#define DOPE_KEY_BACKSPACE         14
#define DOPE_KEY_TAB               15
#define DOPE_KEY_Q                 16
#define DOPE_KEY_W                 17
#define DOPE_KEY_E                 18
#define DOPE_KEY_R                 19
#define DOPE_KEY_T                 20
#define DOPE_KEY_Y                 21
#define DOPE_KEY_U                 22
#define DOPE_KEY_I                 23
#define DOPE_KEY_O                 24
#define DOPE_KEY_P                 25
#define DOPE_KEY_LEFTBRACE         26
#define DOPE_KEY_RIGHTBRACE        27
#define DOPE_KEY_ENTER             28
#define DOPE_KEY_LEFTCTRL          29
#define DOPE_KEY_A                 30
#define DOPE_KEY_S                 31
#define DOPE_KEY_D                 32
#define DOPE_KEY_F                 33
#define DOPE_KEY_G                 34
#define DOPE_KEY_H                 35
#define DOPE_KEY_J                 36
#define DOPE_KEY_K                 37
#define DOPE_KEY_L                 38
#define DOPE_KEY_SEMICOLON         39
#define DOPE_KEY_APOSTROPHE        40
#define DOPE_KEY_GRAVE             41
#define DOPE_KEY_LEFTSHIFT         42
#define DOPE_KEY_BACKSLASH         43
#define DOPE_KEY_Z                 44
#define DOPE_KEY_X                 45
#define DOPE_KEY_C                 46
#define DOPE_KEY_V                 47
#define DOPE_KEY_B                 48
#define DOPE_KEY_N                 49
#define DOPE_KEY_M                 50
#define DOPE_KEY_COMMA             51
#define DOPE_KEY_DOT               52
#define DOPE_KEY_SLASH             53
#define DOPE_KEY_RIGHTSHIFT        54
#define DOPE_KEY_KPASTERISK        55
#define DOPE_KEY_LEFTALT           56
#define DOPE_KEY_SPACE             57
#define DOPE_KEY_CAPSLOCK          58
#define DOPE_KEY_F1                59
#define DOPE_KEY_F2                60
#define DOPE_KEY_F3                61
#define DOPE_KEY_F4                62
#define DOPE_KEY_F5                63
#define DOPE_KEY_F6                64
#define DOPE_KEY_F7                65
#define DOPE_KEY_F8                66
#define DOPE_KEY_F9                67
#define DOPE_KEY_F10               68
#define DOPE_KEY_NUMLOCK           69
#define DOPE_KEY_SCROLLLOCK        70
#define DOPE_KEY_KP7               71
#define DOPE_KEY_KP8               72
#define DOPE_KEY_KP9               73
#define DOPE_KEY_KPMINUS           74
#define DOPE_KEY_KP4               75
#define DOPE_KEY_KP5               76
#define DOPE_KEY_KP6               77
#define DOPE_KEY_KPPLUS            78
#define DOPE_KEY_KP1               79
#define DOPE_KEY_KP2               80
#define DOPE_KEY_KP3               81
#define DOPE_KEY_KP0               82
#define DOPE_KEY_KPDOT             83

#define DOPE_KEY_ZENKAKUHANKAKU    85
#define DOPE_KEY_102ND             86
#define DOPE_KEY_F11               87
#define DOPE_KEY_F12               88
#define DOPE_KEY_RO                89
#define DOPE_KEY_KATAKANA          90
#define DOPE_KEY_HIRAGANA          91
#define DOPE_KEY_HENKAN            92
#define DOPE_KEY_KATAKANAHIRAGANA  93
#define DOPE_KEY_MUHENKAN          94
#define DOPE_KEY_KPJPCOMMA         95
#define DOPE_KEY_KPENTER           96
#define DOPE_KEY_RIGHTCTRL         97
#define DOPE_KEY_KPSLASH           98
#define DOPE_KEY_SYSRQ             99
#define DOPE_KEY_RIGHTALT         100
#define DOPE_KEY_LINEFEED         101
#define DOPE_KEY_HOME             102
#define DOPE_KEY_UP               103
#define DOPE_KEY_PAGEUP           104
#define DOPE_KEY_LEFT             105
#define DOPE_KEY_RIGHT            106
#define DOPE_KEY_END              107
#define DOPE_KEY_DOWN             108
#define DOPE_KEY_PAGEDOWN         109
#define DOPE_KEY_INSERT           110
#define DOPE_KEY_DELETE           111
#define DOPE_KEY_MACRO            112
#define DOPE_KEY_MUTE             113
#define DOPE_KEY_VOLUMEDOWN       114
#define DOPE_KEY_VOLUMEUP         115
#define DOPE_KEY_POWER            116
#define DOPE_KEY_KPEQUAL          117
#define DOPE_KEY_KPPLUSMINUS      118
#define DOPE_KEY_PAUSE            119

#define DOPE_KEY_KPCOMMA          121
#define DOPE_KEY_HANGUEL          122
#define DOPE_KEY_HANJA            123
#define DOPE_KEY_YEN              124
#define DOPE_KEY_LEFTMETA         125
#define DOPE_KEY_RIGHTMETA        126
#define DOPE_KEY_COMPOSE          127

#define DOPE_KEY_STOP             128
#define DOPE_KEY_AGAIN            129
#define DOPE_KEY_PROPS            130
#define DOPE_KEY_UNDO             131
#define DOPE_KEY_FRONT            132
#define DOPE_KEY_COPY             133
#define DOPE_KEY_OPEN             134
#define DOPE_KEY_PASTE            135
#define DOPE_KEY_FIND             136
#define DOPE_KEY_CUT              137
#define DOPE_KEY_HELP             138
#define DOPE_KEY_MENU             139
#define DOPE_KEY_CALC             140
#define DOPE_KEY_SETUP            141
#define DOPE_KEY_SLEEP            142
#define DOPE_KEY_WAKEUP           143
#define DOPE_KEY_FILE             144
#define DOPE_KEY_SENDFILE         145
#define DOPE_KEY_DELETEFILE       146
#define DOPE_KEY_XFER             147
#define DOPE_KEY_PROG1            148
#define DOPE_KEY_PROG2            149
#define DOPE_KEY_WWW              150
#define DOPE_KEY_MSDOS            151
#define DOPE_KEY_COFFEE           152
#define DOPE_KEY_DIRECTION        153
#define DOPE_KEY_CYCLEWINDOWS     154
#define DOPE_KEY_MAIL             155
#define DOPE_KEY_BOOKMARKS        156
#define DOPE_KEY_COMPUTER         157
#define DOPE_KEY_BACK             158
#define DOPE_KEY_FORWARD          159
#define DOPE_KEY_CLOSECD          160
#define DOPE_KEY_EJECTCD          161
#define DOPE_KEY_EJECTCLOSECD     162
#define DOPE_KEY_NEXTSONG         163
#define DOPE_KEY_PLAYPAUSE        164
#define DOPE_KEY_PREVIOUSSONG     165
#define DOPE_KEY_STOPCD           166
#define DOPE_KEY_RECORD           167
#define DOPE_KEY_REWIND           168
#define DOPE_KEY_PHONE            169
#define DOPE_KEY_ISO              170
#define DOPE_KEY_CONFIG           171
#define DOPE_KEY_HOMEPAGE         172
#define DOPE_KEY_REFRESH          173
#define DOPE_KEY_EXIT             174
#define DOPE_KEY_MOVE             175
#define DOPE_KEY_EDIT             176
#define DOPE_KEY_SCROLLUP         177
#define DOPE_KEY_SCROLLDOWN       178
#define DOPE_KEY_KPLEFTPAREN      179
#define DOPE_KEY_KPRIGHTPAREN     180

#define DOPE_KEY_F13              183
#define DOPE_KEY_F14              184
#define DOPE_KEY_F15              185
#define DOPE_KEY_F16              186
#define DOPE_KEY_F17              187
#define DOPE_KEY_F18              188
#define DOPE_KEY_F19              189
#define DOPE_KEY_F20              190
#define DOPE_KEY_F21              191
#define DOPE_KEY_F22              192
#define DOPE_KEY_F23              193
#define DOPE_KEY_F24              194

#define DOPE_KEY_PLAYCD           200
#define DOPE_KEY_PAUSECD          201
#define DOPE_KEY_PROG3            202
#define DOPE_KEY_PROG4            203
#define DOPE_KEY_SUSPEND          205
#define DOPE_KEY_CLOSE            206
#define DOPE_KEY_PLAY             207
#define DOPE_KEY_FASTFORWARD      208
#define DOPE_KEY_BASSBOOST        209
#define DOPE_KEY_PRINT            210
#define DOPE_KEY_HP               211
#define DOPE_KEY_CAMERA           212
#define DOPE_KEY_SOUND            213
#define DOPE_KEY_QUESTION         214
#define DOPE_KEY_EMAIL            215
#define DOPE_KEY_CHAT             216
#define DOPE_KEY_SEARCH           217
#define DOPE_KEY_CONNECT          218
#define DOPE_KEY_FINANCE          219
#define DOPE_KEY_SPORT            220
#define DOPE_KEY_SHOP             221
#define DOPE_KEY_ALTERASE         222
#define DOPE_KEY_CANCEL           223
#define DOPE_KEY_BRIGHTNESSDOWN   224
#define DOPE_KEY_BRIGHTNESSUP     225
#define DOPE_KEY_MEDIA            226

#define DOPE_BTN_MISC           0x100
#define DOPE_BTN_0              0x100
#define DOPE_BTN_1              0x101
#define DOPE_BTN_2              0x102
#define DOPE_BTN_3              0x103
#define DOPE_BTN_4              0x104
#define DOPE_BTN_5              0x105
#define DOPE_BTN_6              0x106
#define DOPE_BTN_7              0x107
#define DOPE_BTN_8              0x108
#define DOPE_BTN_9              0x109

#define DOPE_BTN_MOUSE          0x110
#define DOPE_BTN_LEFT           0x110
#define DOPE_BTN_RIGHT          0x111
#define DOPE_BTN_MIDDLE         0x112
#define DOPE_BTN_SIDE           0x113
#define DOPE_BTN_EXTRA          0x114
#define DOPE_BTN_FORWARD        0x115
#define DOPE_BTN_BACK           0x116
#define DOPE_BTN_TASK           0x117

#define DOPE_BTN_JOYSTICK       0x120
#define DOPE_BTN_TRIGGER        0x120
#define DOPE_BTN_THUMB          0x121
#define DOPE_BTN_THUMB2         0x122
#define DOPE_BTN_TOP            0x123
#define DOPE_BTN_TOP2           0x124
#define DOPE_BTN_PINKIE         0x125
#define DOPE_BTN_BASE           0x126
#define DOPE_BTN_BASE2          0x127
#define DOPE_BTN_BASE3          0x128
#define DOPE_BTN_BASE4          0x129
#define DOPE_BTN_BASE5          0x12a
#define DOPE_BTN_BASE6          0x12b
#define DOPE_BTN_DEAD           0x12f

#define DOPE_BTN_GAMEPAD        0x130
#define DOPE_BTN_A              0x130
#define DOPE_BTN_B              0x131
#define DOPE_BTN_C              0x132
#define DOPE_BTN_X              0x133
#define DOPE_BTN_Y              0x134
#define DOPE_BTN_Z              0x135
#define DOPE_BTN_TL             0x136
#define DOPE_BTN_TR             0x137
#define DOPE_BTN_TL2            0x138
#define DOPE_BTN_TR2            0x139
#define DOPE_BTN_SELECT         0x13a
#define DOPE_BTN_START          0x13b
#define DOPE_BTN_MODE           0x13c
#define DOPE_BTN_THUMBL         0x13d
#define DOPE_BTN_THUMBR         0x13e

#define DOPE_BTN_DIGI           0x140
#define DOPE_BTN_TOOL_PEN       0x140
#define DOPE_BTN_TOOL_RUBBER    0x141
#define DOPE_BTN_TOOL_BRUSH     0x142
#define DOPE_BTN_TOOL_PENCIL    0x143
#define DOPE_BTN_TOOL_AIRBRUSH  0x144
#define DOPE_BTN_TOOL_FINGER    0x145
#define DOPE_BTN_TOOL_MOUSE     0x146
#define DOPE_BTN_TOOL_LENS      0x147
#define DOPE_BTN_TOUCH          0x14a
#define DOPE_BTN_STYLUS         0x14b
#define DOPE_BTN_STYLUS2        0x14c
#define DOPE_BTN_TOOL_DOUBLETAP 0x14d
#define DOPE_BTN_TOOL_TRIPLETAP 0x14e

#define DOPE_BTN_WHEEL          0x150
#define DOPE_BTN_GEAR_DOWN      0x150
#define DOPE_BTN_GEAR_UP        0x151

#define DOPE_KEY_OK             0x160
#define DOPE_KEY_SELECT         0x161
#define DOPE_KEY_GOTO           0x162
#define DOPE_KEY_CLEAR          0x163
#define DOPE_KEY_POWER2         0x164
#define DOPE_KEY_OPTION         0x165
#define DOPE_KEY_INFO           0x166
#define DOPE_KEY_TIME           0x167
#define DOPE_KEY_VENDOR         0x168
#define DOPE_KEY_ARCHIVE        0x169
#define DOPE_KEY_PROGRAM        0x16a
#define DOPE_KEY_CHANNEL        0x16b
#define DOPE_KEY_FAVORITES      0x16c
#define DOPE_KEY_EPG            0x16d
#define DOPE_KEY_PVR            0x16e
#define DOPE_KEY_MHP            0x16f
#define DOPE_KEY_LANGUAGE       0x170
#define DOPE_KEY_TITLE          0x171
#define DOPE_KEY_SUBTITLE       0x172
#define DOPE_KEY_ANGLE          0x173
#define DOPE_KEY_ZOOM           0x174
#define DOPE_KEY_MODE           0x175
#define DOPE_KEY_KEYBOARD       0x176
#define DOPE_KEY_SCREEN         0x177
#define DOPE_KEY_PC             0x178
#define DOPE_KEY_TV             0x179
#define DOPE_KEY_TV2            0x17a
#define DOPE_KEY_VCR            0x17b
#define DOPE_KEY_VCR2           0x17c
#define DOPE_KEY_SAT            0x17d
#define DOPE_KEY_SAT2           0x17e
#define DOPE_KEY_CD             0x17f
#define DOPE_KEY_TAPE           0x180
#define DOPE_KEY_RADIO          0x181
#define DOPE_KEY_TUNER          0x182
#define DOPE_KEY_PLAYER         0x183
#define DOPE_KEY_TEXT           0x184
#define DOPE_KEY_DVD            0x185
#define DOPE_KEY_AUX            0x186
#define DOPE_KEY_MP3            0x187
#define DOPE_KEY_AUDIO          0x188
#define DOPE_KEY_VIDEO          0x189
#define DOPE_KEY_DIRECTORY      0x18a
#define DOPE_KEY_LIST           0x18b
#define DOPE_KEY_MEMO           0x18c
#define DOPE_KEY_CALENDAR       0x18d
#define DOPE_KEY_RED            0x18e
#define DOPE_KEY_GREEN          0x18f
#define DOPE_KEY_YELLOW         0x190
#define DOPE_KEY_BLUE           0x191
#define DOPE_KEY_CHANNELUP      0x192
#define DOPE_KEY_CHANNELDOWN    0x193
#define DOPE_KEY_FIRST          0x194
#define DOPE_KEY_LAST           0x195
#define DOPE_KEY_AB             0x196
#define DOPE_KEY_NEXT           0x197
#define DOPE_KEY_RESTART        0x198
#define DOPE_KEY_SLOW           0x199
#define DOPE_KEY_SHUFFLE        0x19a
#define DOPE_KEY_BREAK          0x19b
#define DOPE_KEY_PREVIOUS       0x19c
#define DOPE_KEY_DIGITS         0x19d
#define DOPE_KEY_TEEN           0x19e
#define DOPE_KEY_TWEN           0x19f

#define DOPE_KEY_DEL_EOL        0x1c0
#define DOPE_KEY_DEL_EOS        0x1c1
#define DOPE_KEY_INS_LINE       0x1c2
#define DOPE_KEY_DEL_LINE       0x1c3

#define DOPE_KEY_MAX            0x1ff


#endif /* _DOPE_KEYCODES_H_ */
