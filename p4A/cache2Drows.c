////////////////////////////////////////////////////////////////////////////////
// Main File:        cache2Drows.c 2D array to traverse each element in arr to sum of its row-wise order.
//
// This File:        cache2Drows.c
// Other Files:      (name of all other files if any)
// Semester:         CS 354 Lecture 00? Fall 2022
// Instructor:       deppeler
//
// Author:           Jake Christensen
// Email:            Jrchristens2@wisc.edu
// CS Login:         jakec
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