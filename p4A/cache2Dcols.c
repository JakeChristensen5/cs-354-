#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ROWS 3000
#define COLS 500

int arr[ROWS][COLS];

int main(){

  for(int col = 0; col < COLS; col++){

    for(int row = 0; row < ROWS; row++){

      arr[row][col] = row + col;

    }
  }
}
