/**
 * @file assembler.c
 * @author PB21000xxx
 * @brief 
 * @version 0.3
 * @date 2022-12-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "assembler.h"
#include "assembler_consts.h"
#include "instruction_handler.h"


/**
 * @brief main.c 中的全局设置结构体。
 * 
 */
extern __global_states_type __global_states;


/**
 * @brief 三个链表销毁函数
 * 
 */
void _label_list_destory(_label_type label);
void _line_list_destory(_line_type line);
void _machine_code_list_destory(_machine_code_type machine_code);
/**
 * }
 */


/**
 * @brief 完成 assemble 的核心函数，包含4个环节。
 *        是这个文件里面唯一一个能被 main.c 调用的函数。
 * 
 * @param buffer 读入文件的内容。
 * @return int 0表示出错了，1表示正常完成。
 */
int assemble(char *buffer){
    _err_type err;
    _line_type line_list = {-1, -1, 0, {""}, NULL};
    _label_type label_list = {"", -1, NULL};
    _machine_code_type machine_code_list = {"", NULL};
    int base_address = 0, lines = 0, state = 1;

    while(1){
        err = _pre_process(buffer, &line_list);
        if(__raise_error(err) == 0){
            state = 0;
            break;
        }
        err = _first_path(&line_list, &label_list, &base_address);
        if(__raise_error(err) == 0){
            state = 0; break;
        }

        err = _second_process(&line_list, &label_list, base_address, &machine_code_list);
        if(__raise_error(err) == 0){
            state = 0; break;
        }

        err = _save_machine_code_string(&machine_code_list);
        if(__raise_error(err) == 0){
            state = 0; break;
        }
        break;
    }
    _line_list_destory(line_list);
    _label_list_destory(label_list);
    // _machine_code_list_destory(machine_code_list);
    printf("assemble done, state = %d\n", state);
    return state;
}
/**
 * @brief 链表销毁
 * 
 * @param line 带有头节点的 _line_type 链表
 */
void _line_list_destory(_line_type line){
    _line_type  *p = line.next, *temp = NULL;
    while(p != NULL){
        temp = p->next;
        free(p);
        p = temp;
    }
}


/**
 * @brief 链表销毁
 * 
 * @param label 带有头节点的 _label_type 链表
 */
void _label_list_destory(_label_type label){
    _label_type  *p = label.next, *temp = NULL;
    while(p != NULL){
        temp = p->next;
        free(p);
        p = temp;
    }
}


/**
 * @brief 同上
 * 
 * @param machine_code 
 */
void _machine_code_list_destory(_machine_code_type machine_code){
    _machine_code_type *p = machine_code.next, *temp = NULL;
    while(p != NULL){
        temp = p->next;
        free(p);
        p = temp;
    }
}


/**
 * @brief 报错函数，懒得完善了，就这样吧。
 * 
 * @param err _err_type 类型变量。
 * @return int 0表示有错，1表示没错。
 */
int __raise_error(_err_type err){
    if(err.info == SUCCESS) return 1;
    else{
        printf("line : %d , err code : %d\n", err.line, err.info);
        return 0;
    }
    return 1;
}


/**
 * @brief 预处理函数，解析读入文件buffer，删除注释，
 *          然后将其转化为 _line_type 链表。
 *          注意单独成行的标签会被加入下一行。
 *          如果没有 .ORIG 或 .END 会在这里被发现。
 * 
 * @param buffer 读入文件
 * @param line_list _line_type 链表
 * @return _err_type 错误信息
 */
