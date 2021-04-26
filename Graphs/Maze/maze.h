#ifndef MAZE_HT_SENTRY
#define MAZE_HT_SENTRY


#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <inttypes.h>

#include "maze_algors.h"
#include "utils.h"


typedef struct maze         MAZE_S;
typedef MAZE_S*             MAZE;
typedef struct cell_coord   CELL_XY_S;
typedef CELL_XY_S*          CELL_XY;
typedef	char				cell_t;

typedef void (*mazebuilder) (void *maze);



enum maze_tiles { WALL          = '#', 
                  PATH          = '*', 
                  WALL_PUSHBACK = 'P',
                  START_POS     = 'S',
                  END_POS       = 'E',
                  ANCHOR        = 'A'
                };


CELL_XY     create_cell             (size_t x, size_t y);

MAZE        create_maze             (size_t w, size_t h);
void        delete_maze             (MAZE maze);
void        fill_maze               (MAZE maze, int tile);
void        build_maze              (MAZE maze, mazebuilder algo);
void        print_maze              (MAZE maze);
void        prepare_correct_path    (MAZE maze);

cell_t*     get_maze_arr            (MAZE maze);
LIST        get_maze_walllist       (MAZE maze);
size_t      get_maze_w              (MAZE maze);
size_t      get_maze_h              (MAZE maze);

size_t      get_cell_x              (CELL_XY cell);
size_t      get_cell_y              (CELL_XY cell);
void        set_cell_x              (CELL_XY cell, size_t x);
void        set_cell_y              (CELL_XY cell, size_t y);


#endif
