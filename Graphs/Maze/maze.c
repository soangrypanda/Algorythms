#include "maze.h"


struct maze {
	char *	maze;
	LIST    walls;
	size_t	w;
	size_t	h;
	size_t	wc;
};

struct node {
	size_t 			x, y;
	struct node* 	next;
};

struct list {
	struct node **list;
	struct node	*last;
};

void
push_item(LIST list, size_t x, size_t y)
{
	struct node *item = calloc(1, sizeof(*item));
	assert(item != NULL);
	
	item->x = x;
	item->y = y;
	item->next = *list->list;
	*list->list = item;
}

void
delete_list_do(LIST list);
void
delete_list(LIST list)
{
	delete_list_do(list);
	free(list);
}

void
delete_list_do(LIST list)
{
	NODE *head = list->list;
	while(*head != NULL) {
		NODE tmp = *head;
		head	 = &(*head)->next;
		free(tmp);	
	}	
}

void
delete_maze(MAZE maze) 
{
	delete_list(maze->walls);
	free(maze->maze);
	free(maze);
}

enum maze_tiles { WALL = '#', PATH = '*', WALL_PUSHBACK = 'P' };
MAZE
create_maze(size_t w, size_t h)
{
	size_t size = w * h;
	MAZE maze	= calloc(1, sizeof(*maze));
	assert(maze != NULL);
	
	char *arr	= calloc(size, sizeof(*arr));
	assert(arr	!= NULL);
	
	LIST list = calloc(1, sizeof(*list));
	assert(list != NULL);		
	list->last	= NULL;
	list->list = &list->last;
	
	maze->maze	= arr;
	maze->w		= w;
	maze->h		= h;
	maze->walls	= list;
	maze->wc	= 0;
	
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
	size_t wc		= maze->wc;
	char *arr		= maze->maze;
	size_t w		= maze->w;
	size_t h		= maze->h;
	
	if(cy > 1 && arr[w * (cy-2) + cx] == WALL) {
		push_item(wall_l, cx, cy - 2);
		wc++;
	}
	if(cy < h-2 && arr[w * (cy+2) + cx] == WALL) {
		push_item(wall_l, cx, cy + 2);
		wc++;
	}
	if(cx > 1 && arr[w * cy + (cx-2)] == WALL) {
		push_item(wall_l, cx - 2, cy);
		wc++;
	}
	if(cx < w-2 && arr[w * cy + (cx+2)] == WALL) {
		push_item(wall_l, cx + 2, cy);
		wc++;
	}
	
	maze->wc = wc;
}



unsigned int seed;
static void
init_rand(void) 
{
    seed = time(NULL);
}

static size_t
get_rand_num(size_t l, size_t h)
{
    return l + rand_r(&seed) % h;
}

NODE
get_random_cell_wall_from_list(MAZE maze)
{
	size_t node_n = get_rand_num(0, maze->wc);
	NODE *tmp = maze->walls->list;
	while(node_n != 0) {
		tmp = &((*tmp)->next);
		node_n--;
	}
	NODE ret = *tmp;
	*tmp = ret->next;
	ret->next = NULL;
	
	maze->wc--;
	return ret;
}

NODE
find_rand_path_neighbour(MAZE maze, NODE cell)
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
	
	NODE ret;
	if(n == 0)	{
		ret = NULL;
		goto clean_and_return;
	}
	
	size_t rn = get_rand_num(0, n);
	ret = calloc(1, sizeof(*ret));
	assert(ret != NULL);
	
	ret->x = x_neighb[rn];
	ret->y = y_neighb[rn];
	ret->next = NULL;
	
clean_and_return:
	free(x_neighb);
	free(y_neighb);
		
	return ret;
}

void
connect_cells(MAZE maze, NODE wall_c, NODE path_c)
{
	char *arr	= maze->maze;
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

void
build_maze(MAZE maze)
{
	init_rand();
	char *arr		= maze->maze;
	size_t w		= maze->w;
	size_t h		= maze->h;
	size_t x		= get_rand_num(0, w-1);
	size_t y		= get_rand_num(0, h-1);
	
	arr[w * y + x] = PATH;
	push_cell_walls(maze, x, y);
	
	while(maze->wc != 0) {
		NODE wall_c = get_random_cell_wall_from_list(maze);
		NODE path_c = find_rand_path_neighbour(maze, wall_c);
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
}




int main(void)
{
	MAZE maze = create_maze(200, 100);	
	
	fill_maze(maze, WALL);
	build_maze(maze);
	print_maze(maze);
	delete_maze(maze);
}