_err_type _pre_process(char *buffer, _line_type *line_list){
    _err_type  err = {-1, SUCCESS};
    int num_line = 0, i = 0, physical_line = 0, state = 0, word_index = 0, sigle_label_line = 0;
    char **_buffer = (char **)malloc(sizeof(char *) * NUM_OF_LINES);
    char *token = 0;
    char word_buffer[LINE_WORD_LEN] = {'\0'};

    for(i = 0; i < NUM_OF_LINES; ++ i)  _buffer[i] = (char *)malloc(sizeof(char) * LINE_WORD_NUM * LINE_WORD_LEN);
    /* 按照每行经行分割 */
    num_line = __split_str(buffer, "\n", _buffer);
    for(i = 0; i < num_line; ++ i){
printf("----------------\nline : %d\n", i);
        if(_buffer[i] != NULL){
            __dete_comment(_buffer[i]);             // 删除注释
            if(strlen(_buffer[i]) == 0) continue;   // 全是注释
            else{

                token = strtok(_buffer[i], " ");
                printf("token = %s\n", token);
                if(strcmp(token, ".ORIG") == 0 && state == 0){  // 找到 .ORIG
                    state = 1;
                }
                if(strcmp(token, ".END") == 0 && state == 1){
                    state = 2;
                }
                if(state != 0){
                    if(sigle_label_line == 1){
                        sigle_label_line = 0;
                    }else{
                        line_list->next = (_line_type *)malloc(sizeof(_line_type));
                        line_list = line_list->next;
                        line_list->physical_line = physical_line;
                        line_list->origional_line = i;
                        line_list->next = NULL;
                    }
                    while (token != NULL && word_index < LINE_WORD_NUM){
                        strcpy((line_list->content)[word_index], token);
printf("--%s--\n", token);
                        token =strtok(NULL, " ");
                        ++ word_index;
                    }
                    if(word_index == 1){
                        sigle_label_line = 1;
                        continue;
                    }
                    line_list->words_num = word_index;
                    ++ physical_line;
                    word_index = 0;
                }   // end if state != 0
            }   // end else
        }
    }

    if(state == 0){
        err.line = -1;
        err.info = NO_ORIG_FOUND;
    }else if(state == 1){
        err.line = -1;
        err.info = NO_END_FOUND;
    }

    for(i = 0; i < NUM_OF_LINES; ++ i)  free(_buffer[i]);
    free(_buffer);
    return err;
}


/**
 * @brief 将字符串按照 split_str 分割，然后依次存入 d_str。
 * 
 * @param s_str char * 变量，被分割的对象。由于是用 strtok 实现的，这会破坏 s_str。
 * @param split_str char * 变量，用于分割的符号，注意最后不会出现在结果中。
 * @param d_str char ** 类变量，需要预先给好空间。（是二维指针，不是二维数组）
 * @return int 
 */
int __split_str(char *s_str, const char *split_str, char **d_str){
    int lines = 0;
    char *p_line_end = s_str;

    p_line_end = strtok(s_str, split_str);
    while(p_line_end != NULL){
        strcpy(d_str[lines], p_line_end);
        ++ lines;
        p_line_end = strtok(NULL, split_str);
    }
    return lines;
}


/**
 * @brief 删除注释，顺遍删除开头的空格和制表符，
 *        同时将单词间的逗号、空格和制表符统统变为一个空格。
 * 
 * @param _str 需要删除注释的这一行
 */
void __dete_comment(char *_str){
    int state = 0, num_of_spaces = 0;
    char *p_slow = _str, *p_fast = _str;

    while(*p_fast == ' ' || *p_fast == '\t')    ++ p_fast;  // 除去句首空格

    while(*p_fast != '\0'){
        if(*p_fast == ';'){
            break;
        }
        if(*p_fast == ' ' || *p_fast == '\t' || *p_fast == ','){
            if(num_of_spaces == 0){
                *p_slow = ' ';
                ++ p_slow;
                num_of_spaces = 1;
            }
        }else{
            if(num_of_spaces == 1)  num_of_spaces = 0;
            *p_slow = *p_fast;
            ++ p_slow;
        }
        ++ p_fast;
    }
    *p_slow = '\0';
}


/**
 * @brief 判断是否为关键字
 * 
 * @param str 
 * @return int 0不是，1是指令，2是伪指令
 */
int __is_key_word(char *str){
    int i = 0;
    for(i = 0; i < sizeof(__instructions_string) / INSTRUCTION_STRING_LEN; ++ i){
        if(strcmp(str, __instructions_string[i]) == 0)  return 1;
    }
    for(i = 0; i < sizeof(__pseudo_ops) / PSEUDO_OPS_STRING_LEN; ++ i){
        if(strcmp(str, __pseudo_ops[i]) == 0)  return 2;
    }
    return 0;
}


