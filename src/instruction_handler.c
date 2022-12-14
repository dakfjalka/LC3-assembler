#include "instruction_handler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char _BR_const[][6] = {
        "BR", "BRn", "BRz", "BRp", "BRnz", "BRzp", "BRnzp"
};

const char _BR_inttruction_const[][4] = {
        "111", "100", "010", "001", "110", "011", "111"
};

_label_type *__locate_label(_label_type *p_list, char *name){
    p_list = p_list->next;
    while(p_list != NULL){
        if(strcmp(p_list->name, name) == 0) return p_list;
        p_list = p_list->next;
    }
    return NULL;
}

int __get_rigister_index(char *str){
    if(str == NULL) return -1;
    else{
        if(strcmp(str, "R0") == 0)    return 0;
        else if(strcmp(str, "R1") == 0)    return 1;
        else if(strcmp(str, "R2") == 0)    return 2;
        else if(strcmp(str, "R3") == 0)    return 3;
        else if(strcmp(str, "R4") == 0)    return 4;
        else if(strcmp(str, "R5") == 0)    return 5;
        else if(strcmp(str, "R6") == 0)    return 6;
        else if(strcmp(str, "R7") == 0)    return 7;
        else return -1;
    }
}

void __register_int_to_bin(int register_index, char *dst){
    dst[3] = '\0';
    dst[2] = (register_index & 0x0001) == 0 ? '0' : '1';
    dst[1] = (register_index & 0x0002) == 0 ? '0' : '1';
    dst[0] = (register_index & 0x0004) == 0 ? '0' : '1';
}

int __imm_to_int(char *src, int *result){
    char str_buffer[20];
    char *p_src = str_buffer;
    if(src == NULL) return 0;
    strcpy(str_buffer, src);

    if(strcmp("0", p_src) == 0){
        *result = 0;
        return 1;
    }
    else if(strcmp("1", p_src) == 0){
        *result = 1;
        return 1;
    }
    else if(*p_src == '#'){
        int signal = 1;
        p_src ++;
        if(*p_src == '-'){
            signal = -1;
            p_src ++;
        }

        *result = 0;
        while(*p_src != '\0'){
            if(*p_src >= '0' && *p_src <= '9'){
                *result *= 10;
                *result += *p_src - '0';
            }else   return 0;
            ++ p_src;
        }
        *result *= signal;
        return 1;
    }else if(*p_src == 'b' || *p_src == 'B'){
        /**
         * @brief 这里我也不知道该怎么搞，我自己都不懂b100, b0100, x0FF, xFF是正的还是负的
         * 
         */
        return 1;
    }else if(*p_src == 'x' || *p_src == 'X'){
        p_src ++;
        while(*p_src != '\0'){
            *result *= 16;
            if(*p_src >= '0' && *p_src <= '9'){
                *result += *p_src - '0';
            }else if(*p_src >= 'a' && *p_src <= 'f'){
                *result += *p_src - 'a' + 10;
            }else if(*p_src >= 'A' && *p_src <= 'F'){
                *result += *p_src - 'A' + 10;
            }else return 0;
            ++ p_src;
        }
        return 1;
    }else   return 0;
}

int __int_to_2_com_bin(int value, int num_of_bits, char *dst){
    int i = 0, signal = value < 0 ? -1 : 1;
    uint16_t all_1 = (uint16_t)(((uint16_t)(1) << num_of_bits) - 1);
    uint16_t highest_bit = (uint16_t)((uint16_t)(1) << (num_of_bits - 1));
    uint16_t uint16_t_value = 0;

    if(num_of_bits <= 0 || num_of_bits > 16)    return 0;
    if(signal == -1)    value = - value;
    uint16_t_value = (uint16_t)(value & 0xFFFF);
    if(signal == -1)    uint16_t_value = all_1 - uint16_t_value + (uint16_t)(1); //取反再加一

    for(i = 0; i < num_of_bits; ++ i){
        dst[i] = (uint16_t_value & highest_bit) == 0 ? '0' : '1';
        highest_bit >>= 1;
    }
    dst[i] = '\0';
    return 1;
}

