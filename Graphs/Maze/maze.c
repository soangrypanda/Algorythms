#include "maze.h"


struct maze {
	char *	maze;
	LIST    walls;
	size_t	w;
	size_t	h;
	size_t 	sx, sy, ex, ey;
};

struct cell_coord {
	size_t x, y;	
};

enum maze_tiles { WALL 			= '#', 
				  PATH 			= '*', 
				  WALL_PUSHBACK = 'P',
				  START_POS		= 'S',
				  END_POS		= 'E',
				  ANCHOR		= 'A'
				};


void push_cell_walls(MAZE maze, size_t cx, size_t cy);
CELL_XY get_random_cell_from_walllist(MAZE maze);
CELL_XY get_rand_path_neighbour(MAZE maze, CELL_XY cell);
void connect_cells(MAZE maze, CELL_XY wall_c, CELL_XY path_c);
static void make_path(MAZE maze, size_t x1, size_t y1, size_t x2, size_t y2);
static void find_start_and_end_points(MAZE maze);


MAZE
create_maze(size_t w, size_t h)
{
	size_t size = w * h;
	MAZE maze	= calloc(1, sizeof(*maze));
	assert(maze != NULL);
	
	char *arr	= calloc(size, sizeof(*arr));
	assert(arr	!= NULL);
	
	maze->maze	= arr;
	maze->w		= w;
	maze->h		= h;
	maze->walls	= create_list();
	maze->sx = maze->sy = maze->ex = maze-> ey = 0;
	
	return maze;
}

void
fill_maze(MAZE maze, int tile)
{
	if(maze == NULL)	return;
	
	char *m_arr		= maze->maze;
	size_t m_size	= maze->w * maze->h;
	
	memset(m_arr, tile, m_size);
}

void
build_maze(MAZE maze)
{
	char *arr		= maze->maze;
	size_t w		= maze->w;
	size_t h		= maze->h;
	size_t x		= get_rand_num(0, w-1);
	size_t y		= get_rand_num(0, h-1);
	LIST wall_l		= maze->walls;
	
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
			push_cell_walls(maze, wall_c->x, wall_c->y);

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
	
	find_start_and_end_points(maze);
	arr[w*maze->sy + maze->sx] = START_POS;
	arr[w*maze->ey + maze->ex] = END_POS;
}

void
delete_maze(MAZE maze) 
{
	delete_list(maze->walls);
	free(maze->maze);
	free(maze);
}

void
print_maze(MAZE maze)
{
	if(maze == NULL)	return;

	char *m_arr = maze->maze;
	size_t w	= maze->w;
	size_t h 	= maze->h;
	maze->w		= w;
	char *m_line = calloc(w + 1, sizeof(*m_line));
	assert(m_line != NULL);
	
	for(size_t i = 0; i < h; ++i) {
		memcpy(m_line, &(m_arr[0 + w * i]), w);	
		printf("%s\n", m_line);
	}	
	free(m_line);
}





void
push_cell_walls(MAZE maze, size_t cx, size_t cy)
{
	LIST_S *wall_l 	= maze->walls;
	size_t wc		= get_list_len(wall_l);
	char *arr		= maze->maze;
	size_t w		= maze->w;
	size_t h		= maze->h;
	
	if(cy > 1 && arr[w * (cy-2) + cx] == WALL) {
		CELL_XY xy = calloc(1, sizeof(*xy));
		xy->x = cx; xy->y = cy - 2;
		push_item(wall_l, xy);
		wc++;
	}
	if(cy < h-2 && arr[w * (cy+2) + cx] == WALL) {
		CELL_XY xy = calloc(1, sizeof(*xy));
		xy->x = cx; xy->y = cy + 2;
		push_item(wall_l, xy);
		wc++;
	}
	if(cx > 1 && arr[w * cy + (cx-2)] == WALL) {
		CELL_XY xy = calloc(1, sizeof(*xy));
		xy->x = cx - 2; xy->y = cy;
		push_item(wall_l, xy);
		wc++;
	}
	if(cx < w-2 && arr[w * cy + (cx+2)] == WALL) {
		CELL_XY xy = calloc(1, sizeof(*xy));
		xy->x = cx + 2; xy->y = cy;
		push_item(wall_l, xy);
		wc++;
	}
	
	set_list_len(wall_l, wc);
}

CELL_XY
get_random_cell_from_walllist(MAZE maze)
{
	NODE item = pop_random_item(maze->walls);
	
	CELL_XY ret_data = get_item_data(item);
	
	free(item);
	
	return ret_data;
}

CELL_XY
get_rand_path_neighbour(MAZE maze, CELL_XY cell)
{
	size_t cx = cell->x;
	size_t cy = cell->y;
	char *arr = maze->maze;
	size_t w  = maze->w;
	size_t h  = maze->h;
	
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
	ret = calloc(1, sizeof(*ret));
	assert(ret != NULL);
	
	ret->x = x_neighb[rn];
	ret->y = y_neighb[rn];
	
clean_and_return:
	free(x_neighb);
	free(y_neighb);
		
	return ret;
}

