#include "primlib.h"
#include <stdlib.h>
#include <unistd.h>

#define NUM_COLUMNS 3
#define NUM_RINGS 5
#define RING_HEIGHT 10
#define HORIZONAL_MARGIN 50
#define FLOOR_HEIGHT 50


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

void initialize_board(int board[][NUM_RINGS]){
	int r = NUM_RINGS;
	int i = 0;
	
	for (; r > 0; r--, i++){
		board[0][i] = r;
	}
}

void draw_rings(int board[][NUM_RINGS], int col_positions[NUM_COLUMNS], int col_width){
	for (int i = 0; i < NUM_COLUMNS; i++){
		for (int j = 0; j < NUM_RINGS; j ++ ){
			if (board[i][j] == 0) continue;
			int delta = (col_width - RING_HEIGHT) / NUM_RINGS;
			draw_obj(col_positions[i], gfx_screenHeight()-FLOOR_HEIGHT-RING_HEIGHT*(j+1), col_width-delta*(5-board[i][j]), RING_HEIGHT, RED);
		}
	}
}

void draw_floor(){
	gfx_filledRect(0, gfx_screenHeight()-FLOOR_HEIGHT, gfx_screenWidth(), gfx_screenHeight(), BLUE);
}

void move_object(int from, int to, int obj_width, int ){
	int delta = (col_width - RING_HEIGHT) / NUM_RINGS;
}

int handle_click(int code){
	//static int from;
	return 1;
}


int main(int argc, char* argv[])
{
	if (gfx_init()) {
		exit(3);
	}
	
	int displaying = 1;
	int space_between_columns = (gfx_screenWidth() - 2*HORIZONAL_MARGIN)/NUM_COLUMNS;
	int column_width = space_between_columns/2;
	int column_height = RING_HEIGHT*(NUM_RINGS + 2);
	int columns_xs[NUM_COLUMNS];
	int board[NUM_COLUMNS][NUM_RINGS] = {0};
	int from = -1;
	
	initialize_board(board);
	set_columns_xs(columns_xs, space_between_columns);
	
	
	while(displaying){
		displaying = handle_click(gfx_pollkey());
				
		gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1,
					   BLACK);
		draw_floor();

		draw_columns(columns_xs, column_width, column_height);
		draw_rings(board, columns_xs, column_width);
		

		gfx_updateScreen();
		SDL_Delay(10);
	}

	return 0;
}