__err_info_type _set_offset(char *label, int line_address, int num_bits, _label_type *label_list, char *offset_buffer){
    _label_type *p_label = NULL;
    p_label = __locate_label(label_list, label);
    if(p_label == NULL){
        return LABEL_UNDEFINED;
    }else{
        int address_offset = p_label->address - line_address;
        if(__int_to_2_com_bin(address_offset, num_bits, offset_buffer) == 0){
            return OFFSET_TO_STRING_FAIL;
        }

    }
    return SUCCESS;
}

__err_info_type _set_register(char *register_name, char *register_buffer){
    int register_index = __get_rigister_index(register_name);

    if(register_index == -1){
        return INVALID_REGISTER;
    }else{
        __register_int_to_bin(register_index, register_buffer);
    }
    return SUCCESS;
}

__err_info_type _set_start_index(_line_type *p_line, char *instruction_name, int *i){
    if(strcmp(instruction_name, p_line->content[0]) != 0){
        if(strcmp(instruction_name, p_line->content[1]) != 0){
            return INAVLID_INSTRUCTION;
        }
        *i = 1;
    }
    return SUCCESS;
}

_err_type ADD_handler(_line_type *p_line, _label_type *label_list, _machine_code_type *machine_code){
    _err_type err = {p_line->origional_line, SUCCESS};
    int i = 0, rigister_index = -1;
    char R_buffer[4] = {'\0'};

    strcpy(machine_code->machine_code, "0001");
    err.info = _set_start_index(p_line, "ADD", &i);
    if(err.info != SUCCESS) return err;
    ++ i;

    err.info = _set_register(p_line->content[i], R_buffer);
    if(err.info != SUCCESS) return err;
    strcat(machine_code->machine_code, R_buffer);
    ++ i;

    err.info = _set_register(p_line->content[i], R_buffer);
    if(err.info != SUCCESS) return err;
    strcat(machine_code->machine_code, R_buffer);
    ++ i;

    rigister_index = __get_rigister_index(p_line->content[i]);
    if(rigister_index == -1){
        char immidiate_value_buffer[17] = {'\0'};
        int immidiate_value = 0;
        if(__imm_to_int(p_line->content[i], &immidiate_value) == 0){
            err.info = INVALID_IMMIDIATE_VALUE;
            return err;
        }
        if(__int_to_2_com_bin(immidiate_value, 5, immidiate_value_buffer) == 0){
            err.info = IMM_VALUE_TO_STRING_FAIL;
            return err;
        }
        strcat(machine_code->machine_code, "1");
        strcat(machine_code->machine_code, immidiate_value_buffer);
    }else{
        strcat(machine_code->machine_code, "000");
        err.info = _set_register(p_line->content[i], R_buffer);
        if(err.info != SUCCESS) return err;
        strcat(machine_code->machine_code, R_buffer);
    }
    return err;
}

_err_type AND_handler(_line_type *p_line, _label_type *label_list, _machine_code_type *machine_code){
    _err_type err = {p_line->origional_line, SUCCESS};
    int i = 0, rigister_index = -1;
    char R_buffer[4] = {'\0'};

    strcpy(machine_code->machine_code, "0101");
    err.info = _set_start_index(p_line, "AND", &i);
    if(err.info != SUCCESS) return err;
    ++ i;

    err.info = _set_register(p_line->content[i], R_buffer);
    if(err.info != SUCCESS) return err;
    strcat(machine_code->machine_code, R_buffer);
    ++ i;

    err.info = _set_register(p_line->content[i], R_buffer);
    if(err.info != SUCCESS) return err;
    strcat(machine_code->machine_code, R_buffer);
    ++ i;

    rigister_index = __get_rigister_index(p_line->content[i]);
    if(rigister_index == -1){
        char immidiate_value_buffer[17] = {'\0'};
        int immidiate_value = 0;
        if(__imm_to_int(p_line->content[i], &immidiate_value) == 0){
            err.info = INVALID_IMMIDIATE_VALUE;
            return err;
        }
        if(__int_to_2_com_bin(immidiate_value, 5, immidiate_value_buffer) == 0){
            err.info = IMM_VALUE_TO_STRING_FAIL;
            return err;
        }
        strcat(machine_code->machine_code, "1");
        strcat(machine_code->machine_code, immidiate_value_buffer);
    }else{
        strcat(machine_code->machine_code, "000");
        err.info = _set_register(p_line->content[i], R_buffer);
        if(err.info != SUCCESS) return err;
        strcat(machine_code->machine_code, R_buffer);
    }
    return err;
}

