// Connect4_MinMax.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdio.h"

#define false 0
#define true 1
#define N_ROWS 8
#define N_COLUMNS 8
#define DEPTH 60

int table[N_ROWS][N_COLUMNS];

// Either 1 or 2
int current_player = 1;
// Each player's score
int score1 = 0;
int score2 = 0;
int i, j;

// current column picked by current player
int current_move;
// current row picked by current player
int current_row;

void clear_table(){
	for (i = 0; i < N_ROWS; i++)
		for (j = 0; j < N_COLUMNS; j++)
			table[i][j] = 0;
}

/*
Prints table and score
*/
void print_table() {

	printf("~* CONNECT 4 *~ \n \n");

	// Print table
	for (i = 0; i < N_ROWS; i++) {
		printf("|");
		for (j = 0; j < N_COLUMNS; j++) {
			if (table[i][j] == 0)
				printf(" . ");
			if (table[i][j] == 1)
				printf(" X ");
			if (table[i][j] == 2)
				printf(" 0 ");
			printf("|");
		}
		printf("\n");
	}

	// Print numbers
	printf("\n+ ");
	for (j = 0; j < N_COLUMNS; j++)
		printf("%d   ", j);
	printf("+ \n \n");

	// Score
	printf("SCORE: \n Player 1 (X) = %d \n Player 2 (0) = %d \n \n", score1, score2);
}


int column_is_full(int column_j) {
	return (table[0][column_j] != 0);
}

int table_is_full() {
	for (int j = 0; j < N_COLUMNS; j++){
		//If some column is not full, then table is not full
		if (table[0][j] == 0)
			return false;
	}
	return true;
}

int getNextMove();
/*
Ask player which column to pick and change table accordingly
*/
void pick_column() {
	current_move = -1;
	if (current_player == 1)
	{
		current_move = getNextMove();
		printf("player 1 move: %d", current_move);
	}
	else
	{
		printf("Pick a column, then press enter, player %d:\n", current_player);
		scanf_s("%d", &current_move);

		// while move is invalid, keep asking
		while (current_move < 0 || current_move > N_COLUMNS - 1 || column_is_full(current_move)){
			printf("invalid move, pick another column:\n");
			scanf_s("%d", &current_move);
		}
	}

	// Find row where his move will be performed
	// It will be the first row with value equals 0
	int row = N_ROWS - 1;
	while (table[row][current_move] != 0)
		row--;

	// Change table accordingly
	table[row][current_move] = current_player;

	// Store row where player just placed his marker
	// Used to check if current player won the game
	current_row = row;
}

/*
Switch current player
*/
void switch_player(){
	if (current_player == 1)
		current_player = 2;
	else
		current_player = 1;
}

/*
Checks if player won by making a sequence of 4 markers either
horizontally, vertically or diagonally.
*/
int current_player_won(){
	// Check for vertical sequence
	// Look at last marker placed and compare with the 3 markers below it
	if ((current_row < N_ROWS - 3)
		&& (table[current_row][current_move] == table[current_row + 1][current_move])
		&& (table[current_row + 1][current_move] == table[current_row + 2][current_move])
		&& (table[current_row + 2][current_move] == table[current_row + 3][current_move]))
		return true;

	// Check for horizontal sequence
	int sequence_length = 1;
	int j = 1;
	while ((current_move - j >= 0) && (table[current_row][current_move - j] == current_player)){
		j++; sequence_length++;
	}
	j = 1;
	while ((current_move + j < N_COLUMNS) && (table[current_row][current_move + j] == current_player)){
		j++; sequence_length++;
	}
	if (sequence_length >= 4)
		return true;

	//Check for diagonal sequence
	sequence_length = 1;
	j = 1;
	while ((current_move - j >= 0) && (current_row - j >= 0) && (table[current_row - j][current_move - j] == current_player)){
		j++; sequence_length++;
	}
	j = 1;
	while ((current_move + j < N_COLUMNS) && (current_row + j <= 5) && (table[current_row + j][current_move + j] == current_player)){
		j++; sequence_length++;
	}
	if (sequence_length >= 4)
		return true;

	//Check for inverted diagonal sequence
	sequence_length = 1;
	j = 1;
	while ((current_move - j >= 0) && (current_row + j < N_ROWS) && (table[current_row + j][current_move - j] == current_player)){
		j++; sequence_length++;
	}
	j = 1;
	while ((current_move + j < N_COLUMNS) && (current_row - j >= 0) && (table[current_row - j][current_move + j] == current_player)){
		j++; sequence_length++;
	}
	if (sequence_length >= 4)
		return true;

	return false;
}

