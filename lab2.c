#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "lab2.h"

//Included in lab2.h
int** read_board_from_file(char* filename){

    FILE *fp = NULL;
    int** sudoku_board = (int**)malloc(sizeof(int*)*ROW_SIZE);
    for(int row = 0; row < ROW_SIZE; row++){
        sudoku_board[row] = (int*)malloc(sizeof(int)*COL_SIZE);
    }

    fp = fopen(filename,"r");
    for(int i = 0; i < ROW_SIZE; i++) {
        for(int j = 0; j < COL_SIZE; j++) {
            fscanf(fp, "%d%*c", &sudoku_board[i][j]);
        }
    }

    fclose(fp);
    return sudoku_board;
}
//Not included in lab2.h
void *check_valid_line(void* parameters)
{
    param_struct *p = (param_struct*) parameters;
    int validation_array[ROW_SIZE] = {0};
    int is_row = p -> is_row;
    int is_col = p -> is_column;

    if (is_row && !is_col) {
        int row = p -> starting_row;
        for(int i = 0; i< ROW_SIZE; i++)
        {
            int target = sudoku_board[row][i];
            if (target > ROW_SIZE || target < 1 || validation_array[target - 1]==1)
            {
                pthread_exit(NULL);
            }
            else{
                validation_array[target - 1] = 1;
            }
        }
        worker_validation[ROW_SIZE+row] = 1;
        pthread_exit(NULL);

    } else if (is_col && !is_row) {
        int col = p -> starting_col;
        for(int i = 0; i< ROW_SIZE; i++)
        {
            int target = sudoku_board[i][col];
            if (target > ROW_SIZE || target < 1 || validation_array[target - 1]==1)
            {
                pthread_exit(NULL);
            }
            else{
                validation_array[target - 1] = 1;
            }
        }
        worker_validation[ROW_SIZE*2+col] = 1;
        pthread_exit(NULL);
    } else {
        return 0;
    }
}
// Not included in lab2.h
void *check_valid_box(void* parameters)
{
    param_struct *p = (param_struct*) parameters;
    int validation_array[ROW_SIZE] = {0};
    int row = p -> starting_row;
    int col = p -> starting_col;

    for(int i = row; i < row + 3; i++){
        for(int j = col; j<col + 3; j++)
        {
            int target = sudoku_board[i][j];
            if (target > ROW_SIZE || target < 1 || validation_array[target - 1]==1)
            {
                pthread_exit(NULL);
            }
            else{
                validation_array[target-1] = 1;
            }
        }
    }
    worker_validation[row+col/3]=1;
    pthread_exit(NULL);

}
// Included in lab2.h
int is_board_valid() {
    pthread_t *tid;
    int threadIndex = 0;
    worker_validation = (int *) malloc(sizeof(int) * NUM_OF_THREADS);
    tid = (pthread_t *) malloc(sizeof(int) * NUM_OF_THREADS);

    for (int i = 0; i < ROW_SIZE; i++) {
        for (int j = 0; j < COL_SIZE; j++) {
            if (i % 3 == 0 && j % 3 == 0) {
                param_struct *worker_box = (param_struct *) malloc(sizeof(param_struct));
                worker_box->starting_row = i;
                worker_box->starting_col = j;
                pthread_create(&tid[threadIndex++], NULL, check_valid_box, worker_box);
            }
            if (j == 0) {
                param_struct *worker_row = (param_struct *) malloc(sizeof(param_struct));
                worker_row->starting_row = i;
                worker_row->is_row = 1;
                pthread_create(&tid[threadIndex++], NULL, check_valid_line, worker_row);
            }
            if (i == 0) {
                param_struct *worker_col = (param_struct *) malloc(sizeof(param_struct));
                worker_col->starting_col = j;
                worker_col->is_column = 1;
                pthread_create(&tid[threadIndex++], NULL, check_valid_line, worker_col);
            }

        }
    }

    for (int i = 0; i < NUM_OF_THREADS; i++) {
        pthread_join(tid[i], NULL);
        if (worker_validation[i] == 0) {
            return 0;
        }
    }

    free(worker_validation);
    free(tid);
    return 1;
}