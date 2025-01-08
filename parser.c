#include<stdio.h>

//todo return list of instructions
int parse(char* file_path){
  FILE *file;
  char buffer[256];

  file = fopen(file_path, "r");
  if(!file){
    return 1;
  }

  return 0;
}
