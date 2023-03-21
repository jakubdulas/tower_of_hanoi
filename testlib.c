#include "primlib.h"
#include <stdlib.h>
#include <unistd.h>

#define NUM_COLUMNS 3
#define NUM_RINGS 100
#define NUM_POSITIONS
#define COLUMN_THICKNESS 10
#define HORIZONAL_MARGIN 50
#define FLOOR_HEIGHT 50
#define VELOCITY 10
#define UP 0
#define DOWN 1
#define RIGHT 2
#define LEFT 3
#define COLUMN_HEIGHT gfx_screenHeight()/2-FLOOR_HEIGHT
#define RING_HEIGHT (COLUMN_HEIGHT-COLUMN_THICKNESS)/(NUM_RINGS+1)


void draw_obj(int cb_x, int cb_y, int width, int height, enum color c){
	gfx_filledRect(cb_x-width/2, cb_y-height, cb_x+width/2, cb_y, c);
}

void draw_column(int cb_x, int cb_y, int width, int height){
	draw_obj(cb_x, cb_y, COLUMN_THICKNESS, height, YELLOW);
	draw_obj(cb_x, cb_y, width, COLUMN_THICKNESS, YELLOW);
}

void draw_columns(int positions[NUM_COLUMNS], int column_width){
	for (int col = 0; col < NUM_COLUMNS; col++){
		draw_column(positions[col], gfx_screenHeight()-FLOOR_HEIGHT, column_width, COLUMN_HEIGHT);
	}
}

void set_columns_xs(int positions[NUM_COLUMNS], int space_between_columns){
	for (int col = 0; col < NUM_COLUMNS; col++){ 
		positions[col] = HORIZONAL_MARGIN + space_between_columns/2 + space_between_columns*col;;
	}
}

void initialize_board(int board[][NUM_RINGS][NUM_POSITIONS], int col_width, int col_positions[NUM_COLUMNS]){
	int j = 0; 
	int i = NUM_RINGS;
	for (; j < NUM_RINGS; j ++, i--){
		int delta = (col_width - COLUMN_THICKNESS) / NUM_RINGS;
		board[0][j][0] = col_positions[0];
		board[0][j][1] = gfx_screenHeight()-FLOOR_HEIGHT-RING_HEIGHT*j-COLUMN_THICKNESS;
		board[0][j][2] = col_width-delta*(NUM_RINGS-i);
	}
}

void draw_rings(int board[][NUM_RINGS][NUM_POSITIONS]){
	for (int i = 0; i < NUM_COLUMNS; i++){
		for (int j = 0; j < NUM_RINGS; j ++ ){
			if (board[i][j][0] == 0) continue; 
			draw_obj(board[i][j][0], board[i][j][1], board[i][j][2], RING_HEIGHT, RED);
		}
	}
}

void draw_floor(){
	gfx_filledRect(0, gfx_screenHeight()-FLOOR_HEIGHT, gfx_screenWidth(), gfx_screenHeight(), BLUE);
}

int top(int column[][3]){
	int i;
	for (i = 1; i < NUM_RINGS; i++){
		if (column[i][2] == 0) return i-1;
	}
	return i-1;
}

int is_move_valid(int board[][NUM_RINGS][NUM_POSITIONS], int from, int to){
	int ring_from = top(board[from]);
	int ring_to = top(board[to]);
	int ring_from_width = board[from][ring_from][2];
	int ring_to_width = board[to][ring_to][2];
	if (ring_to_width == 0) return 1;
	if (ring_from_width < ring_to_width) return 1;

	return 0;
}

int count_rings(int board[][NUM_RINGS][NUM_POSITIONS], int column){
	int i;
	for (i = 0; i < NUM_RINGS; i++){
		if (board[column][i][2] == 0) return i;
	}
	return i;
}


void display_board(int board[][NUM_RINGS][NUM_POSITIONS]){
	for (int i = 0; i < NUM_COLUMNS; i++){
		printf("Column: %d\n", i);
		for (int j = 0; j < NUM_RINGS; j ++ ){
			for (int k = 0; k < 3; k ++){
				printf("%d ", board[i][j][k]);
			}
			printf("\n");
		}
	}
	printf("\n");
}