int _BR_is_BR(char *str){
    int i = 0;
    for(i = 0; i < 7; ++ i){
        if(strcmp(str, _BR_const[i]) == 0)    return i;
    }
    return -1;
}

_err_type BR_handler(_line_type *p_line, _label_type *label_list, _machine_code_type *machine_code){
    _err_type err = {p_line->origional_line, SUCCESS};
    int i = 0, BR_index = -1;
    char address_offset_buffer[10] = {'\0'};

    strcpy(machine_code->machine_code, "0000");
    if(_BR_is_BR(p_line->content[0]) == -1){
        if(_BR_is_BR(p_line->content[1]) == -1){
            err.info = INAVLID_INSTRUCTION;
            return err;
        }
        i = 1;
    }

    BR_index = _BR_is_BR(p_line->content[i]);
    strcat(machine_code->machine_code, _BR_inttruction_const[BR_index]);
    ++ i;

    err.info = _set_offset(
        p_line->content[i],
        p_line->physical_line,
        9,
        label_list,
        address_offset_buffer);
    if(err.info != SUCCESS) return err;
    strcat(machine_code->machine_code, address_offset_buffer);
    return err;
}

_err_type JMP_handler(_line_type *p_line, _label_type *label_list, _machine_code_type *machine_code){
    _err_type err = {p_line->origional_line, SUCCESS};

    return err;
}

_err_type JSR_handler(_line_type *p_line, _label_type *label_list, _machine_code_type *machine_code){
    _err_type err = {p_line->origional_line, SUCCESS};

    return err;
}

_err_type LD_handler(_line_type *p_line, _label_type *label_list, _machine_code_type *machine_code){
    _err_type err = {p_line->origional_line, SUCCESS};
    int i = 0, dst_register_index = -1;
    char PC_offset_buffer[10] = {'\0'};
    char dst_register_buffer[4] = {'\0'};
    _label_type *p_label = NULL;
    strcpy(machine_code->machine_code, "0010");
    if(strcmp("LD", p_line->content[0]) != 0){
        if(strcmp("LD", p_line->content[1]) != 0){
            err.info = INAVLID_INSTRUCTION;
            return err;
        }
        i = 1;
    }
    ++ i;
    dst_register_index =  __get_rigister_index(p_line->content[i]);
    if(dst_register_index == -1){
        err.info = INVALID_REGISTER;
        return err;
    }
    __register_int_to_bin(dst_register_index, dst_register_buffer);
    strcat(machine_code->machine_code, dst_register_buffer);
    ++ i;

    err.info = _set_offset(
        p_line->content[i],
        p_line->physical_line,
        9,
        label_list,
        PC_offset_buffer);
    if(err.info != SUCCESS) return err;
    strcat(machine_code->machine_code, PC_offset_buffer);
    return err;
}

_err_type LDI_handler(_line_type *p_line, _label_type *label_list, _machine_code_type *machine_code){
    _err_type err = {p_line->origional_line, SUCCESS};
    int i = 0, dst_register_index = -1;
    char PC_offset_buffer[10] = {'\0'};
    char dst_register_buffer[4] = {'\0'};
    _label_type *p_label = NULL;
    strcpy(machine_code->machine_code, "1010");
    if(strcmp("LDI", p_line->content[0]) != 0){
        if(strcmp("LDI", p_line->content[1]) != 0){
            err.info = INAVLID_INSTRUCTION;
            return err;
        }
        i = 1;
    }
    ++ i;
    dst_register_index =  __get_rigister_index(p_line->content[i]);
    if(dst_register_index == -1){
        err.info = INVALID_REGISTER;
        return err;
    }
    __register_int_to_bin(dst_register_index, dst_register_buffer);
    strcat(machine_code->machine_code, dst_register_buffer);
    ++ i;

    err.info = _set_offset(
        p_line->content[i],
        p_line->physical_line,
        9,
        label_list,
        PC_offset_buffer);
    if(err.info != SUCCESS) return err;
    strcat(machine_code->machine_code, PC_offset_buffer);
    return err;
}

