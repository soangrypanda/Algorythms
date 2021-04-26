#include "maze.h"

#define MAZE_S      struct maze
#define CELL_XY_S   struct cell_coord

MAZE_S {
    #undef MAZE_S
    cell_t *maze;
    LIST    walls;
    size_t  w;
    size_t  h;
    size_t  sx, sy, ex, ey;
};

CELL_XY_S {
    #undef CELL_XY_S
    size_t x, y;    
};


static void     find_start_and_end_points   (MAZE maze); 
static void     make_path                   (MAZE maze, size_t x1, size_t y1, size_t x2, size_t y2);


/* MAIN CELL_XY INTERFACE */
 
CELL_XY
create_cell(size_t x, size_t y)
{
    CELL_XY xy = calloc(1, sizeof(*xy));
    assert(xy != NULL);
    xy->x = x;
    xy->y = y;
    return xy;
}


/* MAIN MAZE INTERFACE */

MAZE
create_maze(size_t w, size_t h)
{
    size_t size = w * h;
    MAZE maze   = calloc(1, sizeof(*maze));
    assert(maze != NULL);
    
    cell_t *arr = calloc(size, sizeof(*arr));
    assert(arr  != NULL);
    
    maze->maze  = arr;
    maze->w     = w;
    maze->h     = h;
    maze->walls = create_list();
    maze->sx = maze->sy = maze->ex = maze-> ey = 0;
    
    return maze;
}

void
fill_maze(MAZE maze, int tile)
{
    if(maze == NULL)    return;
    
    cell_t *m_arr   = maze->maze;
    size_t m_size   = maze->w * maze->h;
    
    memset(m_arr, tile, m_size);
}

void
build_maze(MAZE maze, mazebuilder algo)
{
    cell_t *arr     = maze->maze;
    size_t w        = maze->w;
    size_t h        = maze->h;
    
    algo(maze);

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
    if(maze == NULL)    return;

    cell_t *m_arr = maze->maze;
    size_t w      = maze->w;
    size_t h      = maze->h;
    maze->w       = w;
    char *m_line  = calloc(w + 1, sizeof(*m_line));
    assert(m_line != NULL);
    
    for(size_t i = 0; i < h; ++i) {
        memcpy(m_line, &(m_arr[0 + w * i]), w); 
        printf("%s\n", m_line);
    }   
    free(m_line);
}


/* MAZE AND CELL_XY GETTERS AND SETTERS */

//yeah, returning pointer through a getter, ok
cell_t*       
get_maze_arr(MAZE maze)             { return maze->maze; }
LIST
get_maze_walllist(MAZE maze)        { return maze->walls; }
size_t
get_maze_w(MAZE maze)               { return maze->w; }
size_t  
get_maze_h(MAZE maze)               { return maze->h; }
void
get_maze_start_and_end	(MAZE maze, size_t *sx, size_t *sy, size_t *ex, size_t *ey) 
{ *sx = maze->sx; *sy = maze->sy; *ex = maze->ex; *ey = maze->ey;}

size_t
get_cell_x(CELL_XY cell)            { return cell->x; }
size_t
get_cell_y(CELL_XY cell)            { return cell->y; }
void        
set_cell_x(CELL_XY cell, size_t x)  { cell->x = x; }
void 
set_cell_y(CELL_XY cell, size_t y)  { cell->y = y; }



/* CORRECT PATH BUILER */
void
prepare_correct_path(MAZE maze)
{
    cell_t *arr     = maze->maze;
    size_t w        = maze->w;
    size_t h        = maze->h;
    size_t start_x  = maze->sx;
    size_t start_y  = maze->sy;
    size_t end_x    = maze->ex;
    size_t end_y    = maze->ey;
    
    size_t n_anchors    = 0;
    size_t size         = w * h;
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
    if      (start_x == 0)      strx = 1;
    else if (start_x == w-1)    strx = w-2;
    else                        strx = start_x;
    size_t stry;
    if      (start_y == 0)      stry = 1;
    else if (start_y == h-1)    stry = h-2;
    else                        stry = start_y;

    make_path(maze, strx, stry, axarr[0], ayarr[0]);
    
    for(size_t i = 0; i < n_anchors - 1; ++i)
        make_path(maze, axarr[i], ayarr[i], axarr[i+1], ayarr[i+1]);
    
    size_t endx;
    if      (end_x == 0)        endx = 1;
    else if (end_x == w-1)      endx = w-2;
    else                        endx = end_x;
    size_t endy;
    if      (end_y == 0)        endy = 1;
    else if (end_y == h-1)      endy = h-2;
    else                        endy = end_y;

    make_path(maze, axarr[n_anchors-1], ayarr[n_anchors-1], endx, endy);
    
    arr[w*start_y + start_x]    = START_POS;
    arr[w*end_y + end_x]        = END_POS;  
    
    free(axarr);
    free(ayarr);    
}

static void
find_start_and_end_points(MAZE maze)
{
    cell_t *arr = maze->maze;
    size_t w    = maze->w;
    size_t h    = maze->h;
    
    size_t start_x = get_rand_num(0, w-1);
    size_t start_y = get_rand_num(0, h-1);
    
    size_t end_x = get_rand_num(0, w-1);
    size_t end_y = get_rand_num(0, h-1);
    
    while(start_x == end_x && start_y == end_y) {
        end_x = get_rand_num(0, w-1);
    }
    
    int dxs = w - start_x;
    int dys = h - start_y;
    if(dxs > dys)   start_x = 0 + (w-1) * (start_x * h > start_y * w ? 0 : 1);
    else            start_y = 0 + (h-1) * (end_x * h > end_y * w ? 0 : 1);
    
    int dxe = w - end_x;
    int dye = h - end_y;
    if(dxe > dye)   end_x = 0 + (w-1) * (start_x * h > start_y * w ? 0 : 1);
    else            end_y = 0 + (h-1) * (end_x * h > end_y * w ? 0 : 1);    
    
    maze->sx = start_x;
    maze->sy = start_y;
    maze->ex = end_x;
    maze->ey = end_y;
}

static void
make_path(MAZE maze, size_t x1, size_t y1, size_t x2, size_t y2)
{
    cell_t *arr = maze->maze;
    size_t w    = maze->w;
    size_t h    = maze->h;
    size_t ys   = y1 < y2 ? y1 : y2;
    size_t ye   = y1 < y2 ? y2 : y1;
    
    for(; ys <= ye; ++ys) {
        arr[w*ys+x1] = PATH;    
    }
    
    size_t xs   = x1 < x2 ? x1 : x2;
    size_t xe   = x1 < x2 ? x2 : x1;
    
    for(; xs <=xe; ++xs)
        arr[w*y2+xs] = PATH;
}
