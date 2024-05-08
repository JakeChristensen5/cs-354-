#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure that represents a magic square
typedef struct
{
    int size;           // dimension of the square
    int **magic_square; // pointer to heap allocated magic square
} MagicSquare;

/* TODO:
 * Prompts the user for the magic square's size, reads it,
 * checks if it's an odd number >= 3 (if not display the required
 * error message and exit), and returns the valid number.
 */
int getSize()
{

    // get user input and make sure its > = 3 and odd
    int num;

    printf("Enter magic square's size (>= 3 & ODD): ");
    scanf("%d", &num);

    // check conditions
    if (num < 3)
    {
        printf("Magic Square size must be >= 3.");
        exit(1);
    }
    else if (num % 2 == 0)
    {
        printf("Magic Square size must be odd.");
        exit(1);
    }
    else
    {
        return num;
    }
    return 0;
}
/* TODO:
 * Makes a magic square of size n using the alternate
 * Siamese magic square algorithm from assignment and
 * returns a pointer to the completed MagicSquare struct.
 *
 * n the number of rows and columns
 */
MagicSquare *generateMagicSquare(int n)
{
    // dynamically creating a struct pointer & allocate mem
    MagicSquare *MSPTR;                                 // creates pointer to structure
    MSPTR = (MagicSquare *)malloc(sizeof(MagicSquare)); // allocates memory for ptr

    // accessing variables
    MSPTR->size = n;
    // create a dynamic array of n rows & allocating it's memory
    MSPTR->magic_square = (int **)malloc(sizeof(int *) * n);

    if (MSPTR->magic_square != NULL) // memory allocation is successful
    {
        // Allocation of memory for matrix
        int i = 0;
        int j = 0;

        while (i < n)
        {

            // allocating memory for each of the rows (based on user input) on the heap.
            *((MSPTR->magic_square) + i) = (int *)malloc(sizeof(int) * n);

            if (*((MSPTR->magic_square) + i) == NULL)
            {

                while (j < i)
                {

                    // FREE ALL ALLOCATED MEM
                    free(MSPTR);
                    free(*((MSPTR->magic_square) + j));
                    free(MSPTR->magic_square);

                    return 0; // return null
                    j++;
                }
            }
            i++;
        }

        // Init magic_square array elements to => 0
        while (i < n)
        { // rows
            while (j < n)
            { // cols

                *(*((MSPTR->magic_square) + i) + j) = 0; // == magic_square[i][j] == 0
                j++;
            }
            i++;
        }

        // init row = first row; col to middle column (n/2)
        int r = 0;
        int c = n / 2;
        int element = 1;
        int dim = n * n;

        while (element <= dim)
        { // populates magic square

            *(*((MSPTR->magic_square) + r) + c) = element++; // magic_square[i][j] = element++;

            // get next row, col value
            if (r == 0 && c == n - 1)
            { // first row, size-1 col
                r++;
            } // move down 1 row

            else
            {
                r--; // up 1 row
                c++; // right 1 col
            }

            if (r < 0)
            {              // element outisde of row
                r = n - 1; // wrap around
            }

            if (c == n)
            {          // element outside of col
                c = 0; // wrap around
            }

            // magic_square[row][col]
            if (*(*((MSPTR->magic_square) + r) + c))
            { // when both row & col are filled

                c--; // move one col to the left

                // if row reached outside matrix, then wrap around
                if (r == n)
                {
                    r = 0;
                }
                r++;

                // if col is outside of matrix
                if (c < 0)
                {
                    c = n - 1;
                }

                r++; // move down

                // if row reached outside matrix, then wrap around
                if (r == n)
                {
                    r = 0;
                }

                continue; // next iteration of loop
            }
        }
        return MSPTR; // return ptr to struct
        element++;    // increment loop
    }
    else // mem allocation error
    {
        // free memory and return null
        free(MSPTR);
        return NULL;
    }
}

/* TODO:
 * Opens a new file (or overwrites the existing file)
 * and writes the square in the specified format.
 *
 * magic_square: the magic square to write to a file
 * filename: the name of the output file
 */
void fileOutputMagicSquare(MagicSquare *magic_square, char *filename)
{

    // open the file for writing
    FILE *file = fopen(filename, "w"); // creates a file ptr
    int size = magic_square->size;
    int i = 0, j = 0;
    // int equation = size * ((size*size) + 1)/2;

    if (file == NULL)
    {
        printf("No access to file for writing. \n");
        exit(1);
    }

    fprintf(file, "\nThe size of your magic square is: %d\n\n", size);
    // fprintf(file, "%d\n\n", equation);

    while (i < size)
    {
        for (j = 0; j < size; j++)

            fprintf(file, "%d\t", *(*((magic_square->magic_square) + i) + j));
        fprintf(file, "\n");
        i++;
    }

    fclose(file);
}

/* TODO:
 * Generates a magic square of the user specified size and
 * output the quare to the output filename
 *
 * Add description of required CLAs here
 */
int main(int argc, char **argv)
{

    if (argc != 2)
    {
        printf("Usage: ./myMagicSquare <input_filename>");
        exit(1);
    }

    // get size from input
    int n = getSize();

    char *fptr = argv[argc - 1]; // get output

    // Generate the magic square
    MagicSquare *generateSquare = generateMagicSquare(n);

    if (generateSquare == NULL)
    {
        printf("Magic Square failed to generate due to memory altercations\n");
    }

    if (generateSquare != NULL) // magic square created successfully
    {
        int size = generateSquare->size;
        // outputs to file
        fileOutputMagicSquare(generateSquare, fptr);

        for (int i = 0; i < size; i++)
        {

            free(*((generateSquare->magic_square) + i)); // frees mem in each row
        }
        free(generateSquare->magic_square); // free mem to magic_square
        free(generateSquare);               // free mem to object
    }

    return 0;
}
// F22 deppeler myMagicSquare.c
