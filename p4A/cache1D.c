#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define GLOBAL_SIZE 100000


int array[GLOBAL_SIZE];


int main(){

  for(int i = 0; i < GLOBAL_SIZE; i++){
    array[i] = i;
  }

}
