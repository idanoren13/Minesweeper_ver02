//Minesweeper_ver02.c
//
//Final game, this time we have a recursive function that reveals all the neighbors cells that are empty.
//To reveal a chosen cell the user enters row and column and the key 'O'
//the user also have the ability to flag suspicious cells with pressing the 'F' key after the row and column input.
//
//Idan Oren 22/12/2020
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define MINE '*'
#define SPACE ' '
#define ROWS 22
#define COLS 22


void gameFunc();
void menuFunc(int game_size[3]);
void mapPrinter(char map[ROWS][COLS], int rows, int cols);
void gameMapGenerator(char game_map[ROWS][COLS], int rows, int cols);
void randomMapGenerator(char consealed_map[ROWS][COLS], int rows, int cols, int mine_quantity);
void minesInRange(char consealed_map[ROWS][COLS], int rows, int cols);
bool mineChecker(char consealed_square);
void lastCellLocation(char consealed_map[ROWS][COLS], char game_map[ROWS][COLS], int last_cell[2], int row_size, int col_size);
void mapRevealer(char consealed_map[ROWS][COLS], char game_map[ROWS][COLS], int counter, int last_cell[2], int last_row, int last_col, int row_size, int col_size);
int neighborEmptyRevealer(char consealed_map[ROWS][COLS], char game_map[ROWS][COLS], int row_input, int col_input, int row_size, int col_size);
bool spaceChecker(char square);
void printHeadLine();
void printGameOver();
void printWinner();

void main() {
	
	gameFunc();
}

void gameFunc() {
	//input[0] -> rows
	//input[1] -> cols
	int user_input[2];
	char flag;
	

	//game_size[0] -> rows
	//game_size[1] -> cols
	//game_size[3] -> Mines quantity
	int game_size[3] = { 8,8,8 };
	int turns_to_win;

	//last_cell[0] = last row
	//last_cell[1] = las column
	int last_cell[2] = { 0,0 };
	char consealed_map[ROWS][COLS];
	char game_map[ROWS][COLS];

	menuFunc(game_size);
	//if game_size initionlized as -1, the game closes.
	if (game_size[2] == -1) {
		printf("GoodBye!!!");
		return;
	}

	//generates the maps for the game
	turns_to_win = (game_size[0] * game_size[1]) - game_size[2];
	gameMapGenerator(game_map, game_size[0], game_size[1]);
	randomMapGenerator(consealed_map, game_size[0], game_size[1], game_size[2]);

	//Game loop
	while (turns_to_win > 0) {
		mapPrinter(game_map, game_size[0], game_size[1]);
		printf("\n\nTurns To Win:%d\n\n", turns_to_win);
		printf("Please enter a row number and a column number : ");
		scanf(" %d%d", &user_input[0], &user_input[1]);
		if (user_input[0] != -1)
			scanf(" %c", &flag);

		if (user_input[0] == -1 && user_input[1] > 0) { //reveals a portion of the map
			turns_to_win -= user_input[1];
			lastCellLocation(consealed_map, game_map, last_cell, game_size[0], game_size[1]);
			mapRevealer(consealed_map, game_map, user_input[1], last_cell, last_cell[0], last_cell[1], game_size[0], game_size[1]);
		}
		else if (user_input[0] < 0 || user_input[1] < 0 || user_input[0] >= game_size[0] || user_input[1] >= game_size[1]) {
			printf("invalid input! number are out of boundries\n");
		}
		else if (game_map[user_input[0]][user_input[1]] == consealed_map[user_input[0]][user_input[1]]) {
			printf("invalid input! You already entered that numbers\n");
		}
		// if F was pressed it marks the selected cell with F
		else if (((flag == 'f') || (flag == 'F'))) {
			game_map[user_input[0]][user_input[1]] = 'F';
		}
		else if (flag == 'o' || flag == 'O') {
			if (game_map[user_input[0]][user_input[1]] == 'F') {
				game_map[user_input[0]][user_input[1]] = 'X';
			}
			 if (mineChecker(consealed_map[user_input[0]][user_input[1]])) {//check if the selected sqaure is a mine
				mapPrinter(consealed_map, game_size[0], game_size[1]);
				printGameOver();
				return;
			}
			else if (spaceChecker(consealed_map[user_input[0]][user_input[1]])) {
				turns_to_win -= neighborEmptyRevealer(consealed_map, game_map, user_input[0], user_input[1], game_size[0], game_size[1]);
			}

			else {
				game_map[user_input[0]][user_input[1]] = consealed_map[user_input[0]][user_input[1]];//reveals the selected cell
				--turns_to_win;
			}
		
		 }
		else
			printf("invalid action!\n");

	}
	mapPrinter(game_map, game_size[0], game_size[1]);
	printWinner();
}

