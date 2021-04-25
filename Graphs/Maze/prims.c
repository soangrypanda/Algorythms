#include "maze_algors.h"

#include "maze.h"
#include "utils.h"

static void
push_cell_walls(MAZE maze, size_t cx, size_t cy);
static CELL_XY
get_random_cell_from_walllist(MAZE maze);
static CELL_XY
get_rand_path_neighbour(MAZE maze, CELL_XY cell);
static void
connect_cells(MAZE maze, CELL_XY wall_c, CELL_XY path_c);



void
prims_algo(void* maze_p)
{
	MAZE maze		= (MAZE) maze_p;
	char *arr		= get_maze_arr(maze);
	size_t w		= get_maze_w(maze);
	size_t h		= get_maze_h(maze);
	LIST wall_l		= get_maze_walllist(maze);
	
	size_t x		= get_rand_num(0, w-1);
	size_t y		= get_rand_num(0, h-1);

	arr[w * y + x] = PATH;
	push_cell_walls(maze, x, y);
	
	while(get_list_len(wall_l) != 0) {
		CELL_XY wall_c = get_random_cell_from_walllist(maze);
		CELL_XY path_c = get_rand_path_neighbour(maze, wall_c);
		if(path_c == NULL) {
			free(wall_c);
		}
		else {
			connect_cells(maze, wall_c, path_c);
			push_cell_walls(maze, get_cell_x(wall_c), get_cell_y(wall_c));

			free(wall_c);
			free(path_c);
		}
	}	
	
	for(size_t i = 0; i < w; ++i) {
		if(arr[i] 			!= PATH)	arr[i] 			= WALL;
		if(arr[w*(h-1)+i] 	!= PATH) 	arr[w*(h-1)+i] 	= WALL;
	}
	
	for(size_t i = 0; i < h; ++i) {
		if(arr[w*i]			!= PATH)	arr[w*i]		= WALL;
		if(arr[w*i+w-1]		!= PATH)	arr[w*i+w-1]	= WALL;
	}	
}


static void
push_cell_walls(MAZE maze, size_t cx, size_t cy)
{
	char *arr		= get_maze_arr(maze);
	size_t w		= get_maze_w(maze);
	size_t h		= get_maze_h(maze);
	LIST wall_l		= get_maze_walllist(maze);
	size_t wc		= get_list_len(wall_l);

	
	if(cy > 1 && arr[w * (cy-2) + cx] == WALL) {
		CELL_XY xy = create_cell(cx, cy-2);
		push_item(wall_l, xy);
		wc++;
	}
	if(cy < h-2 && arr[w * (cy+2) + cx] == WALL) {
		CELL_XY xy = create_cell(cx, cy+2);
		push_item(wall_l, xy);
		wc++;
	}
	if(cx > 1 && arr[w * cy + (cx-2)] == WALL) {
		CELL_XY xy = create_cell(cx-2, cy);
		push_item(wall_l, xy);
		wc++;
	}
	if(cx < w-2 && arr[w * cy + (cx+2)] == WALL) {
		CELL_XY xy = create_cell(cx+2, cy);
		push_item(wall_l, xy);
		wc++;
	}
	
	set_list_len(wall_l, wc);
}

static CELL_XY
get_random_cell_from_walllist(MAZE maze)
{
	NODE item = pop_random_item(get_maze_walllist(maze));
	
	CELL_XY ret_data = get_item_data(item);
	
	free(item);
	
	return ret_data;
}

static CELL_XY
get_rand_path_neighbour(MAZE maze, CELL_XY cell)
{
	size_t cx = get_cell_x(cell);
	size_t cy = get_cell_y(cell);
	char *arr = get_maze_arr(maze);
	size_t w  = get_maze_w(maze);
	size_t h  = get_maze_h(maze);
	
	size_t *x_neighb = calloc(4, sizeof(*x_neighb));
	size_t *y_neighb = calloc(4, sizeof(*y_neighb));
	size_t n = 0;
		
	if(cy > 1 && arr[w * (cy-2) + cx] == PATH) {
		x_neighb[n] = cx;
		y_neighb[n] = cy - 2;
		n++;
	}
	if(cy < h-2 && arr[w * (cy+2) + cx] == PATH) {
		x_neighb[n] = cx;
		y_neighb[n] = cy + 2;
		n++;
	}
	if(cx > 1 && arr[w * cy + (cx-2)] == PATH) {
		x_neighb[n] = cx - 2;
		y_neighb[n] = cy;
		n++;
	}
	if(cx < w-2 && arr[w * cy + (cx+2)] == PATH) {
		x_neighb[n] = cx + 2;
		y_neighb[n] = cy;
		n++;
	}
	
	CELL_XY ret;
	if(n == 0)	{
		ret = NULL;
		goto clean_and_return;
	}
	
	size_t rn = get_rand_num(0, n);
	ret = create_cell(x_neighb[rn], y_neighb[rn]);
	
clean_and_return:
	free(x_neighb);
	free(y_neighb);
		
	return ret;
}

static void
connect_cells(MAZE maze, CELL_XY wall_c, CELL_XY path_c)
{
	char *arr = get_maze_arr(maze);
	size_t w  = get_maze_w(maze);
	size_t h  = get_maze_h(maze);
	size_t wx = get_cell_x(wall_c);
	size_t wy = get_cell_y(wall_c);
	size_t px = get_cell_x(path_c);
	size_t py = get_cell_y(path_c);
	size_t nx = wx; 
	size_t ny = wy;
	
	int tile_p = PATH;
	int tile_w = PATH;

	if( wx == 0 || wx == w-1 || wy == 0 || wy == h-1 )
		tile_w = WALL_PUSHBACK;
	if( px == 0 || px == w-1 || py == 0 || py == h-1 )
		tile_p = WALL_PUSHBACK;

	if(wx == px) 
		ny = wy > py ? py + 1 : wy + 1;
	else
		nx = wx > px ? px + 1 : wx + 1;
	
	arr[w * ny + nx] = tile_p;
	arr[w * wy + wx] = tile_w;
}