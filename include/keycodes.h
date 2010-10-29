/*
 * \brief   Keycodes used by MTK
 *
 * This file defines the keycodes used by MTK to deliver events to its
 * client applications. The definitions are derrived from Linux-2.4.25.
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/*
 * This file is part of the MTK package, which is distributed under
 * the  terms  of the  GNU General Public Licence 2.  Please see the
 * COPYING file for details.
 */

#ifndef _MTK_KEYCODES_H_
#define _MTK_KEYCODES_H_


#define MTK_KEY_RESERVED           0
#define MTK_KEY_ESC                1
#define MTK_KEY_1                  2
#define MTK_KEY_2                  3
#define MTK_KEY_3                  4
#define MTK_KEY_4                  5
#define MTK_KEY_5                  6
#define MTK_KEY_6                  7
#define MTK_KEY_7                  8
#define MTK_KEY_8                  9
#define MTK_KEY_9                 10
#define MTK_KEY_0                 11
#define MTK_KEY_MINUS             12
#define MTK_KEY_EQUAL             13
#define MTK_KEY_BACKSPACE         14
#define MTK_KEY_TAB               15
#define MTK_KEY_Q                 16
#define MTK_KEY_W                 17
#define MTK_KEY_E                 18
#define MTK_KEY_R                 19
#define MTK_KEY_T                 20
#define MTK_KEY_Y                 21
#define MTK_KEY_U                 22
#define MTK_KEY_I                 23
#define MTK_KEY_O                 24
#define MTK_KEY_P                 25
#define MTK_KEY_LEFTBRACE         26
#define MTK_KEY_RIGHTBRACE        27
#define MTK_KEY_ENTER             28
#define MTK_KEY_LEFTCTRL          29
#define MTK_KEY_A                 30
#define MTK_KEY_S                 31
#define MTK_KEY_D                 32
#define MTK_KEY_F                 33
#define MTK_KEY_G                 34
#define MTK_KEY_H                 35
#define MTK_KEY_J                 36
#define MTK_KEY_K                 37
#define MTK_KEY_L                 38
#define MTK_KEY_SEMICOLON         39
#define MTK_KEY_APOSTROPHE        40
#define MTK_KEY_GRAVE             41
#define MTK_KEY_LEFTSHIFT         42
#define MTK_KEY_BACKSLASH         43
#define MTK_KEY_Z                 44
#define MTK_KEY_X                 45
#define MTK_KEY_C                 46
#define MTK_KEY_V                 47
#define MTK_KEY_B                 48
#define MTK_KEY_N                 49
#define MTK_KEY_M                 50
#define MTK_KEY_COMMA             51
#define MTK_KEY_DOT               52
#define MTK_KEY_SLASH             53
#define MTK_KEY_RIGHTSHIFT        54
#define MTK_KEY_KPASTERISK        55
#define MTK_KEY_LEFTALT           56
#define MTK_KEY_SPACE             57
#define MTK_KEY_CAPSLOCK          58
#define MTK_KEY_F1                59
#define MTK_KEY_F2                60
#define MTK_KEY_F3                61
#define MTK_KEY_F4                62
#define MTK_KEY_F5                63
#define MTK_KEY_F6                64
#define MTK_KEY_F7                65
#define MTK_KEY_F8                66
#define MTK_KEY_F9                67
#define MTK_KEY_F10               68
#define MTK_KEY_NUMLOCK           69
#define MTK_KEY_SCROLLLOCK        70
#define MTK_KEY_KP7               71
#define MTK_KEY_KP8               72
#define MTK_KEY_KP9               73
#define MTK_KEY_KPMINUS           74
#define MTK_KEY_KP4               75
#define MTK_KEY_KP5               76
#define MTK_KEY_KP6               77
#define MTK_KEY_KPPLUS            78
#define MTK_KEY_KP1               79
#define MTK_KEY_KP2               80
#define MTK_KEY_KP3               81
#define MTK_KEY_KP0               82
#define MTK_KEY_KPDOT             83