_err_type LDR_handler(_line_type *p_line, _label_type *label_list, _machine_code_type *machine_code){
    _err_type err = {p_line->origional_line, SUCCESS};

    return err;
}

_err_type LEA_handler(_line_type *p_line, _label_type *label_list, _machine_code_type *machine_code){
    _err_type err = {p_line->origional_line, SUCCESS};

    return err;
}

_err_type NOT_handler(_line_type *p_line, _label_type *label_list, _machine_code_type *machine_code){
    _err_type err = {p_line->origional_line, SUCCESS};
    int i = 0;
    char register_buffer[4] = {'\0'};
    _label_type *p_label = NULL;

    strcpy(machine_code->machine_code, "1001");
    err.info = _set_start_index(p_line, "NOT", &i);
    if(err.info != SUCCESS) return err;
    ++ i;

    err.info = _set_register(p_line->content[i], register_buffer);
    if(err.info != SUCCESS) return err;
    strcat(machine_code->machine_code, register_buffer);
    ++ i;

    err.info = _set_register(p_line->content[i], register_buffer);
    if(err.info != SUCCESS) return err;
    strcat(machine_code->machine_code, register_buffer);

    strcat(machine_code->machine_code, "111111");

    return err;
}

_err_type RET_handler(_line_type *p_line, _label_type *label_list, _machine_code_type *machine_code){
    _err_type err = {p_line->origional_line, SUCCESS};

    return err;
}

_err_type RTI_handler(_line_type *p_line, _label_type *label_list, _machine_code_type *machine_code){
    _err_type err = {p_line->origional_line, SUCCESS};

    return err;
}

_err_type ST_handler(_line_type *p_line, _label_type *label_list, _machine_code_type *machine_code){
    _err_type err = {p_line->origional_line, SUCCESS};

    return err;
}

_err_type STI_handler(_line_type *p_line, _label_type *label_list, _machine_code_type *machine_code){
    _err_type err = {p_line->origional_line, SUCCESS};
    int i = 0;
    char register_buffer[4] = {'\0'};
    char PC_offset_buffer[10] = {'\0'};

    strcpy(machine_code->machine_code, "1011");
    err.info = _set_start_index(p_line, "STI", &i);
    if(err.info != SUCCESS) return err;
    ++ i;

    err.info = _set_register(p_line->content[i], register_buffer);
    if(err.info != SUCCESS) return err;
    strcat(machine_code->machine_code, register_buffer);
    ++ i;

    err.info = _set_offset(
        p_line->content[i],
        p_line->physical_line,
        9,
        label_list,
        PC_offset_buffer);
    if(err.info != SUCCESS) return err;
    strcat(machine_code->machine_code, PC_offset_buffer);
    return err;
}

_err_type STR_handler(_line_type *p_line, _label_type *label_list, _machine_code_type *machine_code){
    _err_type err = {p_line->origional_line, SUCCESS};

    return err;
}

_err_type HALT_handler(_line_type *p_line, _label_type *label_list, _machine_code_type *machine_code){
    _err_type err = {p_line->origional_line, SUCCESS};
    strcpy(machine_code->machine_code, "1111000000100011");
    return err;
}

