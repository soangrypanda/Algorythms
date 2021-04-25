#ifndef MAZE_HT_SENTRY
#define MAZE_HT_SENTRY

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <inttypes.h>

#include "utils.h"

typedef struct maze 		MAZE_S;
typedef MAZE_S*				MAZE;
typedef struct cell_coord	CELL_XY_S;
typedef CELL_XY_S*			CELL_XY;



MAZE		create_maze				(size_t w, size_t h);
void		delete_maze				(MAZE maze);
void		fill_maze				(MAZE maze, int tile);
void		build_maze				(MAZE maze);
void		print_maze				(MAZE maze);
void		prepare_correct_path	(MAZE maze);


#endif