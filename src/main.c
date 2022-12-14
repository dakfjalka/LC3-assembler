#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"
#include "main.h"

__global_states_type __global_states = {0};

const char _help_info[] = "\n\
This is a simple assembler for LC-3.\n\n\
Usage\n\
./assembler [OPTION] ... [FILE] ...\n\n\
Options\n\
-h : print out help information\n\
-f : the path for the input file\n\
-e : print out error information\n\
-o : the path for the output file\n\
-s : hex mode\n";

int main(int argc, char *argv[]){
    int state = 0, i = 0;
    FILE *p_file = NULL;
    char buffer[BUFFER_SIZE] = {'\0'};

    if(__find_substr(argc, argv, "-h") != -1){
        printf("%s\n", _help_info);
        return 0;
    }
    state = _option_handle(argc, argv);
    if(state == 0)  return 0;

    state = _read_file(__global_states.input_file_name, buffer);
    if(state == 0)  return 0;

    state = assemble(buffer);
    if(state == 0)  return 0;

    return 0;
}

int _option_handle(int argc, char *argv[]){
    int i = 0;

    i = __find_substr(argc, argv, "-f");
    if(i == -1){
        printf("error : No input file detected.\n");
        return 0;
    }else{
        if(i + 1 == argc){
            printf("error : No input file detected.\n");
            return 0;
        }
        strcpy(__global_states.input_file_name, argv[i + 1]);
    }

    i = __find_substr(argc, argv, "-o");
    if(i == -1){
        strcpy(__global_states.output_file_name, "a.bin");
    }else{
        if(i + 1 == argc){
            printf("error : No out file detected.\n");
            return 0;
        }
        strcpy(__global_states.input_file_name, argv[i + 1]);
    }

    i = __find_substr(argc, argv, "-s");
    if(i != -1){
        __global_states.hex = 1;
    }else{
        __global_states.hex = 0;
    }
    return 1;
}

int __find_substr(int argc, char *argv[], const char *_str){
    int i = 0;
    for(i = 0; i < argc; ++ i){
        if(strcmp(argv[i], _str) == 0)    return i;
    }
    return -1;
}

int _read_file(char *filename, char *buffer){
    FILE *p_file = NULL;
    char line_buffer[LINE_BUFFER_SIZE] = {'\0'};

    p_file = fopen(filename, "r");
    if(p_file != NULL){
        while(1){
            fgets(line_buffer, LINE_BUFFER_SIZE, p_file);
            strcat(buffer, line_buffer);
            if(feof(p_file))    break;
        }
        fclose(p_file);
    }else{
        printf("error : File %s is not found.\n", filename);
        return 0;
    }
    return 1;
}
