#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ROWS 3000
#define COLS 500

int arr[ROWS][COLS];

int main()
{

  for (int i = 0; i < ROWS; i++)
  {

    for (int j = 0; j < COLS; j++)
    {

      arr[i][j] = i + j;
    }
  }
}