/**
 * @brief 获取表示数字的字符串的值。只能弄正数
 * 
 * @param str 表示数字的字符串的值
 * @param scale 进制
 * @return int 值
 */
int ___str_to_int(char *str, int scale){
    int result = 0, temp_num = 0;
    while(*str != '\0'){
        result *= scale;
        if('0' <= *str && *str <= '9'){
            temp_num = *str - '0';
        }else{
            if('a' <= *str && *str <= 'z'){
                temp_num = *str - 'a' + 10;
            }else if('A' <= *str && *str <= 'Z'){
                temp_num = *str - 'A' + 10;
            }else{
                return -1;
            }
        }
        if(temp_num >= scale || temp_num < 0)    return -1;

        result += temp_num;
        ++ str;
    }
    return result;
}

/**
 * @brief 和上面的功能一样，但是可以判断是什么进制。
 * 
 * @param str 
 * @return int 
 */
int __get_const_value(char *str){
    switch(*str){
        case '#' :
            return ___str_to_int(str + 1, 10);
            break;
        case 'b' :
        case 'B' :
            return ___str_to_int(str + 1, 2);
            break;
        case 'x' :
        case 'X' :
            return ___str_to_int(str + 1, 16);
            break;
        case '1' :
            return 1;
            break;
        case '0' :
            return 0;
            break;
        default  :
            return -1;
    }
}


/**
 * @brief 获得地址增量每一行过后的地址增量，一般来说是1。
 * 
 * @param line 
 * @return int 
 */
int __inc_address(_line_type *line){
    int type = __is_key_word((line->content)[0]), i = 0;
    if(type == 0){      // 是标签
        i = 1;
        type = __is_key_word((line->content)[1]);
        if(type == 0)   return -1;
    }

    type = __is_key_word((line->content)[i]);
    if(type == 1){      // 指令，只占一行
        return 1;
    }else if(type == 2){// 伪指令
        if(strcmp((line->content)[i], ".FILL") == 0){
            return 1;
        }else if(strcmp((line->content)[i], ".BLKW") == 0){
            return __get_const_value((line->content)[i + 1]);
        }else if(strcmp((line->content)[i], ".STRINGZ") == 0){
            return strlen((line->content)[i + 1]) - 2;
        }
    }else   return -1;
}


/**
 * @brief 获取指令在 __instructions_string 中的下标值，不在就返回 -1。
 * 
 * @param str 
 * @return int 
 */
int __get_instruction_index(char *str){
    int index = 0;
    for(
        index = 0;
        index < sizeof(__instructions_string) / INSTRUCTION_STRING_LEN;
        ++ index){
        if(strcmp(str, __instructions_string[index]) == 0){
            return index;
        }
    }
    return -1;
}


/**
 * @brief 获取伪指令在 __pseudo_ops 中的下标值，不在就返回 -1。
 * 
 * @param str 
 * @return int 
 */
int __get_pseudo_op_index(char *str){
    int index = 0;
    for(
        index = 0;
        index < sizeof(__pseudo_ops) / PSEUDO_OPS_STRING_LEN;
        ++ index){
        if(strcmp(str, __pseudo_ops[index]) == 0){
            return index;
        }
    }
    return -1;
}


/**
 * @brief first path，获取 label_list 和 base_address。
 * 
 * @param line_list 
 * @param label_list 
 * @param base_address 
 * @return _err_type 
 */