//Handles the menu at the start
void menuFunc(int game_size[3]) {
	int user_input = -1;
	int row_input = 8, column_input = 8;

	printHeadLine(); //Prints a headline "Minesweeper"
	printf("Please choose one of the following options and enter it's number:\n\n");
	printf("1 - for size 8x8\n\n2 - for size 12x12\n\n3 - for size 15x15\n\n4 - for custom size\n\n0 - Exit\n\n");

	while (user_input < 0 || user_input > 4) {
		scanf("%d", &user_input);

		switch (user_input)
		{
		case 0://Exit the game
			game_size[2] = -1;
			break;
		case 1:
			game_size[0] = 8;
			game_size[1] = 8;
			game_size[2] = 8;
			break;
		case 2:
			game_size[0] = 12;
			game_size[1] = 12;
			game_size[2] = 12;
			break;
		case 3:
			game_size[0] = 15;
			game_size[1] = 15;
			game_size[2] = 15;
			break;
		case 4:
			printf("Custom size. Please enter rows size up to 22 and then columns size up to 22 :");
			scanf("%d%d", &row_input, &column_input);

			//loop for legal input
			while ((row_input < 1) || (column_input < 1) || (row_input > 22) || (column_input > 22)) {
				printf("The size is out of boundries! Please Enter again :");
				scanf("%d%d", &row_input, &column_input);
			}

			game_size[0] = row_input;
			game_size[1] = column_input;
			game_size[2] = (int)sqrt(row_input * column_input);

			break;
		default:
			printf("Invalid input please enter again: ");
		}
	}
}

//prints the map
void mapPrinter(char map[ROWS][COLS], int rows, int cols) {
	int col_index = 0, row_index = 0;
	printf("\n");
	printf("       ");
	for (col_index = 0; col_index < cols; col_index++) {
		printf("%6d", col_index);
	}
	printf("\n\n");
	for (row_index = 0; row_index < rows; row_index++) {
		for (col_index = 0; col_index < cols; col_index++) {
			if (col_index == 0 && row_index < 10)
				printf("      %d ", row_index);
			if (col_index == 0 && row_index >= 10)
				printf("     %d ", row_index);
			printf(" |%3c ", map[row_index][col_index]);
		}
		printf(" |\n");
	}
}

//genrate the game map - puts 'X' in every cell
void gameMapGenerator(char game_map[ROWS][COLS], int rows, int cols) {
	for (int index_row = 0; index_row < rows; index_row++)
		for (int index_col = 0; index_col < cols; index_col++)
			game_map[index_row][index_col] = 'X';

}

//generates a random consealed map
void randomMapGenerator(char consealed_map[ROWS][COLS], int rows, int cols, int mine_quantity) {

	char temp = mine_quantity; //a temp character, uses in both initializing the consealed map and to random the map
	int random_row, random_col = 0;
	int row_index, col_index;

	srand(time(NULL));

	//Initializing the map, places first the mines and then fills the other cells with '0'
	for (row_index = 0; row_index < rows; row_index++)
		for (col_index = 0; col_index < cols; col_index++) {
			if (temp) {
				consealed_map[row_index][col_index] = MINE;
				--temp;
			}
			else
				consealed_map[row_index][col_index] = '0';
		}
	//Random the map
	for (row_index = 0; row_index < rows; row_index++)
		for (col_index = 0; col_index < cols; col_index++) {
			random_row = rand() % (rows - row_index);
			random_col = rand() % (cols - col_index);
			temp = consealed_map[random_row][random_col];
			consealed_map[random_row][random_col] = consealed_map[row_index][col_index];
			consealed_map[row_index][col_index] = temp;
		}
	minesInRange(consealed_map, rows, cols);
}

