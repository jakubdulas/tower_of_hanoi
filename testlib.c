#include "primlib.h"
#include <stdlib.h>
#include <unistd.h>

#define NUM_COLUMNS 3
#define NUM_RINGS 5
#define RING_HEIGHT 10
#define HORIZONAL_MARGIN 50
#define FLOOR_HEIGHT 50
#define VELOCITY 10
#define UP 0
#define DOWN 1
#define RIGHT 2
#define LEFT 3


void draw_obj(int cb_x, int cb_y, int width, int height, enum color c){
	gfx_filledRect(cb_x-width/2, cb_y-height, cb_x+width/2, cb_y, c);
}

void draw_column(int cb_x, int cb_y, int width, int height){
	draw_obj(cb_x, cb_y, RING_HEIGHT, height, YELLOW);
	draw_obj(cb_x, cb_y, width, RING_HEIGHT, YELLOW);
}

void draw_columns(int positions[NUM_COLUMNS], int column_width, int column_height){
	for (int col = 0; col < NUM_COLUMNS; col++){
		draw_column(positions[col], gfx_screenHeight()-FLOOR_HEIGHT, column_width, column_height);
	}
}

void set_columns_xs(int positions[NUM_COLUMNS], int space_between_columns){
	for (int col = 0; col < NUM_COLUMNS; col++){ 
		positions[col] = HORIZONAL_MARGIN + space_between_columns/2 + space_between_columns*col;;
	}
}

void initialize_board(int board[][NUM_RINGS][3], int col_width, int col_positions[NUM_COLUMNS]){
	int j = 0; 
	int i = NUM_RINGS;
	for (; j < NUM_RINGS; j ++, i--){
		int delta = (col_width - RING_HEIGHT) / NUM_RINGS;
		board[0][j][0] = col_positions[0];
		board[0][j][1] = gfx_screenHeight()-FLOOR_HEIGHT-RING_HEIGHT*(j+1);
		board[0][j][2] = col_width-delta*(NUM_RINGS-i);
	}
}

void draw_rings(int board[][NUM_RINGS][3]){
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

int is_move_valid(int board[][NUM_RINGS][3], int from, int to){
	int ring_from = top(board[from]);
	int ring_to = top(board[to]);
	int ring_from_width = board[from][ring_from][2];
	int ring_to_width = board[to][ring_to][2];
	if (ring_to_width == 0) return 1;
	if (ring_from_width < ring_to_width) return 1;

	return 0;
}

int count_rings(int board[][NUM_RINGS][3], int column){
	int i;
	for (i = 0; i < NUM_RINGS; i++){
		if (board[column][i][2] == 0) return i;
	}
	return i;
}

int move_object(int from, int to, int col_positions[NUM_COLUMNS], int board[][NUM_RINGS][3], int column_height){
	if (!is_move_valid(board, from, to)) return 0;
	
	int ring = top(board[from]);
	int ring_to = top(board[to]);
	int direction;
	
	
	if (board[from][ring][1] < gfx_screenHeight() - FLOOR_HEIGHT - column_height && col_positions[to] != board[from][ring][0]){
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
		int bottom_barrier;
		if (board[to][ring_to][1] == 0) bottom_barrier = gfx_screenHeight() - FLOOR_HEIGHT - RING_HEIGHT;
		else bottom_barrier = board[to][ring_to][1] - RING_HEIGHT;
		if (board[from][ring][1] > bottom_barrier){
			board[to][ring_to+1][0] = board[from][ring][0];
			board[to][ring_to+1][1] = bottom_barrier;
			board[to][ring_to+1][2] = board[from][ring][2];
			board[from][ring][0] = 0;
			board[from][ring][1] = 0;
			board[from][ring][2] = 0;			
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
	int column_height = RING_HEIGHT*(NUM_RINGS + 2);
	int columns_xs[NUM_COLUMNS];
	int board[NUM_COLUMNS][NUM_RINGS][3] = {0};
	int is_moving = 0;
	int from = -1;
	int to = -1;
	
	set_columns_xs(columns_xs, space_between_columns);
	initialize_board(board, column_width, columns_xs);
	
	for (int i = 0; i < NUM_COLUMNS; i++){
		for (int j = 0; j < NUM_RINGS; j ++ ){
			for (int k = 0; k < 3; k ++){
				printf("%d ", board[i][j][k]);
			}
			printf("\n");
		}
	}
	
	printf("%d\n", top(board[0]));
	printf("%d", count_rings(board, 1));
	
	while(playing){
		playing = handle_click(gfx_pollkey());
		gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1,
					   BLACK);
		
		if (is_moving){
			is_moving = move_object(from, to, columns_xs, board, column_height);
			if (is_moving == 0){
				from = -1;
				to = -1;
			}
		}
		else {
			if (from == -1) from = get_column(gfx_pollkey());
			else if (to == -1){
				to = get_column(gfx_pollkey());
				if (to != -1) is_moving = 1;
			}				
		}
		
		announce_winner(board);
				
	
		draw_floor();

		draw_columns(columns_xs, column_width, column_height);
		draw_rings(board);
		
		//move_object(0, 1, columns_xs, board, column_height);

		gfx_updateScreen();
		SDL_Delay(10);
	}
	
	

	return 0;
}