_err_type _first_path(_line_type *line_list, _label_type *label_list, int *base_address){
    _err_type err = {-1, SUCCESS};
    char label_buffer[LABEL_NAME_SIZE];
    int state = 0, current_address = 0, address_incr = 0;
    _line_type *prior = NULL, *current = NULL;

    current = line_list->next;
    prior = line_list;

    while(current != NULL){
        if(strcmp((current->content)[0], ".ORIG") == 0 && state == 0){
            state = 1;
            *base_address = __get_const_value((current->content)[1]);
            __global_states.base_address = *base_address;
            if(*base_address == -1){
                err.line = current->origional_line;
                err.info = ORIG_INVALID_ADDR;
                break;
            }
            prior ->next = current->next;
            free(current);
            current = prior->next;
            continue;               // 否则就会可能导致current访问出错
        }
        if(strcmp((current->content)[0], ".END") == 0){
            state = 2;
            prior->next = NULL;
            while(current != NULL){
                prior = current;
                current = current->next;
                free(prior);
            }
            break;
        }
        if(state == 1){
            if(__is_key_word((current->content)[0]) == 0){     // 是label
                label_list->next = (_label_type *)malloc(sizeof(_label_type));
                label_list = label_list->next;

                label_list->address = current_address;
                label_list->next = NULL;
                strcpy((label_list->name), (current->content)[0]);
            }
            address_incr = __inc_address(current);
            if(address_incr < 0){
                err.line = current->physical_line;
                err.info = INVALID_WORD;
            }
            current_address += address_incr;
        }
        prior = current;
        current = current->next;
    }
    return err;
}


/**
 * @brief second path，根据first pat 获取的 label_list 和 base_address 翻译指令。
 * 
 * @param line_list 
 * @param label_list 
 * @param base_address 
 * @param machine_code_list 
 * @return _err_type 
 */
_err_type _second_process(_line_type *line_list, _label_type *label_list, int base_address, _machine_code_type *machine_code_list){
   int counter = 0, line_type = 0, i = 0, index = -1;
    _err_type err = {-1, SUCCESS};
    _line_type *p_line = line_list->next;

    machine_code_list->next = (_machine_code_type *)malloc(sizeof(machine_code_list));
    machine_code_list = machine_code_list->next;

    machine_code_list->next = NULL;

    err = base_address_handler(base_address, machine_code_list);
    if(err.info != SUCCESS) return err;

    while(p_line != NULL){
        machine_code_list->next = (_machine_code_type *)malloc(sizeof(machine_code_list));
        machine_code_list = machine_code_list->next;

        machine_code_list->next = NULL;

        i = 0;
        line_type = __is_key_word(p_line->content[i]);

        if(line_type == 0){
            i = 1;
            line_type = __is_key_word(p_line->content[i]);
        }
        if(line_type == 1){
            index = __get_instruction_index(p_line->content[i]);
            if(index >= 0 && index <= (sizeof(instruction_handler) / sizeof(_instruction_handler_type)))
                err = instruction_handler[index](p_line, label_list, machine_code_list);
            printf("instruction index : %d\n", index);
            if(err.info != SUCCESS) return err;
        }else if(line_type == 2){
            index = __get_pseudo_op_index(p_line->content[i]);
            if(index >= 0 && index <= (sizeof(pseudo_op_handler) / sizeof(_instruction_handler_type)))
                err = pseudo_op_handler[index](p_line, label_list, &machine_code_list);
            printf("pseudo_op index : %d\n", index);
            if(err.info != SUCCESS) return err;
        }
        printf("line : %d : \t%s\n", p_line->physical_line, machine_code_list->machine_code);
        counter ++;
        p_line = p_line->next;
    }
    return err;
}


/**
 * @brief 将翻译好了的机器码存入文件。
 * 
 * @param machine_code_list 
 * @return _err_type 
 */
_err_type _save_machine_code_string(_machine_code_type *machine_code_list){
    _err_type err = {-1, SUCCESS};
    FILE *p_file = NULL;

    machine_code_list = machine_code_list->next;

    p_file = fopen(__global_states.output_file_name, "w");
    printf("filename : %s\n", __global_states.output_file_name);
    if(p_file != NULL){
        while(machine_code_list != NULL){
            fprintf(p_file, "%s\n", machine_code_list->machine_code);
            printf("%s\n", machine_code_list->machine_code);
            machine_code_list = machine_code_list->next;
        }
        fclose(p_file);
    }else{
        err.info = CREAT_OUTPUT_FILE_FAIL;
        return err;
    }
    return err;
}
