#pragma once

#define GAME_TITLE "Qix"

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 800

#define GAME_SPEED 1.0

#define SHUTTLE_FILEPATH         "Resources/shuttle.png"
#define MONSTER_FILEPATH         "Resources/monster_tmp.png"

#define SPARX_DIRPATH            "Resources/sparx/"
#define SPARX_SIZE               36
#define SPARX_FRAMES             7
#define SPARX_FRAME_RATE         0.1
#define SPARX_SPEED              200.0f
#define SPARX_MAX_NUM            10
#define SPARX_GENERATE_TIME      30

#define FIELD_WIDTH 540
#define FIELD_HEIGHT 650
#define FIELD_THICK 4
#define FIELD_LEFT_OFFSET (SCREEN_WIDTH - FIELD_WIDTH - FIELD_THICK) / 2
#define FIELD_TOP_OFFSET (SCREEN_HEIGHT - FIELD_HEIGHT - 25)

#define FIELD_STATE_EMPTY 0
#define FIELD_STATE_OCCUPIED 1
#define FIELD_STATE_EDGE 2
#define FIELD_STATE_TEST1 3
#define FIELD_STATE_TEST2 -3

#define STATE_PLAYING 99
#define STATE_DIE 98
#define STATE_WIN 97
#define STATE_WIN_ANI 96
#define STATE_GAME_OVER 95

#define DIE_ANI_TIME 3

#define SHUTTLE_MOVING_SPEED 150.0f
#define SHUTTLE_SLOW_MOVING_RATE 0.5f
#define LINE_CHASING_SPEED 300.0f
#define LINE_CHASING_DELAY 0.3f

#define SHUTTLE_SIZE 30

#define SHUTTLE_STOP -1
#define SHUTTLE_MOVE_UP 0
#define SHUTTLE_MOVE_DOWN 1
#define SHUTTLE_MOVE_LEFT 2
#define SHUTTLE_MOVE_RIGHT 3

#define SHUTTLE_STATE_MOVING_MIDDLE 2
#define SHUTTLE_STATE_MOVING_EDGE 1
#define SHUTTLE_STATE_STOP 0

#define SHUTTLE_DIE_DELAY 1

#define LINE_WIDTH 3

#define MONSTER_SIZE 20
#define MONSTER_SPEED 200.0f

#define FIELD_UP 1
#define FIELD_DOWN -1
#define FIELD_LEFT 2
#define FIELD_RIGHT -2