/*
Increase current player's score
*/
void update_score(){
	if (current_player == 1)
		score1++;
	else
		score2++;
}

/*
Structures for maintaining the state of the recursion.
*/
typedef struct board_state
{
	int board[N_ROWS][N_COLUMNS]; //board state
	int next_moves; //points to the location on stack holding the
					//next possible moves from this state.
};

typedef struct children
{
	board_state p[7];
	int q; //the current state being explored
	int max; //the total number of valid states generated. 
}children;

typedef struct state
{
	int alpha, beta, m, t;
};

children STACK[1024];
state STATE[1024];

#define L 1024
#define INIFINITY 9999999999
int count = 1;

int a[L + 2], r[L + 1];

void copy_board(int copy[N_ROWS][N_COLUMNS], int board[N_ROWS][N_COLUMNS])
{
	for (int i = 0; i < N_ROWS; i++)
		for (int j = 0; j < N_COLUMNS; j++)
			copy[i][j] = board[i][j];
}

/*Generate the possible moves
from the current position*/
void generate(int l, int player)
{
	STACK[r[l]].p[STACK[r[l]].q].next_moves = count;
	STACK[count].max = -1;
	STACK[count].q = 0;

	int flag = 0;

	for (int i = 0; i < N_COLUMNS; i++)
	{
		for (int j = N_ROWS - 1; j >= 0 && STACK[r[l]].p[STACK[r[l]].q].board[j][i] == 0; j++);
		if (j == N_ROWS-1)
		{
			continue;
		}
		flag++;
		copy_board(STACK[count].p[++(STACK[count].max)].board, STACK[r[l]].p[STACK[r[l]].q].board);
		STACK[count].p[STACK[count].max].board[j + 1][i] = player;
		STACK[count].max++;
	}

	if (flag == 0)
		STACK[r[l]].p[STACK[r[l]].q].next_moves = -1;
	count++;
}

/*Get the first move*/
int first(int l)
{
	return STACK[r[l]].p[STACK[r[l]].q].next_moves;
}

/*Get the next move*/
int next(int l)
{
	int val = STACK[r[l]].q + 1;
	STACK[r[l]].q++;
	if (val > STACK[r[l]].max)
		return -1;
	return val;
}

/*
Get the heuristic value for the given state.
*/

int getHeuristic(int board[N_ROWS][N_COLUMNS])
{
	int player1 = 0, player2 = 0;
	// Check for vertical sequence
	// Look at last marker placed and compare with the 3 markers below it
	int current_row, current_move, current_player;

	for (int j = 0; j < N_COLUMNS; j++)
	{
		current_move = j;
		int score = 1;

		for (current_row = 0; current_row < N_ROWS && board[current_row][j] != 0; current_row++);

		if (board[current_row][current_move] == 1)
			current_player = 1;
		else
			current_player = 2;

		for (int i = 0; i < 3; i++)
		{
			if (board[current_row - i][current_move] == board[current_row - i - 1][current_move])
				score++;
			else
				break;
		}

		if (i == 3)
		{
			if (current_player = 1)
				return 5;
			else
				return -5;
		}

		// Check for horizontal sequence
		int sequence_length = 1;
		int j = 1;
		while ((current_move - j >= 0) && (board[current_row][current_move - j] == current_player)){
			j++; sequence_length++;
		}
		j = 1;
		while ((current_move + j < N_COLUMNS) && (board[current_row][current_move + j] == current_player)){
			j++; sequence_length++;
		}
		if (sequence_length >= 4)
		{
			if (current_player = 1)
				return 5;
			else
				return -5;
		}

		if (sequence_length > score)
			score = sequence_length;


		//Check for diagonal sequence
		sequence_length = 1;
		j = 1;
		while ((current_move - j >= 0) && (current_row - j >= 0) && (board[current_row - j][current_move - j] == current_player)){
			j++; sequence_length++;
		}
		j = 1;
		while ((current_move + j < N_COLUMNS) && (current_row + j <= 5) && (board[current_row + j][current_move + j] == current_player)){
			j++; sequence_length++;
		}
		if (sequence_length >= 4)
		{
			if (current_player = 1)
				return 5;
			else
				return -5;
		}

		if (sequence_length > score)
			score = sequence_length;

		//Check for inverted diagonal sequence
		sequence_length = 1;
		j = 1;
		while ((current_move - j >= 0) && (current_row + j < N_ROWS) && (board[current_row + j][current_move - j] == current_player)){
			j++; sequence_length++;
		}
		j = 1;
		while ((current_move + j < N_COLUMNS) && (current_row - j >= 0) && (board[current_row - j][current_move + j] == current_player)){
			j++; sequence_length++;
		}
		if (sequence_length >= 4)
		{
			if (current_player = 1)
				return 5;
			else
				return -5;
		}

		if (sequence_length > score)
			score = sequence_length;

		if (current_player = 1)
		{
			if (player1 > score)
				player1 = score;
		}
		else
			if (player2 > score)
				player2 = score;
	}

	return player1 - player2;
}

