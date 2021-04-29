#include "bfs.h"
#include "../../datastructures/hash/hash_table/open_adr/hto.h"


static void put_neighbours          (cell_t const *arr, CELL_XY cell, LIST frontier);
static void get_new_current_cell    (CELL_XY *xyp, LIST f, HT s);
   
void
bfs_traverse(MAZE maze)
{
    cell_t *arr = get_maze_arr(maze);
    size_t w    = get_maze_w(maze);
    size_t h    = get_maze_h(maze);
    size_t sx, sy, ex, ey;
    get_maze_start_and_end(maze, &sx, &sy, &ex, &ey);

    if(sx == ex && sy == ey)    return;

    size_t cx = sx; 
    size_t cy = sy;

    LIST frontier = create_list();
    new_ht(seen, 8); 
   
    CELL_XY current_cell = create_cell(sx, sy);
    CELL_XY target_cell  = create_cell(ex, ey); 
    size_t  cell_size    = get_xy_size(current_cell);

    char cell_not_found  = 1;

    while(cell_not_found && current_cell != NULL) {

        insert_ht_item_do(seen, current_cell, cell_size, current_cell, cell_size);
        put_neighbours(arr, current_cell, frontier); 
        get_new_current_cell(&current_cell, frontier, seen);

        if(cellcmp(current_cell, target_cell))
            cell_not_found = 0; 
    }     

    if(cell_not_found) {
        printf("cannot find path!\n");
    } 
    else
        printf("path found!\n");
}

static void
put_neighbours(cell_t const *arr, CELL_XY cell, LIST frontier)
{

}

static void
get_new_current_cell(CELL_XY *xyp, LIST f, HT s)
{
    free(*xyp);
    /* check if seen in a loop */ 
    *xyp = pop_item(f);        
}