int move_object(int from, int to, int col_positions[NUM_COLUMNS], int board[][NUM_RINGS][NUM_POSITIONS]){
	if (!is_move_valid(board, from, to)) return 0;
	
	int ring = top(board[from]);
	
	if (board[from][ring][0] == 0) return 0;
	int ring_to = top(board[to]);
	if (board[to][ring_to][0] != 0) ring_to += 1;
	int direction;
	
	
	if (board[from][ring][1] < gfx_screenHeight() - FLOOR_HEIGHT - COLUMN_HEIGHT && col_positions[to] != board[from][ring][0]){
		if (from > to){
			direction = LEFT;
		}else{
			direction = RIGHT;
		}
	}
	else if (col_positions[from] == board[from][ring][0]) {
		direction = UP;
	}else if (col_positions[to] == board[from][ring][0]) direction = DOWN;
	
	if (direction == UP){
		board[from][ring][1] -= VELOCITY;
		return 1;
	}else if (direction == DOWN){
		board[from][ring][1] += VELOCITY;
		int bottom_barrier = gfx_screenHeight()-1 - FLOOR_HEIGHT - RING_HEIGHT*ring_to- COLUMN_THICKNESS;
		if (board[from][ring][1] > bottom_barrier){
			board[to][ring_to][0] = board[from][ring][0];
			board[to][ring_to][1] = bottom_barrier;
			board[to][ring_to][2] = board[from][ring][2];
			board[from][ring][0] = 0;
			board[from][ring][1] = 0;
			board[from][ring][2] = 0;
			
			printf("Moved\n");
			//display_board(board);
				
			return 0;
		}
		return 1;
	}else if (direction == LEFT){
		if (col_positions[to] > board[from][ring][0]){
			board[from][ring][0] = col_positions[to];
		}else{
			board[from][ring][0] -= VELOCITY;
		}
	}else{
		if (col_positions[to] <= board[from][ring][0]){
			board[from][ring][0] = col_positions[to];
		}else{
			board[from][ring][0] += VELOCITY;
		}
	}
	return 1;
}

int handle_click(int code){
	if (code == SDLK_ESCAPE){
		return 0;
	}
	return 1;
}

int get_column(int code){
	int idx = code - 48;
	if (idx >= 0 && idx < NUM_COLUMNS) return idx;
	if (code == SDLK_ESCAPE) return -2;
	return -1;
}

void announce_winner(int board[][NUM_RINGS][3]){
	for (int i = 1; i<NUM_COLUMNS; i++){
		if (top(board[i]) == NUM_RINGS-1) {
			gfx_textout(gfx_screenWidth()/2, gfx_screenHeight()/2, "You Won!", GREEN);
		};
	}
}


int main(int argc, char* argv[])
{
	if (gfx_init()) {
		exit(3);
	}
	
	int playing = 1;
	int space_between_columns = (gfx_screenWidth() - 2*HORIZONAL_MARGIN)/NUM_COLUMNS;
	int column_width = space_between_columns/2;
	int columns_xs[NUM_COLUMNS];
	int board[NUM_COLUMNS][NUM_RINGS][NUM_POSITIONS] = {0};
	int is_moving = 0;
	int from = -1;
	int to = -1;
	
	
	set_columns_xs(columns_xs, space_between_columns);
	initialize_board(board, column_width, columns_xs);
	
	display_board(board);
	
	printf("%d\n", top(board[0]));
	printf("%d\n", count_rings(board, 1));
	
	while(playing){
		playing = handle_click(gfx_pollkey());
		gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1,
					   BLACK);
		
		if (is_moving){
			printf("%d -> %d\n", from, to);
			is_moving = move_object(from, to, columns_xs, board);
			if (is_moving == 0){
				from = -1;
				to = -1;
			}
		}

		announce_winner(board);	
		draw_floor();
	

		draw_columns(columns_xs, column_width);
		draw_rings(board);
	
		
		gfx_updateScreen();
		
		if (!is_moving){
			if (from == -1) from = get_column(gfx_getkey());
			else if (from == -2) playing = 0;
			else if (to == -1) {
				to = get_column(gfx_getkey());
				if (to > -1) is_moving = 1;
			}
			else if (to == -2) playing = 0;
		}
		
		SDL_Delay(10);
	}
	
	

	return 0;
}