_err_type TRAP_handler(_line_type *p_line, _label_type *label_list, _machine_code_type *machine_code){
    _err_type err = {p_line->origional_line, SUCCESS};
    int i = 0, tarpvector_value = -1;
    char trapvector_buffer[9] = {'\0'};

    strcpy(machine_code->machine_code, "1111");
    err.info = _set_start_index(p_line, "TRAP", &i);
    if(err.info != SUCCESS) return err;
    ++ i;

    strcat(machine_code->machine_code, "0000");

    if(__imm_to_int(p_line->content[i], &tarpvector_value) == 0){
        err.info = INVALID_TRAP_VECTOR;
        return err;
    }
    if(__int_to_2_com_bin(tarpvector_value, 8, trapvector_buffer) == 0){
        err.info = INVALID_TRAP_VECTOR;
        return err;
    }
    strcat(machine_code->machine_code, trapvector_buffer);

    return err;
}

_err_type FILL_handler(_line_type *p_line, _label_type *label_list, _machine_code_type **machine_code){
    _err_type err = {p_line->origional_line, SUCCESS};
    int i = 0, imm_value = 0;
    char imm_buffer[17] = {'\0'};

    err.info = _set_start_index(p_line, ".FILL", &i);
    if(err.info != SUCCESS) return err;
    ++ i;

    if(__imm_to_int(p_line->content[i], &imm_value) == 0){
        err.info = INVALID_IMMIDIATE_VALUE;
        return err;
    }

    if(__int_to_2_com_bin(imm_value, 16, imm_buffer) == 0){
        err.info = IMM_VALUE_TO_STRING_FAIL;
        return err;
    }

    strcpy((*machine_code)->machine_code, imm_buffer);

    return err;
}

_err_type BLKW_handler(_line_type *p_line, _label_type *label_list, _machine_code_type **machine_code){
    _err_type err = {p_line->origional_line, SUCCESS};
    int i = 0, imm_value = 0, counter = 0;
    _machine_code_type *p_machine_code = *machine_code;

    err.info = _set_start_index(p_line, ".BLKW", &i);
    if(err.info != SUCCESS) return err;
    ++ i;

    if(__imm_to_int(p_line->content[i], &imm_value) == 0){
        err.info = INVALID_IMMIDIATE_VALUE;
        return err;
    }

    for(counter = 0; counter < imm_value; ++ counter){
        strcpy(p_machine_code->machine_code, "0000000000000000");
        *machine_code = p_machine_code;
    	p_machine_code->next = (_machine_code_type *)malloc(sizeof(_machine_code_type));
    	p_machine_code = p_machine_code->next;
    	p_machine_code->next = NULL;
    }
    free(p_machine_code);
    (*machine_code)->next = NULL;
    return err;
}

_err_type STRINGZ_handler(_line_type *p_line, _label_type *label_list, _machine_code_type **machine_code){
    _err_type err = {p_line->origional_line, SUCCESS};
    int i = 0, imm_value = 0;
    char imm_buffer[17] = {'\0'}, *p_string = NULL;
    _machine_code_type *p_machine_code = *machine_code;

    err.info = _set_start_index(p_line, ".STRINGZ", &i);
    if(err.info != SUCCESS) return err;
    ++ i;
    
    p_string = p_line->content[i] + 1;
    while(*p_string != '\0' && *p_string != '\"'){
    	if(__int_to_2_com_bin(*p_string, 16, imm_buffer) == 0){
    		err.info = STRING_FAIL;
    		return err;
		}
    	strcpy(p_machine_code->machine_code, imm_buffer);
        *machine_code = p_machine_code;
    	p_machine_code->next = (_machine_code_type *)malloc(sizeof(_machine_code_type));
    	p_machine_code = p_machine_code->next;
    	p_machine_code->next = NULL;
    	p_string ++;
	}
    strcpy(p_machine_code->machine_code, "0000000000000000");
	*machine_code = p_machine_code;
    (*machine_code)->next = NULL;
    return err;
}

_err_type base_address_handler(int base_address, _machine_code_type *machine_code){
    _err_type err = {-1, SUCCESS};
    char base_address_buffer[17] = {'\0'};

    if(__int_to_2_com_bin(base_address, 16, base_address_buffer) == 0){
        err.info = INVALID_BASE_ADDRESS;
        return err;
    }

    strcat(machine_code->machine_code, base_address_buffer);

    return err;
}