#define MTK_KEY_ZENKAKUHANKAKU    85
#define MTK_KEY_102ND             86
#define MTK_KEY_F11               87
#define MTK_KEY_F12               88
#define MTK_KEY_RO                89
#define MTK_KEY_KATAKANA          90
#define MTK_KEY_HIRAGANA          91
#define MTK_KEY_HENKAN            92
#define MTK_KEY_KATAKANAHIRAGANA  93
#define MTK_KEY_MUHENKAN          94
#define MTK_KEY_KPJPCOMMA         95
#define MTK_KEY_KPENTER           96
#define MTK_KEY_RIGHTCTRL         97
#define MTK_KEY_KPSLASH           98
#define MTK_KEY_SYSRQ             99
#define MTK_KEY_RIGHTALT         100
#define MTK_KEY_LINEFEED         101
#define MTK_KEY_HOME             102
#define MTK_KEY_UP               103
#define MTK_KEY_PAGEUP           104
#define MTK_KEY_LEFT             105
#define MTK_KEY_RIGHT            106
#define MTK_KEY_END              107
#define MTK_KEY_DOWN             108
#define MTK_KEY_PAGEDOWN         109
#define MTK_KEY_INSERT           110
#define MTK_KEY_DELETE           111
#define MTK_KEY_MACRO            112
#define MTK_KEY_MUTE             113
#define MTK_KEY_VOLUMEDOWN       114
#define MTK_KEY_VOLUMEUP         115
#define MTK_KEY_POWER            116
#define MTK_KEY_KPEQUAL          117
#define MTK_KEY_KPPLUSMINUS      118
#define MTK_KEY_PAUSE            119

#define MTK_KEY_KPCOMMA          121
#define MTK_KEY_HANGUEL          122
#define MTK_KEY_HANJA            123
#define MTK_KEY_YEN              124
#define MTK_KEY_LEFTMETA         125
#define MTK_KEY_RIGHTMETA        126
#define MTK_KEY_COMPOSE          127

#define MTK_KEY_STOP             128
#define MTK_KEY_AGAIN            129
#define MTK_KEY_PROPS            130
#define MTK_KEY_UNDO             131
#define MTK_KEY_FRONT            132
#define MTK_KEY_COPY             133
#define MTK_KEY_OPEN             134
#define MTK_KEY_PASTE            135
#define MTK_KEY_FIND             136
#define MTK_KEY_CUT              137
#define MTK_KEY_HELP             138
#define MTK_KEY_MENU             139
#define MTK_KEY_CALC             140
#define MTK_KEY_SETUP            141
#define MTK_KEY_SLEEP            142
#define MTK_KEY_WAKEUP           143
#define MTK_KEY_FILE             144
#define MTK_KEY_SENDFILE         145
#define MTK_KEY_DELETEFILE       146
#define MTK_KEY_XFER             147
#define MTK_KEY_PROG1            148
#define MTK_KEY_PROG2            149
#define MTK_KEY_WWW              150
#define MTK_KEY_MSDOS            151
#define MTK_KEY_COFFEE           152
#define MTK_KEY_DIRECTION        153
#define MTK_KEY_CYCLEWINDOWS     154
#define MTK_KEY_MAIL             155
#define MTK_KEY_BOOKMARKS        156
#define MTK_KEY_COMPUTER         157
#define MTK_KEY_BACK             158
#define MTK_KEY_FORWARD          159
#define MTK_KEY_CLOSECD          160
#define MTK_KEY_EJECTCD          161
#define MTK_KEY_EJECTCLOSECD     162
#define MTK_KEY_NEXTSONG         163
#define MTK_KEY_PLAYPAUSE        164
#define MTK_KEY_PREVIOUSSONG     165
#define MTK_KEY_STOPCD           166
#define MTK_KEY_RECORD           167
#define MTK_KEY_REWIND           168
#define MTK_KEY_PHONE            169
#define MTK_KEY_ISO              170
#define MTK_KEY_CONFIG           171
#define MTK_KEY_HOMEPAGE         172
#define MTK_KEY_REFRESH          173
#define MTK_KEY_EXIT             174
#define MTK_KEY_MOVE             175
#define MTK_KEY_EDIT             176
#define MTK_KEY_SCROLLUP         177
#define MTK_KEY_SCROLLDOWN       178
#define MTK_KEY_KPLEFTPAREN      179
#define MTK_KEY_KPRIGHTPAREN     180

