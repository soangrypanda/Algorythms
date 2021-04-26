#include "maze.c"
#include "utils.c"



int
BFS(MAZE maze)
{
	cell_t *arr = get_maze_arr(maze);
	size_t sx, sy, ex, ey;
	get_maze_start_and_end(maze, &sx, &sy, &ex, &ey);
	
	size_t x, y;
	x = y = 0;
	
	LIST frontier = create_list();
	push_item(create_cell(x, y));
	
	
	
}