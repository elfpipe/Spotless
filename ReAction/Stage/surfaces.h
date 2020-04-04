#ifndef DB101_SURFACES_H
#define DB101_SURFACES_H

#include <proto/intuition.h>

struct surface {
	struct surface *next;
	
	float x, y;
	float width, height;
	
	float original_x, original_y;
	float original_width, original_height;
	
	float target_x, target_y;
	float target_width, target_height;

	float max_width;

	int original_row, original_index;
	
	BOOL infocus;
    
	struct BitMap *bitmap;
	Object *object;
	
	//enum window_element element;
	char *name;
	
	BOOL empty_space;
};			

#define MAX_ROWS 10
#define MAX_SURFACES_PER_ROW 10

#define MAX_THUMBNAILS 5

struct grid {
	struct surface *rows[MAX_ROWS];

	int surfaces_per_row[MAX_ROWS];
	int rows_available;

	int thumbs_available;
	struct surface *thumbnails[MAX_THUMBNAILS];

	float width, height;
    
	float padding_outer;
	float padding_between_surfaces;
	float space_above;
	
	BOOL drag_to_thumbnail_area;
};

#define MAX(x, y) ((x) < (y) ? (y) : (x))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define STEPS 50.0

typedef enum {
	SPACE_SURFACE = 0,
	SPACE_EDGE = 1,
	SPACE_EMPTY_SPACE = 2,
	SPACE_SPACE_ABOVE = 3,
	SPACE_UNKNOWN = 4
} space_type;
 
struct surface *surface_at_point (struct grid *g, int x, int y, int *row, int *index);
space_type identify_area (struct grid *g, int x, int y, int *row, int *index);

struct surface *thumbnail_at_point (struct grid *g, int x, int y);
void remove_thumbnail_from_grid (struct grid *g, struct surface *th);

struct grid *alloc_grid ();
void destroy_grid (struct grid *);
void grid_adjust (struct grid *g);
void calculate_grid_dimensions (struct grid *g);
void read_surface_states (struct grid *g);
void write_result_to_prefs (struct grid *g);

struct surface *insert_empty_space_into_row (struct grid *, int, int);
void insert_grid_element_into_row (struct grid *g, struct surface *surface, int insert_row, int insert_position);
struct surface *free_surface_from_row (struct grid *g, int row, int position);
struct surface *insert_empty_row_into_grid (struct grid *g, int insert_row);
void remove_row_from_grid (struct grid *g, int row);

BOOL animate (struct grid *, void (*render_function)(struct grid *));

#endif