#define MTK_KEY_F13              183
#define MTK_KEY_F14              184
#define MTK_KEY_F15              185
#define MTK_KEY_F16              186
#define MTK_KEY_F17              187
#define MTK_KEY_F18              188
#define MTK_KEY_F19              189
#define MTK_KEY_F20              190
#define MTK_KEY_F21              191
#define MTK_KEY_F22              192
#define MTK_KEY_F23              193
#define MTK_KEY_F24              194

#define MTK_KEY_PLAYCD           200
#define MTK_KEY_PAUSECD          201
#define MTK_KEY_PROG3            202
#define MTK_KEY_PROG4            203
#define MTK_KEY_SUSPEND          205
#define MTK_KEY_CLOSE            206
#define MTK_KEY_PLAY             207
#define MTK_KEY_FASTFORWARD      208
#define MTK_KEY_BASSBOOST        209
#define MTK_KEY_PRINT            210
#define MTK_KEY_HP               211
#define MTK_KEY_CAMERA           212
#define MTK_KEY_SOUND            213
#define MTK_KEY_QUESTION         214
#define MTK_KEY_EMAIL            215
#define MTK_KEY_CHAT             216
#define MTK_KEY_SEARCH           217
#define MTK_KEY_CONNECT          218
#define MTK_KEY_FINANCE          219
#define MTK_KEY_SPORT            220
#define MTK_KEY_SHOP             221
#define MTK_KEY_ALTERASE         222
#define MTK_KEY_CANCEL           223
#define MTK_KEY_BRIGHTNESSDOWN   224
#define MTK_KEY_BRIGHTNESSUP     225
#define MTK_KEY_MEDIA            226

#define MTK_BTN_MISC           0x100
#define MTK_BTN_0              0x100
#define MTK_BTN_1              0x101
#define MTK_BTN_2              0x102
#define MTK_BTN_3              0x103
#define MTK_BTN_4              0x104
#define MTK_BTN_5              0x105
#define MTK_BTN_6              0x106
#define MTK_BTN_7              0x107
#define MTK_BTN_8              0x108
#define MTK_BTN_9              0x109

#define MTK_BTN_MOUSE          0x110
#define MTK_BTN_LEFT           0x110
#define MTK_BTN_RIGHT          0x111
#define MTK_BTN_MIDDLE         0x112
#define MTK_BTN_SIDE           0x113
#define MTK_BTN_EXTRA          0x114
#define MTK_BTN_FORWARD        0x115
#define MTK_BTN_BACK           0x116
#define MTK_BTN_TASK           0x117

#define MTK_BTN_JOYSTICK       0x120
#define MTK_BTN_TRIGGER        0x120
#define MTK_BTN_THUMB          0x121
#define MTK_BTN_THUMB2         0x122
#define MTK_BTN_TOP            0x123
#define MTK_BTN_TOP2           0x124
#define MTK_BTN_PINKIE         0x125
#define MTK_BTN_BASE           0x126
#define MTK_BTN_BASE2          0x127
#define MTK_BTN_BASE3          0x128
#define MTK_BTN_BASE4          0x129
#define MTK_BTN_BASE5          0x12a
#define MTK_BTN_BASE6          0x12b
#define MTK_BTN_DEAD           0x12f

#define MTK_BTN_GAMEPAD        0x130
#define MTK_BTN_A              0x130
#define MTK_BTN_B              0x131
#define MTK_BTN_C              0x132
#define MTK_BTN_X              0x133
#define MTK_BTN_Y              0x134
#define MTK_BTN_Z              0x135
#define MTK_BTN_TL             0x136
#define MTK_BTN_TR             0x137
#define MTK_BTN_TL2            0x138
#define MTK_BTN_TR2            0x139
#define MTK_BTN_SELECT         0x13a
#define MTK_BTN_START          0x13b
#define MTK_BTN_MODE           0x13c
#define MTK_BTN_THUMBL         0x13d
#define MTK_BTN_THUMBR         0x13e

