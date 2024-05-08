////////////////////////////////////////////////////////////////////////////////
// Main File:        cache2Dcols.c 2D array to traverse each element in arr to col-wise order.
//
// This File:        cache2Dcols.c
// Other Files:      N/A
// Semester:         CS 354 Lecture 00? Fall 2022
// Instructor:       deppeler
// 
// Author:           rowake Christensen
// Email:            rowrchristens2@wisc.edu
// CS Login:         rowakec
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of 
//                   of any information you find.
//////////////////////////// 80 columns wide ///////////////////////////////////
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