/*
alpha beta pruning algorithm as per the
paper :
An Analysis of Alpha-Beta Priming,
by Donald E. Knuth and Ronald W. Moore
*/
int alphabeta(int board[N_ROWS][N_COLUMNS])
{
	int l = 0;
	a[0] = a[1] = -INIFINITY;
	r[0] = 0;
	STACK[r[0]].max = 0;
	copy_board(STACK[r[0]].p[0].board, board);
	int player = 2;

F2: generate(l, player);
	r[l + 1] = first(l);
	if (r[l + 1] == -1 || l > DEPTH)
		a[l + 2] = getHeuristic(STACK[r[l]].p[STACK[r[l]].q].board);
	else
	{
		a[l + 2] = a[l];
	loop: l = l + 1;
		player = (player - 1) % 2 + 1;
		goto F2;
	resume: if (-a[l + 3] > a[l + 2])
	{
		a[l + 2] = a[l + 3];
		if (a[l + 3] <= a[l + 1])
			goto done;

	}
			r[l + 1] = next(l + 1);
			if (r[l + 1] != -1)
				goto loop;
	}
done: l = l - 1;
	player = (player - 1) % 2 + 1;
	if (l >= 0)
		goto resume;

	return a[2];
}

/*
Find the best move for the computer using iterative minimax.
with alpha beta pruning as well.
The heuristic used: Computer connected dots - player connected.
					If computer wins: 5
					If player wins: -5
*/
int getNextMove()
{
	int copy[N_ROWS][N_COLUMNS];

	int values[N_COLUMNS];

	for (int j = 0; j < N_COLUMNS; j++)
	{
		copy_board(copy, table);
		for (int i = N_ROWS - 1; i >= 0 && copy[i][j] == 0; i--);
		if (i == N_ROWS - 1)
		{
			values[j] = -1;
			continue;
		}

		copy[i + 1][j] = 1;
		count = 1;
		values[j] = alphabeta(copy);
	}

	int max=values[0], move = 0;
	printf("VAlues: \n");
	for (int i = 1; i < N_COLUMNS; i++)
	{
		printf("%d\n",values[i]);
		if (values[i]>max)
		{
			max = values[i];
			move = i;
		}
	}

	return move;
}

int _tmain(int argc, _TCHAR* argv[])
{
	// game loop
	while (true) {
		clear_table();
		print_table();

		// match loop
		while (true) {
			pick_column();
			print_table();

			if (current_player_won()){
				update_score();
				print_table();
				printf("Congratulations, Player %d! \n", current_player);
				// leave match
				break;
			}
			// If nobody wins and table is full, we come to a draw
			else if (table_is_full()){
				printf("Draw! \n");
				// leave match
				break;
			}

			switch_player();
		}

		printf("Do you wish to play again?(y/n) \n");
		char play_again;
		scanf_s(" %c", &play_again);
		if (!(play_again == 'y' || play_again == 'Y'))
			break;
	}
	
	return 0;
}