#define MTK_BTN_DIGI           0x140
#define MTK_BTN_TOOL_PEN       0x140
#define MTK_BTN_TOOL_RUBBER    0x141
#define MTK_BTN_TOOL_BRUSH     0x142
#define MTK_BTN_TOOL_PENCIL    0x143
#define MTK_BTN_TOOL_AIRBRUSH  0x144
#define MTK_BTN_TOOL_FINGER    0x145
#define MTK_BTN_TOOL_MOUSE     0x146
#define MTK_BTN_TOOL_LENS      0x147
#define MTK_BTN_TOUCH          0x14a
#define MTK_BTN_STYLUS         0x14b
#define MTK_BTN_STYLUS2        0x14c
#define MTK_BTN_TOOL_DOUBLETAP 0x14d
#define MTK_BTN_TOOL_TRIPLETAP 0x14e

#define MTK_BTN_WHEEL          0x150
#define MTK_BTN_GEAR_DOWN      0x150
#define MTK_BTN_GEAR_UP        0x151

#define MTK_KEY_OK             0x160
#define MTK_KEY_SELECT         0x161
#define MTK_KEY_GOTO           0x162
#define MTK_KEY_CLEAR          0x163
#define MTK_KEY_POWER2         0x164
#define MTK_KEY_OPTION         0x165
#define MTK_KEY_INFO           0x166
#define MTK_KEY_TIME           0x167
#define MTK_KEY_VENDOR         0x168
#define MTK_KEY_ARCHIVE        0x169
#define MTK_KEY_PROGRAM        0x16a
#define MTK_KEY_CHANNEL        0x16b
#define MTK_KEY_FAVORITES      0x16c
#define MTK_KEY_EPG            0x16d
#define MTK_KEY_PVR            0x16e
#define MTK_KEY_MHP            0x16f
#define MTK_KEY_LANGUAGE       0x170
#define MTK_KEY_TITLE          0x171
#define MTK_KEY_SUBTITLE       0x172
#define MTK_KEY_ANGLE          0x173
#define MTK_KEY_ZOOM           0x174
#define MTK_KEY_MODE           0x175
#define MTK_KEY_KEYBOARD       0x176
#define MTK_KEY_SCREEN         0x177
#define MTK_KEY_PC             0x178
#define MTK_KEY_TV             0x179
#define MTK_KEY_TV2            0x17a
#define MTK_KEY_VCR            0x17b
#define MTK_KEY_VCR2           0x17c
#define MTK_KEY_SAT            0x17d
#define MTK_KEY_SAT2           0x17e
#define MTK_KEY_CD             0x17f
#define MTK_KEY_TAPE           0x180
#define MTK_KEY_RADIO          0x181
#define MTK_KEY_TUNER          0x182
#define MTK_KEY_PLAYER         0x183
#define MTK_KEY_TEXT           0x184
#define MTK_KEY_DVD            0x185
#define MTK_KEY_AUX            0x186
#define MTK_KEY_MP3            0x187
#define MTK_KEY_AUDIO          0x188
#define MTK_KEY_VIDEO          0x189
#define MTK_KEY_DIRECTORY      0x18a
#define MTK_KEY_LIST           0x18b
#define MTK_KEY_MEMO           0x18c
#define MTK_KEY_CALENDAR       0x18d
#define MTK_KEY_RED            0x18e
#define MTK_KEY_GREEN          0x18f
#define MTK_KEY_YELLOW         0x190
#define MTK_KEY_BLUE           0x191
#define MTK_KEY_CHANNELUP      0x192
#define MTK_KEY_CHANNELDOWN    0x193
#define MTK_KEY_FIRST          0x194
#define MTK_KEY_LAST           0x195
#define MTK_KEY_AB             0x196
#define MTK_KEY_NEXT           0x197
#define MTK_KEY_RESTART        0x198
#define MTK_KEY_SLOW           0x199
#define MTK_KEY_SHUFFLE        0x19a
#define MTK_KEY_BREAK          0x19b
#define MTK_KEY_PREVIOUS       0x19c
#define MTK_KEY_DIGITS         0x19d
#define MTK_KEY_TEEN           0x19e
#define MTK_KEY_TWEN           0x19f

#define MTK_KEY_DEL_EOL        0x1c0
#define MTK_KEY_DEL_EOS        0x1c1
#define MTK_KEY_INS_LINE       0x1c2
#define MTK_KEY_DEL_LINE       0x1c3

#define MTK_KEY_MAX            0x1ff


#endif /* _MTK_KEYCODES_H_ */