//Determine the number of neighboring mines to a square
void minesInRange(char consealed_map[ROWS][COLS], int rows, int cols) {
	for (int index_cols = 0; index_cols < cols; index_cols++) {
		for (int index_rows = 0; index_rows < rows; index_rows++) {
			if (!mineChecker(consealed_map[index_rows][index_cols])) {
				//NW
				if ((index_rows - 1 >= 0) && (index_cols - 1 >= 0) && mineChecker(consealed_map[index_rows - 1][index_cols - 1]))
					consealed_map[index_rows][index_cols]++;
				//N	
				if ((index_rows - 1 >= 0) && mineChecker(consealed_map[index_rows - 1][index_cols]))
					consealed_map[index_rows][index_cols]++;
				//NE
				if ((index_rows - 1 >= 0) && (index_cols + 1 < cols) && mineChecker(consealed_map[index_rows - 1][index_cols + 1]))
					consealed_map[index_rows][index_cols]++;
				//W
				if ((index_cols - 1 >= 0) && mineChecker(consealed_map[index_rows][index_cols - 1]))
					consealed_map[index_rows][index_cols]++;
				//E
				if ((index_cols + 1 < cols) && mineChecker(consealed_map[index_rows][index_cols + 1]))
					consealed_map[index_rows][index_cols]++;
				//SW
				if ((index_rows + 1 < rows) && (index_cols - 1 >= 0) && mineChecker(consealed_map[index_rows + 1][index_cols - 1]))
					consealed_map[index_rows][index_cols]++;
				//S
				if ((index_rows + 1 < rows) && mineChecker(consealed_map[index_rows + 1][index_cols]))
					consealed_map[index_rows][index_cols]++;
				//SE
				if ((index_rows + 1 < rows) && (index_cols + 1 < cols) && mineChecker(consealed_map[index_rows + 1][index_cols + 1]))
					consealed_map[index_rows][index_cols]++;

				//converts '0' to ' '
				if (consealed_map[index_rows][index_cols] == '0')
					consealed_map[index_rows][index_cols] = SPACE;
			}
		}
	}
}

bool mineChecker(char consealed_square) {
	return (consealed_square == MINE);
}

//Calculates the last cell location and writes it to the 
void lastCellLocation(char consealed_map[ROWS][COLS], char game_map[ROWS][COLS], int last_cell[2], int row_size, int col_size) {
	int row_cntr = last_cell[0], col_cntr = last_cell[1];
	while (game_map[row_cntr][col_cntr] == consealed_map[row_cntr][col_cntr] && row_cntr < row_size) {
		col_cntr += 1;
		if (col_cntr == col_size) {
			col_cntr = 0;
			row_cntr += 1;
		}
	}
	last_cell[1] = col_cntr;
	last_cell[0] = row_cntr;
}

//Reveals the the chosen number of squares that does not contain a mine by order
void mapRevealer(char consealed_map[ROWS][COLS], char game_map[ROWS][COLS], int counter, int last_cell[2], int last_row, int last_col, int row_size, int col_size) {
	int row_cntr = last_row, col_cntr = last_col;
	while (row_cntr < col_size && counter>0) {
		for (col_cntr; col_cntr < row_size && counter>0; col_cntr++) {
			if (!mineChecker(consealed_map[row_cntr][col_cntr]) && !(game_map[row_cntr][col_cntr] == consealed_map[row_cntr][col_cntr])) {
				game_map[row_cntr][col_cntr] = consealed_map[row_cntr][col_cntr];//updates the game map
				counter -= 1;
			}
		}
		if (col_cntr == col_size) {
			col_cntr = 0;
			row_cntr += 1;
		}
	}
	last_cell[1] = col_cntr;
	last_cell[0] = row_cntr;

}

