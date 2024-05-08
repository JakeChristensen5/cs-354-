////////////////////////////////////////////////////////////////////////////////
// Main File:        cache1D.c Iterate through arr and set index in arr to value to its index.
//
// This File:        cache1D.c
// Other Files:      N/A
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
#define GLOBAL_SIZE 100000


int array[GLOBAL_SIZE];


int main(){

  for(int i = 0; i < GLOBAL_SIZE; i++){
    array[i] = i;
  }

}
