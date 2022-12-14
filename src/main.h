#ifndef __MAIN_H_
#define __MAIN_H_

#define BUFFER_SIZE 10000
#define LINE_BUFFER_SIZE 100

typedef struct{
    int base_address;
    char hex;
    char input_file_name[100];
    char output_file_name[100];
}__global_states_type;

extern __global_states_type __global_states;

int __find_substr(int argc, char *argv[], const char *_str);
int _option_handle(int argc, char *argv[]);
int _read_file(char *filename, char *buffer);
int _save_file(char *filename, char *buffer);
#endif  //__MAIN_H_