void
connect_cells(MAZE maze, CELL_XY wall_c, CELL_XY path_c)
{
	char *arr = maze->maze;
	size_t w  = maze->w;
	size_t h  = maze->h;
	size_t wx = wall_c->x;
	size_t wy = wall_c->y;
	size_t px = path_c->x;
	size_t py = path_c->y;
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

static void
find_start_and_end_points(MAZE maze)
{
	char * arr	= maze->maze;
	size_t w	= maze->w;
	size_t h	= maze->h;
	
	size_t start_x = get_rand_num(0, w-1);
	size_t start_y = get_rand_num(0, h-1);
	
	size_t end_x = get_rand_num(0, w-1);
	size_t end_y = get_rand_num(0, h-1);
	
	while(start_x == end_x && start_y == end_y) {
		end_x = get_rand_num(0, w-1);
	}
	
	int dxs = w - start_x;
	int dys = h - start_y;
	if(dxs > dys)	start_x = 0 + (w-1) * (start_x * h > start_y * w ? 0 : 1);
	else			start_y = 0 + (h-1) * (end_x * h > end_y * w ? 0 : 1);
	
	int dxe = w - end_x;
	int dye = h - end_y;
	if(dxe > dye)	end_x = 0 + (w-1) * (start_x * h > start_y * w ? 0 : 1);
	else			end_y = 0 + (h-1) * (end_x * h > end_y * w ? 0 : 1);	
	
	maze->sx = start_x;
	maze->sy = start_y;
	maze->ex = end_x;
	maze->ey = end_y;
}

void
prepare_correct_path(MAZE maze)
{
	char * arr		= maze->maze;
	size_t w		= maze->w;
	size_t h		= maze->h;
	size_t start_x 	= maze->sx;
	size_t start_y	= maze->sy;
	size_t end_x	= maze->ex;
	size_t end_y	= maze->ey;
	
	size_t n_anchors 	= 0;
	size_t size			= w * h;
	for(; size != 0; size/=10) {
		n_anchors++;
	}
	printf("%zu\n", n_anchors);
	
	size_t *axarr = calloc(n_anchors, sizeof(*axarr));
	size_t *ayarr = calloc(n_anchors, sizeof(*ayarr));	
	
	for(size_t i = 0; i < n_anchors; ++i) {
		size_t ax = get_rand_num(1, w-2);
		size_t ay = get_rand_num(1, h-2);
		axarr[i] = ax;
		ayarr[i] = ay;
		arr[w*ay+ax] = ANCHOR;
	}
	
	size_t strx;
	if		(start_x == 0)		strx = 1;
	else if	(start_x == w-1)	strx = w-2;
	else						strx = start_x;
	size_t stry;
	if		(start_y == 0)		stry = 1;
	else if	(start_y == h-1)	stry = h-2;
	else						stry = start_y;

	make_path(maze, strx, stry, axarr[0], ayarr[0]);
	
	for(size_t i = 0; i < n_anchors - 1; ++i)
		make_path(maze, axarr[i], ayarr[i], axarr[i+1], ayarr[i+1]);
	
	size_t endx;
	if		(end_x == 0)		endx = 1;
	else if	(end_x == w-1)		endx = w-2;
	else						endx = end_x;
	size_t endy;
	if		(end_y == 0)		endy = 1;
	else if	(end_y == h-1)		endy = h-2;
	else						endy = end_y;

	//make_path(maze, strx, stry, endx, endy);
	make_path(maze, axarr[n_anchors-1], ayarr[n_anchors-1], endx, endy);
	
	arr[w*start_y + start_x] 	= START_POS;
	arr[w*end_y + end_x] 		= END_POS;	
	
	free(axarr);
	free(ayarr);	
}

static void
make_path(MAZE maze, size_t x1, size_t y1, size_t x2, size_t y2)
{
	char *arr	= maze->maze;
	size_t w	= maze->w;
	size_t h	= maze->h;
	size_t ys	= y1 < y2 ? y1 : y2;
	size_t ye	= y1 < y2 ? y2 : y1;
	
	for(; ys <= ye; ++ys) {
		arr[w*ys+x1] = PATH;	
	}
	
	size_t xs	= x1 < x2 ? x1 : x2;
	size_t xe	= x1 < x2 ? x2 : x1;
	
	for(; xs <=xe; ++xs)
		arr[w*y2+xs] = PATH;
}



int main(void)
{
	MAZE maze = create_maze(100, 50);	
	fill_maze(maze, WALL);
	build_maze(maze);
	prepare_correct_path(maze);
	print_maze(maze);
	delete_maze(maze);
}