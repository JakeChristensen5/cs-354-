#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char *DELIM = ","; // commas ',' are a common delimiter character for data strings
/*
 * Read the first line of input file to get the size of that board.
 *
 * PRE-CONDITION #1: file exists
 * PRE-CONDITION #2: first line of file contains valid non-zero integer value
 *
 * fptr: file pointer for the board's input file
 * size: a pointer to an int to store the size
 */
void get_board_size(FILE *fptr, int *size)
{
    char *line1 = NULL;
    size_t len = 0;

    if (getline(&line1, &len, fptr) == -1)
    {
        printf("Error reading the input file.\n");
        exit(1);
    }

    char *size_chars = NULL;
    size_chars = strtok(line1, DELIM); // just removes certain things (ie whitespace/unneeded chars)
    *size = atoi(size_chars);          // string => int

    // free memory allocated for reading first link of file
    free(line1);
    line1 = NULL;
}

/*
 * Returns 1 if and only if the board is in a valid Sudoku board state.
 * Otherwise returns 0.
 *
 * A valid row or column contains only blanks or the digits 1-size,
 * with no duplicate digits, where size is the value 1 to 9.
 *
 * Note: p2A requires only that each row and each column are valid.
 *
 * board: heap allocated 2D array of integers
 * size:  number of rows and columns in the board
 */
int valid_board(int **board, int size)
{

    // checks size: 1 to 9 (1 - n)
    if (size > 9 || size < 1)
    {
        return 0; // return false
    }

    for (int row = 0; row < size; row++)
    { // each row
        for (int col = 0; col < size; col++)
        { // each col

            /**
            ------------------------------
            checking for duplicate vals in rows
            */
            for (int k = col + 1; k < size; k++)
            {

                // board[i][j] == board[i][k]
                if (*(*(board + row) + col) == *(*(board + row) + k) && *(*(board + row) + col) != 0)
                {
                    return 0; // return false
                }
            }

            /**
            Checking for duplicate vals in cols
            */
            for (int k = col + 1; k < size; k++)
            {

                // board[j][i] == board[k][i]
                if (*(*(board + col) + row) == *(*(board + k) + row) && *(*(board + col) + row) != 0)
                {
                    return 0; // return false if duplicates
                }
            }
        }
    }
    return 1; // return true
}
/*
 * This program prints "valid" (without quotes) if the input file contains
 * a valid state of a Sudoku puzzle board wrt to rows and columns only.
 *
 * A single CLA which is the name of the file that contains board data
 * is required.
 *
 * argc: the number of command line args (CLAs)
 * argv: the CLA strings, includes the program name
 */
int main(int argc, char **argv)
{
    // TODO: Check if number of command-line arguments is correct.

    if (argc != 2)
    {
        printf("Usage: ./check_board <input_filename>");
        exit(1);
    }

    // Open the file and check if it opened successfully.
    FILE *fp = fopen(*(argv + 1), "r");
    if (fp == NULL)
    {
        printf("Can't open file for reading.\n");
        exit(1);
    }

    // Declare local variables.
    int size;

    // TODO: Call get_board_size to read first line of file as the board size.
    get_board_size(fp, &size);

    // Dynamically allocate a 2D array for given board size.
    int i;                                    // init var i
    int **arr = malloc(sizeof(int *) * size); // init 2D array (dynamically allocated)

    // populate the array
    for (i = 0; i < size; i++)
    {
        *(arr + i) = malloc(sizeof(int) * size);
    }

    // Read the rest of the file line by line.
    // Tokenize each line wrt the delimiter character
    // and store the values in your 2D array.
    char *line = NULL;
    size_t len = 0;
    char *token = NULL;
    for (int i = 0; i < size; i++)
    {

        if (getline(&line, &len, fp) == -1)
        {
            printf("Error while reading line %i of the file.\n", i + 2);
            exit(1);
        }

        token = strtok(line, DELIM);
        for (int j = 0; j < size; j++)
        {

            // init 2D array
            *(*(arr + i) + j) = atoi(token);
            token = strtok(NULL, DELIM);
        }
    }

    /** Call the function valid_board and print the appropriate
    output depending on the function's return value.
    */
    if (valid_board(arr, size) == 1)
    {
        printf("The Sudoku board is valid.\n");
    }
    else
    {
        printf("The Sudoku board is invalid!\n");
    }

    // Free all dynamically allocated memory.
    free(arr);

    // Close the file.
    if (fclose(fp) != 0)
    {
        printf("Error while closing the file.\n");
        exit(1);
    }
    return 0;
}

// s22