//Recursive function that prints all the empty spaces
//Returns the number of cells that was revealed
int neighborEmptyRevealer(char consealed_map[ROWS][COLS], char game_map[ROWS][COLS], int row_input, int col_input, int row_size, int col_size) {
	int output = 0;
	if (!(row_input >= 0 && col_input >= 0 && row_input < row_size && col_input < col_size)) //checks if the call is valid
		return 0;
	if (game_map[row_input][col_input] == consealed_map[row_input][col_input])	//checks if the called cell is alreay revealed
		return 0;
	if (game_map[row_input][col_input] == 'F')	//checks if the called cell is falgged
		return 0;
	else if (!spaceChecker(consealed_map[row_input][col_input]) && game_map[row_input][col_input] != consealed_map[row_input][col_input]) {	//checks if the called cell is a consealed native number
		game_map[row_input][col_input] = consealed_map[row_input][col_input];
		return 1;
	}
	else {
		game_map[row_input][col_input] = consealed_map[row_input][col_input];
		output++;

		output += neighborEmptyRevealer(consealed_map, game_map, row_input - 1, col_input, row_size, col_size);//N

		output += neighborEmptyRevealer(consealed_map, game_map, row_input - 1, col_input - 1, row_size, col_size);//NW

		output += neighborEmptyRevealer(consealed_map, game_map, row_input - 1, col_input + 1, row_size, col_size);//NE

		output += neighborEmptyRevealer(consealed_map, game_map, row_input, col_input - 1, row_size, col_size);//W

		output += neighborEmptyRevealer(consealed_map, game_map, row_input, col_input + 1, row_size, col_size);//E

		output += neighborEmptyRevealer(consealed_map, game_map, row_input + 1, col_input, row_size, col_size);//S

		output += neighborEmptyRevealer(consealed_map, game_map, row_input + 1, col_input - 1, row_size, col_size);//SW

		output += neighborEmptyRevealer(consealed_map, game_map, row_input + 1, col_input + 1, row_size, col_size);//SE

		return output;
	}
}

bool spaceChecker(char square) {
	return(square == SPACE);
}

void printHeadLine() {
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("  __  __  _                                                           \n");
	printf(" |  \\/  |(_)                                                          \n");
	printf(" | \\  / | _  _ __    ___  ___ __      __ ___   ___  _ __    ___  _ __ \n");
	printf(" | |\\/| || || '_ \\  / _ \\/ __|\\ \\ /\\ / // _ \\ / _ \\| '_ \\  / _ \\| '__|\n");
	printf(" | |  | || || | | ||  __/\\__ \\ \\ V  V /|  __/|  __/| |_) ||  __/| |   \n");
	printf(" |_|  |_||_||_| |_| \\___||___/  \\_/\\_/  \\___| \\___|| .__/  \\___||_|   \n");
	printf("                                                   | |                \n");
	printf("                                                   |_|                \n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
}

//Prints a headline "Game over"
void printGameOver() {
	printf("   _____                                                  \n");
	printf("  / ____|                                                 \n");
	printf(" | |  __   __ _  _ __ ___    ___     ___ __   __ ___  _ __ \n");
	printf(" | | |_ | / _` || '_ ` _ \\  / _ \\   / _ \\\\ \\ / // _ \\| '__|\n");
	printf(" | |__| || (_| || | | | | ||  __/  | (_) |\\ V /|  __/| |   \n");
	printf("  \\_____| \\__,_||_| |_| |_| \\___|   \\___/  \\_/  \\___||_|   \n");
}

void printWinner() {
	printf(" __     __ ____   _    _    __          __ _____  _   _    _   _   _ \n");
	printf(" \\ \\   / // __ \\ | |  | |   \\ \\        / /|_   _|| \\ | |  | | | | | |\n");
	printf("  \\ \\_/ /| |  | || |  | |    \\ \\  /\\  / /   | |  |  \\| |  | | | | | |\n");
	printf("   \\   / | |  | || |  | |     \\ \\/  \\/ /    | |  | . ` |  | | | | | |\n");
	printf("    | |  | |__| || |__| |      \\  /\\  /    _| |_ | |\\  |  |_| |_| |_|\n");
	printf("    |_|   \\____/  \\____/        \\/  \\/    |_____||_| \\_|  (_) (_) (_)\n\n");
}

